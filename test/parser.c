#include "../virtual machine/parser.c"
#define INPUT_LIMIT 1000

/*
    collects the current form
*/
char* source_code(LexerType* lexer,int start_index)
{
    /*
        collect the string then print it
    */
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';
    char* str;
    for (int i=start_index; i < lexer->index; i++)
    {
        str = char_as_string(lexer->source[i]);
        value = realloc(value, (strlen(value) + strlen(str) + 1) * sizeof(char));
        strcat(value, str);
    }
    return value;
}

#define print_form(num,str) printf("FORM(%d,%s)\n",num,str)
#define default_print print_form(form->type,source_code(lexer,index))
/*
    Parses tokens into forms that can converted into executable forms
*/
void parse(LexerType* lexer)
{
    FormType* form;
    int index;
    while (lexer_isrunning(lexer))
    {
        index=lexer->index; // used in default_print
        form = next_form(lexer);
        if (form->type==SYNTAX_ERROR){default_print;return;}
        if (form->type==TOKEN_NEWLINE){print_form(TOKEN_NEWLINE,"\\n");return;}
        if (form->type==TOKEN_LINE_CONTINUATION){default_print;return;}
        default_print;
    }
}

char* start_up_info="";

int main()
{
    char input[INPUT_LIMIT];
    printf("%s\n>>> ",start_up_info);
    LexerType* lexer;
    while (fgets(input, INPUT_LIMIT, stdin))
    {
        lexer = lexer_init(input);
        parse(lexer); // is the only line that differs from test/lexer.c and this file
        printf(">>> ");
    }
    return 0;
}