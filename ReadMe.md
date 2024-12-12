# Virtual Machine

A template virtual machine for a base custom/customizable programming language.

The goal is to make a template virtual machine so that modifications can be made to create custom grammars and forms I want for a custom programming language.

I'm also considering adding functionality for compiling sections of text, tokens, or forms into machine code as well e.g. writing a compiler or otherwise transpiler (e.g. converts my code into i.e. C code an then compile that).

# Note:

This repo is for educational purposes. I don't mind if you use this for speeding up learning or inspiration to make your own programming language but this is beyond what you need to know and is more of a hobby for me as something I've been wanting to do for a long time.

Also, it's not a complete programming language (it's a template), and I'll likely add some more things and run some more tests but for the most part it's not really meant to be an executable as is but I'll add more support for other things that should be available and fix some things.

# how to use:

1. git clone the repo

2. open the terminal to the directory it's located

3. run:
 - ```make clean``` to remove any created files from running any of the following commands:
 - ```make lexer``` to test the lexer (tokenizer)
 - ```make parser``` to test the parser (creates forms from tokens)
 - ```make memory``` a simple test to see if the global memory is created as a hash table, a frame can be formed as a hash table in a linked list within the globals hash table, and can be retrieved.
 - ```make evaluator``` to test the eval_loop (runs the program; won't work since I haven't done much here)

Note: make sure to run ```make clean``` before you recompile because it can decide not to compile since the .exe is already up to date (from its point of view).

# Additional info:

Not everything in this implementation is as efficient as it could be or necessary for a language more tailored for what you may want. It's supposed to be that way as a template so that it's easier to modify since you have a base idea of how your lexer and parser are working with a basic implementation of the memory management.

This language by default is a stackless language since the intructions will map directly into functions that are not manipulating a stack object but you could modify the mappings of instructions into any data structure or algorithm for the evaluator beyond being a template support for anything you want to modify it into.

At the moment it's made of the following:
 - lexer
 - parser
 - memory
 - evaluator

# lexer

 - collection (gather or skip characters i.e. comments, strings, numbers, variable names etc.)
 - matching (gather a single character i.e. an operator token)

Essentially it reads charcter by character from the source code string to form matches. Each match becomes a struct called ```TokenType``` with a type that corresponds to an enum of token types and the original source code section it represents.

# parser

 - emergent matching of a sequence of tokens to form Forms.

It reads token by token until a single full match is found.

Emergent matching means the series of tokens are accumulated one by one where with each additional token that is added to the current partial form it's checked against the forms in the ```FORMS``` variable in grammar.c file to see which one it could be (it's not able to know right away which one, hence it uses emergent matching).

i.e. to illustrate emergent matching:

say these represent the forms:
```
| A | C | B | A | A |
| B | B | C | C | B |
| C | F | G | H | I |
```

(Note: the forms do not have to be exactly three tokens long. They can be different lengths.)

Each form is separated by ```|``` and reads vertically. Each of the letters represents a token or abstract form.

Then, this is how the algorithm would work for the sequence of tokens: A, B, C
(Note: numbers above them are just their examplar indexes in the FORMS array)
```
      0   1   2   3   4
    | A | C | B | A | A |
    | B | B | C | C | B |
    | C | F | G | H | I |

    Token to match: A
    row: 1

    (because we want a unique match we can stop after two matches have been found)

      Y   N   N   Y -----------stop
    | A | C | B | A | A |
       
         becomes

 CACHED     ||  START FROM LAST INDEX
  0   3     ||       4
| A | A |   ||     | A |
| B | C |   ||     | B |
       
         becomes

        Token to match: B
        row: 2
  Y   N              Y
| B | C |   ||     | A | - row 1 match against previous token e.g. A
                     Y
                   | B | - row 2 match against current token e.g. B
         becomes

 CACHED     ||  START FROM LAST INDEX
  0   4     ||
| A | A |   ||
| B | B |   ||
| C | I |   ||

         becomes

        Token to match: C
        row: 3

  Y   N
| C | I |   ||
```
Since one unique Form is found, it's index is the return value and therefore the algorithm returns 0 as the index of the form (essentially).

So, key points on how the parser works are:

 - the goal is to get one unique match
 - if two matches are found these are added to the cache, the current index is saved, and the algorithm continues
 - going through the cache either retains or removes from the cache

Forms also have a direct mapping to an array of instructions (since both the form and its instructions have the same index) used for the evaluation.

What this means in relation to the example is that the program would retrieve the instructions by using the index on a corresponding array that holds the instructions:

```EXEC_FORMS[0]```

```0``` Since this was the index the algorithm before computed.

# memory

 - storing and retrieving values at unique ids

