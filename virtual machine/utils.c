/*
    Non modifiable utility functions used throughout the virtual machine
*/
#include <stdlib.h> // calloc
#include <string.h> // strlen
#include <ctype.h> // isdigit, isalnum
#include <stdio.h> // printf, NULL
#include "grammar.c"

typedef struct TOKEN_STRUCT
{
    int type;
    char* value;
} TokenType;
// lexer
typedef struct LEXER_STRUCT
{
    char value;
    char* source;
    int index;
    int length;
} LexerType;
// function definitions
int lexer_isrunning(LexerType* lexer);
char* char_to_string(LexerType* lexer);
void lexer_next(LexerType* lexer);
int compare_grammar(LexerType* lexer,char* compare);
LexerType* lexer_init(char* source);
TokenType* token_init(int type, char* value);
TokenType* next_token(LexerType* lexer);
TokenType* collect_token(LexerType* lexer);
TokenType* collect_string(LexerType* lexer);
TokenType* check_grammar(LexerType* lexer);

typedef struct FORM_STRUCT
{
    int type;
    int frame;
    TokenType* partial_form[MAX_FORM_SIZE]; // tokens only
    struct FORM_STRUCT* abstract_form; // evaluated prior to the form being evaluated
    char* message; // used for warnings and errors typically
} FormType;

FormType* form_init()
{
    FormType* form = calloc(1, sizeof(struct FORM_STRUCT));
    form->type = -1;
    return form;
}
/*
   gets the length of an array (won't work as expect if array decay occurs 
   e.g. when you pass in an array into a function it will only pass the 
   pointer to the first element and not the array)
*/
#define len(x)  (sizeof(x) / sizeof((x)[0]))

// helper function to convert the char into a string (char pointer) and move the lexer on
char* char_to_string(LexerType* lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = lexer->value;
    str[1] = '\0'; // null byte's needed to tell when string termination is i.e. for printf
    lexer_next(lexer);
    return str;
}
/*
    get the length up to the first null byte
*/
int int_len(int* x)
{
    int i = 0;
    while (x[i]){i++;}
    return i;
}

/* generalized version of char_to_string from lexer.c */
char* char_as_string(char value)
{
    char* str = calloc(2, sizeof(char));
    str[0] = value;
    str[1] = '\0'; // null byte's needed to tell when string termination is i.e. for printf
    return str;
}
/***************************
* other Internal utilities *
***************************/
/*
    prints the help message
*/
void help(char** instructions,int instruction_length)
{
    printf("\ninstruction_length: %d\n",instruction_length);
    if (instruction_length==1) // general information about the program
    {
        printf("\n");
        printf("This program is written an compiled in C for the default language components.\n");
        printf("The language is designed to be very flexible, meaning, if you want it to be\n");
        printf("ran on a virtual machine, compiled, interpreted, specific grammar extensions, etc.\n");
        printf("You can do this by using the \\- command which allows editing the program at runtime\n");
        printf("but you also have the option to compile the code to be used internally by program\n");
        printf("\n");
        printf("List of possible special commands and arguements to be used with the \\- command:\n");
        printf("\n");
        printf("%-10s - %s\n","?","prints this help message");
        printf("%-10s - %s\n","grammar","prints the current grammar");
        printf("%-10s - %s\n","debug","enters debug mode");
        printf("%-10s - %s\n","view","views the current grammar");
        printf("%-10s - %s\n","compile","compiles the current file");
        printf("%-10s - %s\n","exit","exits the program");
        printf("\n");
        return;
    }
    printf("Error: Invalid number of arguments for help command. Use 0 arguments.\n");
}
#define ASSIGN_GRAMMAR(index) \
start_grammar[index]=start; \
end_grammar[index]=end; \
collect_grammar[index]=collect; \
grammar_name[index]=name; \
return;

/* needs fixing for displaying the representation of the grammar i.e. end_grammar="\n" */
void view_grammar()
{
    // sizeof / sizeof could be a problem is they are not the same size individually
    for (int i = 0; i < sizeof(start_grammar) / sizeof(start_grammar[0]); i++)
    {
        printf("\nNAME: %s\nSTART: %s\nEND: %s\nCOLLECT: %d",grammar_name[i],start_grammar[i],end_grammar[i],collect_grammar[i]);
    }
}

