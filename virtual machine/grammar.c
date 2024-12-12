/* 
    Lexers grammar and parsers forms

    Grammars:

    Grammars are the individual tokens in relation to the source code
    that form its tokenization or categorization into more widely or 
    specifically accepted grammars that make up the language.

    There are two types of grammars:
    1. custom grammar (overrides (most) builtin grammar and can be changed by the user at runtime)
    2. builtin grammar (cannot be changed by the user at runtime)

    Note: all grammars are unique therefore, in particular,
    the custom grammars start grammar must be unique.

    The custom grammar is stored in the arrays:
     - start_grammar
     - end_grammar
     - collect_grammar
     - grammar_name

    and is modified by the user via 
    
    \-grammar add *start* *end* *collect* *name*

    Modifiable grammars are grammars greater than TOKEN_MODIFIABLE 
    (except the DEFAULT_GRAMMAR_SIZE) and builtin grammars are less 
    than TOKEN_MODIFIABLE.

    Modifying the grammar is only to change how the token is 
    collected and doesn't add new functionality immmediately.
    In other words, you get to customize if a token should be
    treated as a i.e. comment, collection of something, or operator.

    When you modify grammar you're modifying the custom grammar 
    arrays that take precedence over the default modifiable grammars.

    Note: the more custom grammars you add the longer it will take to tokenize
    because it has to do a linear search for the matches and then perform a 
    linear search on each match to compare the characters to match the grammar.

    Forms:

    Forms are basically abstractions of the grammar that act like an abstract
    syntax tree, however, for efficiency a full tree is not formed but rather
    a series of forms are generated (e.g. it's a generator) from grammars (tokens)
    and then evaluated into executable forms.

    There are two types of forms:
    1. executing forms  - maps to an executable set of instructions
    2. abstract forms - used to recurse until a complete form is made

    A form has an integer that communicates doing something with the token/s 
    within a partial form within its scope but doesn't actually do anything 
    by itself since every form maps to a set of instructions that executes it.

    Abstract forms are forms within forms.

    If an ABSTRACT_FORM is being matched it will do a one index look ahead to 
    see if it's a complete form or if it needs to do recursion for a form.

    All forms are stored in the FORMS array where each index represents the form
    and the corresponding index in the EXEC_FORMS array (the instructions used to
    executed the form)
    
    The FORMS array is used to match a sequence of tokens to a form.

     - If no match is found the parser raises an error.

     - If a single match is found that's the form used.
       - if an abstract form was encountered then this will be recursed on

     - If at least two matches are found the parser continues 
       to the next token until a unique match is found or the 
       max form length is reached.
    
     - if an abstract form is directly beside another abstract form
       an error will be raised since this doesn't make sense because
       you'd end up recursing on potentially irrelevant tokens/forms.

    Note: as with more custom grammars, more forms will add overhead.
*/
/**********************************
*         CUSTOM GRAMMAR          *
**********************************/

#define DEFAULT_OPERATORS_START "!","%","&","*","-","=","+","|","/",">","<"
#define DEFAULT_OPERATORS_END "","","","","","","","","","",""
#define DEFAULT_OPERATORS 2,2,2,2,2,2,2,2,2,2,2
#define DEFAULT_OPERATORS_NAME "TOKEN_NOT","TOKEN_PERCENT","TOKEN_AMPERSAND","TOKEN_ASTERISK","TOKEN_MINUS","TOKEN_EQUALS","TOKEN_PLUS","TOKEN_OR","TOKEN_SLASH","TOKEN_GREATER","TOKEN_LESS"

#define MAX_GRAMMAR_SIZE 200
// IMPORTANT: TOKEN_INTERNAL must come first
// custom grammar (can be changed by the user at runtime)
char* start_grammar[MAX_GRAMMAR_SIZE]={"\\-","#","/*",DEFAULT_OPERATORS_START}; // indicates the starting string of a grammar
char* end_grammar[MAX_GRAMMAR_SIZE]={"\n","\n" ,"*/",DEFAULT_OPERATORS_END}; // indicates the ending string of a grammar
// values are 0,1,2: skip, collect, operator
int collect_grammar[MAX_GRAMMAR_SIZE]={1,0,0,DEFAULT_OPERATORS}; // indicates if the grammar should be skipped, collected, or used as an operator
// name of the grammar
char* grammar_name[MAX_GRAMMAR_SIZE]={"INTERNAL_MODIFIER","DEFAULT_COMMENT","DEFAULT_MULTILINE_COMMENT",DEFAULT_OPERATORS_NAME};

