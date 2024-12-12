/* 
    concrete syntax tree (cst) / tokenizer / lexer / grammar layout

    | Function       | line number|
    -------------------------------
    lexer_isrunning     - 28
    token_init          - 31
    lexer_init          - 38
    lexer_next          - 48
    COLLECTOR           - 60
    IS_CONST            - 74
    next_token          - 88
    token_type          - 116
    collect_token       - 120
    HAS_LEXER_ENDED     - 162
    collect_string      - 169
    compare_grammar     - 196
    SKIP                - 207
    check_grammar       - 211

*/
#include "utils.c"

/*********************************************************
*         Defining the token and lexer functions         *
*********************************************************/

int lexer_isrunning(LexerType* lexer){return (lexer->value != '\0' && lexer->index < lexer->length);}

// token and lexer setup
TokenType* token_init(int type, char* value)
{
    TokenType* token = calloc(1, sizeof(struct TOKEN_STRUCT));
    token->type = type;
    token->value = value;
    return token;
}
LexerType* lexer_init(char* source)
{
    LexerType* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->source = source;
    lexer->length = strlen(source);
    lexer->index = 0;
    lexer->value = source[lexer->index];
    return lexer;
}
/* move the lexer onto the next character */
void lexer_next(LexerType* lexer)
{
    if (lexer_isrunning(lexer)) // we have to deal with null bytes since c truncates it
    {
        lexer->index += 1;
        lexer->value = lexer->source[lexer->index];
    }
}
/**********************************************************
*    The main lexing function to tokenize source code     *
**********************************************************/
/* collects a series of same tokens based on a condition function*/
#define COLLECTOR(condition,token,macro) \
char* value = calloc(1, sizeof(char)); \
value[0] = '\0'; \
char* str; \
while (condition(lexer->value)) \
{ \
    str = char_to_string(lexer); \
    value = realloc(value, (strlen(value) + strlen(str) + 1) * sizeof(char)); \
    strcat(value, str); \
} \
macro \
return token_init(token, value);

/* checks if an ID is a constant */
#define IS_CONST \
int i=0; \
str=consts[0]; \
while (str!=NULL) /* should we add || str!="\0" ???*/  \
{ \
    if (strcmp(str,value)==0){return token_init(TOKEN_CONST, value);} \
    i++; \
    str=consts[i]; \
}

/* 
    Tokenizes based on a single character; it will use look 
    aheads to collect multiple characters if necessary
*/
TokenType* next_token(LexerType* lexer)
{
    if (lexer->value == '\0')
    {
        // lets you know when tokenization's done
        if (lexer->index == lexer->length){return token_init(TOKEN_EOF, "\0");}
        else
        {
            printf("Error: Unterminated string literal\n");
            return token_init(TOKEN_ERROR, NULL);
        }
    }
    if (lexer->value == '\n')
    {
        lexer_next(lexer);
        return token_init(TOKEN_NEWLINE, "\\n");
    }
    if (isspace(lexer->value)){COLLECTOR(isspace,TOKEN_WHITESPACE,NULL;);}
    // digits before ids (so that the varnames are correct regardless of what digits; in case wanting an algebra like syntax)
    if (isdigit(lexer->value)){COLLECTOR(isdigit,TOKEN_NUMBER,NULL;);}
    if (isalnum(lexer->value)){COLLECTOR(isalnum,TOKEN_ID,IS_CONST);}
    if (lexer->value == '"' || lexer->value == '\''){return collect_string(lexer);}
    // check for custom grammar
    TokenType* token=check_grammar(lexer);
    if (token->type != TOKEN_ERROR){return token;} // TOKEN_ERROR implies no custom tokens were found
    return collect_token(lexer);
}