int char_pointer_pointer_len(char** x)
{
    int i = 0;
    while (x[i]){i++;}
    return i;
}

void add_grammar(char* start, char* end, int collect, char* name)
{
    // check that the starting grammar dosen't already exist otherwise overwrite it
    int last_index=char_pointer_pointer_len(start_grammar);
    for (int i = 0; i < last_index; i++){if (strcmp(start_grammar[i],start)==0){ASSIGN_GRAMMAR(i)}}
    // allocate and assign
    ASSIGN_GRAMMAR(last_index-1); // -1 because it returns after
}
void remove_grammar(int index)
{
    int last_index=char_pointer_pointer_len(start_grammar)-1;
    for (int i = index; i < last_index; i++)
    {
        start_grammar[i]=start_grammar[i+1];
        end_grammar[i]=end_grammar[i+1];
        collect_grammar[i]=collect_grammar[i+1];
        grammar_name[i]=grammar_name[i+1];
    }
    // safe practice to set the last index to null rather than assume the array has an extra index
    start_grammar[last_index]=NULL;
    end_grammar[last_index]=NULL;
    collect_grammar[last_index]=-1;
    grammar_name[last_index]=NULL;   
}

void view_form()
{
    int index=0;
    while (FORMS[index]!=0)
    {
        printf("%d: ",index);
        int* temp=FORMS[index];
        for (int i = 0; i < int_len(FORMS[index]); i++){printf("%d ",temp[i]);}
        printf("\n");
        index++;
    }
}
void add_form(char* token_sequence,char* instruction_mapping)
{
    printf("To be implemented\n");
}
void remove_form(int index)
{
    printf("To be implemented\n");
}

#define HANDLE_ERROR else{printf("Error: Invalid arguments for grammar command.\n");}
/* 
    For viewing and modifying the modifiable grammar
*/
void grammar(char** instructions,int instruction_length)
{
    /* for debugging */
    // printf("----------------------");
    // // '\'+"n"; // need to be able to do this
    // for (int i = 0; i < 2; i++){printf("%c ",instructions[1][i]);}
    // printf("----------------------");
    // return;
    if (instruction_length > 7){printf("Error: Invalid number of arguments for grammar command. Use 1-7 arguments.\n");return;}
    if (instruction_length==1){view_grammar();}
    else if (instruction_length==7 && strcmp(instructions[1],"add")==0)
    {
        if (strcmp(instructions[1],"token")==0){add_grammar(instructions[3],instructions[4],atoi(instructions[5]),instructions[6]);}
        else if(strcmp(instructions[1],"form")==0){add_form(instructions[3],instructions[4]);}
        HANDLE_ERROR
    }
    else if (instruction_length==3 && strcmp(instructions[1],"remove")==0)
    {
        if (strcmp(instructions[1],"token")==0){remove_grammar(atoi(instructions[3]));}
        else if(strcmp(instructions[1],"form")==0){remove_form(atoi(instructions[3]));}
        HANDLE_ERROR
    }
    /* for debugging */
    // for (int i = 0; i < custom_grammar_length; i++)
    // {printf("Start: %s,End: %s,Collect: %d,Name: %s\n",start_grammar[i],end_grammar[i],collect_grammar[i],grammar_name[i]);}
}
/* 
    allows compiling sections of the program into machine code
*/
void compile(char** instructions,int instruction_length)
{
    printf("To be implemented\n");
}
/* 
    exits the program
*/
void exit_proxy(char** instructions,int instruction_length)
{
    if (instruction_length==1){exit(0);}
    printf("Error: Invalid number of arguments for internal function \\-exit. Use 1 or 2 arguments.\n");
}