The memory.c file itself is mostly definitions but semantically what memory does is initialize a global hash table and all values/objects get stored there.

Frames get stored there (variables in local scopes get stored on their frames).

If using threading or something that wants to make use of the same function it should create their own frame and then allocate the frames there to avoid duplicates. This still means the threads etc. can still access the global memory and if really wanted to you can let it access active frames in use since they'll be stored there (whether or not you want frames to be accessible by the user is up to you).

# evaluator

 - holds all the builtin functions
 - matches forms with their instructions to functions
 
The evaluator is mostly looking through the corresponding instructions array and then based on this utilizing the tokens in the forms partial form to execute code.

So it essentially runs a switch statement or uses a hash table or potentially just an array and then passes on the forms into the relevant function based on the match.

# To make into a complete language:

 - more instructions need implementation (evaluator.c)
 - more builtin functions and possibly additional memory management functions would be needed (i.e. for threading, processes, network requests, I/O etc.) (evaluator.c or memory.c)
 - you could upgrade the parser for using more advanced look aheads or other places where it's limited.
 - modify anything you want for your own language

# design overview at a high level

The program functions as a basic CLI. How it's designed is to:

1. Tokenize parts of user input into tokens. This step is sometimes also known as creating a concrete syntax tree (cst).

2. create an abstract syntax tree (ast). 

In this language the abstract syntax tree is not fully created then used, instead it's evaluated like a generator. Essentially: character by character for tokens, token by token for forms, and instruction by instruction for the evaluator.

I call the ast nodes 'forms'. Basically each form is a node in an abstract syntax tree that has it's own related partial form (array of tokens that makes it up) and the form itself has a direct mapping to a set of instructions that instruct it to do something internal for the user i.e. load into memory, take out of memory, or run an internal function with or without certain memory involved. It has an abstract form attribute that if used holds a child form that gets substituted into the parent form (on execution) (for recursive decent parsing).

Forms come in two types:

    1. executable:
        are mappings to a series of instructions
    
    2. abstract
        These are forms where the partial forms can (in concept/execution; it actually doesn't) include forms in them as well as tokens to make up their form. This will use recursion to get the next form and know when to stop based on a look ahead e.g. the form after an ABSTRACT_FORM will be the end form or token desired.

Forms map directly to a pre defined set of instructions. Forms are merely abstractions or high level definitions of what the user/developer thinks their code does but forms directly map into instructions that perform the executions based on the tokens and source code.

These instructions are evaluated one by one resulting in memory management, external comms, or printing to display (it all depends on what you've implemented as the builtins of your language).

So, the code follows this format:

Source -> tokens/grammars -> ast nodes / forms -> instructions -> directly exec'd

i.e. a typical configuration could evaluate as follows:

source:

"call(a,b,c)"

tokens:

TOKEN_ID,TOKEN_LPAREN,TOKEN_ID,TOKEN_COMMA,TOKEN_ID,TOKEN_COMMA,TOKEN_ID,TOKEN_COMMA,TOKEN_RPAREN

forms:

[TOKEN_ID,TOKEN_LPAREN] [TOKEN_ID,TOKEN_COMMA,TOKEN_ID,TOKEN_COMMA,TOKEN_ID,TOKEN_COMMA],TOKEN_RPAREN
```
(abstract form - ends at TOKEN_RPAREN)
FORM_CALL, -
            (abstract form - ends at TOKEN_RPAREN)
            FORM_ITEMS

evaluations:

-------------
- FORM_CALL -
-------------
LOAD frame_init
FrameType* new_frame = frame_init(\*function name\*)
STORE new_frame (in globals)
REFERENCE new_frame
--------------
- FORM_ITEMS -
--------------
new_frame->locals=\*list of function args and kwargs\*
... evaluate its source code
return the result and free the frame object
```

Generally for this program (before customization/ using it's defaults):

grammars     - mostly well defined (relates parts of text to parts of forms)

forms        - not well defined (relates series of tokens to semantics)

instructions - Mostly not well defined (relating forms to memory + display + external comms)

evaluation   - Mostly not well defined (relating instructions to builtin functions)

# internal commands:

I've made this language to be very flexible e.g. you can change the grammar of the language at run time and decide what it does. You can access some specific variables from the lexer, parser, memory, and evaluator that allows you to do this.

There's an internal command code (code 0 on the Default grammar enum e.g. INTERNAL)

i.e. At runtime grammars can be added via an internal command:

\\-grammar add token start_grammar end_grammar collect_grammar grammar_name

Forms are similar (once I implement it):

\\-grammar add form token_sequence instruction_mapping

I'll also add support for modifying the consts variable.

To get the help menu use:

\\-?
