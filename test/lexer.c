#include "../virtual machine/lexer.c"
#define INPUT_LIMIT 1000

/* prints the tokens as they come */
int token_print(LexerType* lexer)
{
    TokenType* token=next_token(lexer);
    while (token->type!=TOKEN_EOF)
    {
        if (token->type==TOKEN_WHITESPACE)
        {printf("TOKEN(%d,%*s)\n", token->type, strlen(token->value),"");}
        else
        {printf("TOKEN(%d,%s)\n", token->type, token->value);}
        token=next_token(lexer);
    }
    // for the EOF
    printf("TOKEN(%d,)\n", token->type, token->value);
    if (token->type==TOKEN_LINE_CONTINUATION){return 1;}
    return 0;
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
        token_print(lexer);
        printf(">>> ");
    }
    return 0;
}