/* 
    Restarts the session
*/
void restart(char** instructions,int instruction_length)
{
    printf("To be implemented\n");
    // stop all threads
    // clear everything from memory
}
/* user won't be able to modify these at run time */
char* internals_keys[]={"?","grammar","compile","exit","restart"};
void (*internals_values[])(char**, int) = {help,grammar,compile,exit_proxy,restart};
// this is arbitary, it depends on how many args you want
#define MAX_COMMAND_ARGS 10
/* 
    command_parse takes in an array of instructions and a set
    of functions that can be used to parse the instructions

    Note: in c, when you pass in an array (i.e. char* keys[])
    only a pointer to the first element is passed in. This is
    called array decay. Because of this we have to pass in it's
    size separately.
*/
void command_parse(char* instructions,char* keys[],void (*values[])(char**, int),int array_size)
{
    /* 
        the instruction array can be fixed to the most 
        amount of args there are used for the internal 
        functions should be enough memory allocated
    
       Note: internal commands are strict e.g. no error handling on them. They have to always 
       be correctly formmated since they are a direct manipulation of the programs internals
       and the rationale is that typically you have a config setup or changing them manually
       rather than changing the program at runtime through variables because that's not explicit */

    if (isspace(instructions[0])){printf("SyntaxError: Invalid command '%s'. Ensure there are no leading spaces for the first arguement\n",instructions);return;}
    // char* instruction_array[10]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    /* calloc returns a char* and we want a pointer so it becomes char** */
    char** instruction_array = calloc(MAX_COMMAND_ARGS, sizeof(char*));
    int space=0; // indicates if a space has occured
    int instruction_length=0;
    for (int i = 0; i < strlen(instructions); i++)
    {
        char c =instructions[i];
        // collect instructions
        char* collected_value = calloc(1, sizeof(char));
        collected_value[0] = '\0';
        char* value;
        while (c != ' ' && c != '\n' && c != '\0')
        {
            value = char_as_string(c);
            collected_value = realloc(collected_value, (strlen(collected_value) + strlen(value) + 1) * sizeof(char));
            strcat(collected_value, value);
            c = instructions[++i];
        }
        // if it changed insert it into the array
        if (collected_value[0] != '\0')
        {
            instruction_array[instruction_length] = collected_value;
            instruction_length++;
        }
        // formmatting
        if (c == '\n') {break;}
    }
    // if there are instructions execute them
    if (instruction_length)
    {
        /* for debugging */
        // for (int i = 0; i < instruction_length; i++){printf("%s ||", instruction_array[i]);}
        for (int i = 0; i < array_size; i++)
        {
            if (strcmp(instruction_array[0], keys[i]) == 0)
            {return values[i](instruction_array,instruction_length);}
        }
    }
}
/***************************************
* linked list node type and functions  *
*    specifically for a hash table     *
***************************************/
#define TABLE_SIZE 100

typedef struct node {
    char* key;
    void* value;
    struct node* next;
} Node;

// there might be better ways of doing this but a struct will do for now if I really want a HashTable type
typedef struct HASHTABLE_STRUCT {Node** table;} HashTable;

HashTable* table_init() {
    HashTable* table = calloc(1, sizeof(HashTable));
    table->table = calloc(TABLE_SIZE, sizeof(Node*));
    return table;
}

/***********************
*      Hash Table      *
***********************/
/* 
    To make a hash table we make an array where the 
    indexes are determined by a hash of the keys.

    The simpler the hash function the better the performance
    but also the more likely for collisions (to keys with same 
    hash, therefore, have the same index; when they shouldn't).

    The goal is to get as simple a hash function with as few 
    collisions as possible.

    Therefore, typically it's said that on average a hash table
    is roughly constant time (if no collisions) or linear if 
    there is (e.g. it's will traverse a linked list of the keys
    and do a string compare to figure out which node has the value).
*/

int hash(char* key) {
    int hash=0;
    for (int i = 0; i < strlen(key); i++) {hash+=key[i];}
    return hash % TABLE_SIZE;
}

void table_set(HashTable* table, char* key, void* value) {
    Node* node=calloc(1, sizeof(Node*));
    node->key=key;
    node->value=value;
    int index=hash(key);
    /* make them point to each other */
    node->next=table->table[index];
    table->table[index]=node;
}

void* table_get(HashTable* table, char* key)
{
    Node* node=table->table[hash(key)];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0){return node->value;}
        node=node->next;
    }
    return NULL;
}

void table_delete(HashTable* table, char* key) {
    int index=hash(key);
    Node* node=table->table[index];
    Node* prev=NULL;
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (prev == NULL) {table->table[index] = node->next;} 
            else {prev->next=node->next;}
            free(node);
            return;
        }
        prev=node;
        node=node->next;
    }
}

void free_table(HashTable* table)
{
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* node = table->table[i];
        while (node != NULL) {
            Node* next = node->next;
            free(node);
            node = next;
        }
    }
    free(table->table);
    free(table);
}