#define token_type(case_type, type) case case_type: return token_init(type, char_to_string(lexer));
/* 
    switch statement that collects an individual token
*/
TokenType* collect_token(LexerType* lexer)
{
    switch (lexer->value)
    {
        /* parentheses */
        token_type('(', TOKEN_LPAREN);
        token_type(')', TOKEN_RPAREN);
        token_type('[', TOKEN_LBRACE);
        token_type(']', TOKEN_RBRACE);
        token_type('{', TOKEN_LCBRACE);
        token_type('}', TOKEN_RCBRACE);
        /* modifiable tokens */
        token_type('`', TOKEN_BACKTICK);
        token_type('~', TOKEN_TILDE);
        token_type('!', TOKEN_NOT);
        token_type('@', TOKEN_AT);
        token_type('#', TOKEN_HASH);
        token_type('$', TOKEN_DOLLAR);
        token_type('%', TOKEN_PERCENT);
        token_type('^', TOKEN_CARET);
        token_type('&', TOKEN_AMPERSAND);
        token_type('*', TOKEN_ASTERISK);
        token_type('_', TOKEN_UNDERSCORE);
        token_type('-', TOKEN_MINUS);
        token_type('=', TOKEN_EQUALS);
        token_type('+', TOKEN_PLUS);
        token_type('\\', TOKEN_LINE_CONTINUATION);
        token_type('|', TOKEN_OR);
        token_type(';', TOKEN_SEMICOLON);
        token_type(':', TOKEN_COLON);
        token_type('/', TOKEN_SLASH);
        token_type('?', TOKEN_QUESTION);
        token_type('.', TOKEN_DOT);
        token_type('>', TOKEN_GREATER);
        token_type(',', TOKEN_COMMA);
        token_type('<', TOKEN_LESS);
        default:
            printf("Error: Unexpected character '%c'\n", lexer->value);
            return token_init(TOKEN_ERROR, NULL);
    }
}

#define HAS_LEXER_ENDED \
if (lexer->value == '\0') \
{ \
    printf("Error: Unterminated string literal\n"); \
    return token_init(TOKEN_ERROR, NULL); \
}

TokenType* collect_string(LexerType* lexer)
{
    char reference_char=lexer->value;
    lexer_next(lexer); // to skip the " or ' chars
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';
    int back_slash = 0;
    // passes a through
    while (lexer->value != reference_char || back_slash) // backslashes allow for \" and \'
    {
        HAS_LEXER_ENDED;
        // get the backslash first because it'll get lost when char_to_string runs lexer_next
        if (lexer->value == '\\' && back_slash==0)
        {back_slash = 1;} 
        else if (back_slash==1)
        {back_slash = 0;}
        char* str = char_to_string(lexer);
        value = realloc(value, (strlen(value) + strlen(str) + 1) * sizeof(char));
        strcat(value, str);
    }
    lexer_next(lexer); // to skip the " or ' chars
    return token_init(TOKEN_STRING, value);
}
/* 
    compares two strings to see if they are the same
    specific for the lexer.
*/
int compare_grammar(LexerType* lexer,char* compare)
{
    char value;
    for (int i = 0; i < strlen(compare); i++)
    {
        value=lexer->source[lexer->index+i];
        if (value != compare[i]){return 1;}
    }
    return 0;
}
/* moves the lexer forwards to skip values */
#define SKIP(value) for (int j = 0; j < strlen(value); j++){lexer_next(lexer);}
/* 
    checks if the grammar is part of the modifiable tokens
*/
TokenType* check_grammar(LexerType* lexer)
{
    // compare the sequential values of the source from the value to determine the grammar
    for (int i = 0; i < len(grammar_name); i++)// while(1) // int i=0;i+=1
    {
        char* start=start_grammar[i];
        if (start==NULL){break;} /* ------ since start_grammar has a fixed size and can have NULLs */
        if (compare_grammar(lexer,start)==0)
        {
            // skip past the start
            SKIP(start);
            char* end=end_grammar[i];
            // input to collect
            int collect=collect_grammar[i];
            // 0: skip from start to end
            if (collect==0)
            {
                while (compare_grammar(lexer,end)){lexer_next(lexer);}
                SKIP(end);
                return token_init(TOKEN_SKIP, NULL);
            } 
            // 1: collect from start to end
            else if (collect==1)
            {
                char* value = calloc(1, sizeof(char));
                value[0] = '\0';
                char* str;
                while (compare_grammar(lexer,end))
                {
                    HAS_LEXER_ENDED;
                    str = char_to_string(lexer);
                    value = realloc(value, (strlen(value) + strlen(str) + 1) * sizeof(char));
                    strcat(value, str);
                }
                SKIP(end);
                if (i==INTERNAL){command_parse(value,internals_keys,internals_values,len(internals_keys));}
                return token_init(i, value);
            }
            // 3: custom operator from the grammar
            else if(collect == 2)
            {
                return token_init(TOKEN_OPERATOR, start); // +1 for 0 based indexing
            }
        }
    }
    return token_init(TOKEN_ERROR, NULL);
}