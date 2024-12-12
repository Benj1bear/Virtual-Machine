// abstract syntax tree (ast) / parser / scope layout

/*
    The parser takes in tokens and creates forms based
    on the sequence of tokens that emerges and based on 
    these forms it could:
    
    1. define types into memory
    2. load variables into memory
    3. load values into memory
    4. perform operations on memory
    5. free memory e.g. delete variables from memory
    6. create scopes (this is done automatically by the parser)
    7. print to display
    ---------------------------------
    - later
    ---------------------------------
    open files
    open network requests
    open systems
    access internals better - see what the tokenizer is doing, paser, memory, executor, what frames

    However, note that the forms are merely mappings to the executables
    so, they are conceptually executable code but specifically they act
    as pointers to a series of executable instructions.
*/

#include "evaluator.c"

/*******************************************
*   Defining the form matcher and parser   *
*******************************************/

int isform(int token,int form)
{
    if (form==ABSTRACT_FORM){return -1;}
    return (token==form);
}

/*********************************************
*  Go through all the forms to find a match  *
*********************************************/

#define INDICATOR matches++;if (matches == 2){i++;prev_index=i;break;}
#define BREAK(index) flag=index;break;
#define ERROR(error) form->type=SYNTAX_ERROR;form->message=error;return form;

/* retrieves the next form from the lexer */
FormType* next_form(LexerType* lexer)
{
    FormType* form=form_init();
    int form_index=-1; // -1 for checking MAX_FORM_SIZE and using 0 based indexing
    // cache the forms for efficiency
    int prev_index=0;
    int CACHED_FORMS[2]={-1,-1}; // list of current indexes
    /* other variables used */
    int matches,flag,i,token_ID;
    TokenType* token;
    while(1) // while it's not formed
    {
        /*
            Forms an array of partial forms (tokens)
        */
        form_index++;
        token=next_token(lexer);
        // check if the formation is not valid and if the lexer has finished or encountered an error before formation
        if (form_index==MAX_FORM_SIZE){ERROR("Max form size reached\n")}
        if (token->type==TOKEN_ERROR){ERROR("Syntax error\n")}
        form->partial_form[form_index]=token;
        if (token->type==TOKEN_EOF || token->type==TOKEN_NEWLINE)
        {
            if (form_index > 0){form->type=SYNTAX_ERROR;return form;}
            form->type=TOKEN_NEWLINE;return form;
        }
        if (token->type==TOKEN_LINE_CONTINUATION){form->type=TOKEN_LINE_CONTINUATION;return form;}
        /*
            Finds the unique form based on matching tokens sequentially
            in reference to the FORMS array

            1. go through the CACHED_FORMS (delete any that don't match) (increase matches if match)
            2. go through the remaining FORMS (add any to the cache that match) ...
            break whenever there's more than one match
        */
        /* CACHED_FORMS */
        // break or remove
        token_ID=token->type;
        matches=0;
        i=0;
        while (i < 2 && CACHED_FORMS[i] != -1)
        {
            if (isform(token_ID,FORMS[CACHED_FORMS[i]][form_index])){matches++;i++;continue;}
            // remove the index
            if (i==0){CACHED_FORMS[0]=CACHED_FORMS[1];CACHED_FORMS[1]=-1;}
            else{CACHED_FORMS[1]=-1;break;}
        }
        /* FORMS */
        if (matches < 2)
        {
            i = prev_index;
            int* reference_token=FORMS[i];
            while (reference_token[0]!=0)
            {
                for (int j = 0; j <= form_index; j++) /* <= since index is the actual index */
                {
                    token_ID=form->partial_form[form_index-j]->type;
                    flag=isform(token_ID,reference_token[form_index-j]);
                    /* abstract forms defer to the next token */
                    if (flag==-1)
                    {   /* if it's a matching form then nothing happens, if not, then it's an abstract form */
                        flag=isform(token_ID,reference_token[form_index+1]);
                        /*
                            matches used here is 1 less than what's in the switch 
                            statement since the INDICATOR macro increases it by 1
                        */
                        // you shouldn't have an abstract form next to another abstract form
                        if (flag==-1){matches=3;BREAK(1)}
                        // if it does or doesn't match it still could since it's an abstract form
                        if (!flag){matches=2;BREAK(1)}
                        else{BREAK(1)}
                    }
                    if (!flag){BREAK(-1)}
                }
                if (flag!=-1)
                {
                    // add the index
                    if (CACHED_FORMS[0]==-1){CACHED_FORMS[0]=i;}
                    else {CACHED_FORMS[1]=i;}
                    INDICATOR
                }
                i++;
                prev_index=i;
                reference_token=FORMS[i];
            }
        }
        /************************************************
        *  Assign the form based on the matches found   *
        ************************************************/
        switch (matches)
        {
            case 0: // no matches found
                ERROR("Form Error: No matches found\n")
            case 1: // match found
                flag=CACHED_FORMS[0];
                // it has to match exactly
                if (form_index+1!=int_len(FORMS[flag])){break;}
                form->type=flag;
                return form;
            case 2: // further matches are possible
                break;
            case 3: // it's an abstract form, but once done it should be of the indexes form
                // form->type=CACHED_FORMS[0];
                form->abstract_form=next_form(lexer);
                int* temp=FORMS[CACHED_FORMS[0]];
                if (form->abstract_form->type!=temp[form_index+1]){ERROR("Abstract Form error: Abstract form failed to match\n")}
                // go through the remaining tokens to get the full match (if not already)
                form_index++;
                if (form_index+1==int_len(temp)){return form;}
                break;
            case 4: /* SYNTAX_ERROR from having an abstract form next to another abstract form */
                ERROR("Abstract Form error: cannot have an abstract form next to another abstract form\n")
        }
    }
}
/*****************************
*    Main evaluation loop    *
*****************************/
/* 
    The main evaluation loop for the program

    Basically, from here it's doing memory 
    manipulation and printing to display where
    necessary.

    That's pretty much all there is to it. 
    Anything else is extra to help the program
    run better for the intended use cases.
*/
void eval_loop(char input[],int INPUT_LIMIT)
{
    globals=table_init();
    LexerType* lexer;
    while (fgets(input, INPUT_LIMIT, stdin))
    {  
        lexer = lexer_init(input);
        while (lexer_isrunning(lexer)){eval(next_form(lexer));}
        printf(">>> ");
    }
}