/* consts are separate from the rest of the grammar, they are ids that can be used as values or statements */
char* consts[MAX_GRAMMAR_SIZE]={"True","False","None"};

/**********************************
*         DEFAULT GRAMMAR         *
**********************************/
// builtin grammar (cannot be changed by the user at runtime, but some can be overrided or combined)
// IMPORTANT: INTERNAL must come first, DEFAULT_GRAMMAR_SIZE must come last
enum DEFAULT_GRAMMAR
{
    TOKEN_LINE_CONTINUATION=-3, // '\'
    TOKEN_NEWLINE, // '\n'
    SYNTAX_ERROR, // or FORM_ERROR
    INTERNAL, // allows for viewing and modifying the internal structure of the lexer
    ABSTRACT_FORM, // indicates an abstract formation of tokens (used in the parser)
    OBJECT, // a fully formed object
    TOKEN_EOF, // determines end of tokenization
    TOKEN_SKIP, // used for skipping/passing
    TOKEN_ERROR, // used for errors but may also be used to skip parts of the lexer
    // collections
    TOKEN_WHITESPACE,
    TOKEN_ID,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_CONST,
    // modifiable tokens
    TOKEN_MODIFIABLE,
    // parentheses
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LCBRACE,
    TOKEN_RCBRACE,
    // operators and other symbols
    TOKEN_BACKTICK,
    TOKEN_TILDE,
    TOKEN_NOT,
    TOKEN_AT,
    TOKEN_HASH,
    TOKEN_DOLLAR,
    TOKEN_PERCENT,
    TOKEN_CARET,
    TOKEN_AMPERSAND,
    TOKEN_ASTERISK,
    TOKEN_UNDERSCORE,
    TOKEN_MINUS,
    TOKEN_EQUALS,
    TOKEN_PLUS,
    // TOKEN_BACKSLASH, // replaced with TOKEN_LINE_CONTINUATION
    TOKEN_OR,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_QUESTION,
    TOKEN_SLASH,
    TOKEN_DOT,
    TOKEN_GREATER,
    TOKEN_COMMA,
    TOKEN_LESS,
    // size of the enum
    DEFAULT_GRAMMAR_SIZE
};
/**********************************
*              FORMS              *
**********************************/

#define MAX_FORM_SIZE 10
#define MAX_FORM_ITEMS 100

/*
    formations used to identify each of the forms (can be changed)
*/
int FORMS[MAX_FORM_ITEMS][MAX_FORM_SIZE] = \
{
    {TOKEN_ID,TOKEN_OPERATOR,TOKEN_ID}, // Bin_op
    {TOKEN_ID,TOKEN_LPAREN}, // LOAD frame_init, STORE new_frame in globals
    {OBJECT,TOKEN_OPERATOR,ABSTRACT_FORM,OBJECT},
    // {ABSTRACT_FORM,FORM_VALUE,TOKEN_OPERATOR,ABSTRACT_FORM,FORM_VALUE}, // BIN_OP

    {0} // sentinel (signals the end of the array; don't remove)
};

/************************************
* all internal operations possible  *
* that can be executed by the forms *
************************************/
// Note: you cannot change the internal instructions at runtime
enum INSTRUCTIONS
{
    /* core operations - memory management and display */
    STORE = 2000,
    LOAD,
    BIN_OP,     // operate on values/memory - is the more pressing execution since how it's combined is important
    DELETE,
    EXT,       // external call - print to display, read from display, network requests etc.
};
/* frames and sections (I didn't add these to the enums since you should be able to make functions out of the instructions) */
// TYPE,       // adds a frame on variables that adds metadata - store and create frame,
// MACRO_LOAD, // paste code section in - control flow happens under this
// FRAME_LOAD // paste isolated reusable code section in 

/********************************************
* Every FORM maps directly to a set of      *
* instructions. These are not checked       *
* against the forms like the grammars       *
* did since they are directly executable    *
* code that the user can modify at run time *
*********************************************/
int EXEC_FORMS[MAX_FORM_ITEMS][MAX_FORM_SIZE] = \
{
    /* DEFAULT_FORMS */
    {0} // sentinel (signals the end of the array; don't remove)
};