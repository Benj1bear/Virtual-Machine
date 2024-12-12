/* builtin functions that allow the default program to function */
/* make an internal function that allows you to access any scope etc. */
#include "memory.c"

/* 
    evaluates the forms 

    Based on the forms it will get the corresponding 
    instructions and execute them with the partial forms.

    The evaluator should perform typically the following operations among other things:

    1. Load from or into memory     - everything is stored as a string, in frames or in the global scope, scope names are separated by spaces
    2. Perform operations on memory - operations are performed on strings basically
    3. Free memory                  - simply free it or free its contents as well

    4. Print to display

*/
/********************************************
*           Builtin types                   *
********************************************/
typedef struct TYPE_STRUCT
{
    char* name;
    int modifiable_operations;
    int modifiable_variables;
} Type;


/********************************************
* Internal functions used to evaluate forms *
********************************************/
void print(char* key)
{
    printf("To be implemented\n");
}

// void bin_op(FormType* form)
// {
//     // form = BIN_OP
//     // 1. retrieve all variables as FORM_VALUE
//     if (form->partial_form[0]->type==FORM_VARIABLE)
//     {
//         load(form->partial_form[0]);
//     }
//     // 1. look up the type of the first variable
//     TokenType* partial_form=form->partial_form[0];
//     if (partial_form->type==FORM_TYPED_VARIABLE)
//     {
//         partial_form=partial_form[1]->type;
//     }
//     load(partial_form);
//     printf("To be implemented\n");
// }
void call(FormType* form)
{
    printf("To be implemented\n");
}
void add_metadata(FormType* form)
{
    printf("To be implemented\n");
}



void eval(FormType* form)
{
    /*
        all an abstract form should do is require prior evaluations 
        to be done before the current form is evaluated.

        The FORMS will make use of the abstract forms partial form
    */
    if (form->abstract_form){eval(form->abstract_form);}
    /* go through the instructions */
    int* instructions=EXEC_FORMS[form->type];
    for (int i = 0; i < int_len(instructions); i++)
    {
        /* might make this an array or a hash table for it to be more dynamic for the user */
        switch (instructions[i])
        {
            case STORE:
                break;
            case LOAD:
                break;
            case BIN_OP:
                break;
            case DELETE:
                break;
            case EXT:
                break;
            default:
                printf("Instruction Error: Invalid instruction: %d\n",instructions[i]);
                return;
        }
    }
}