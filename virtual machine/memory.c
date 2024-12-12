/* 
    does all the memory management for the user

    Main programs memory (basically, all memory is stored in frames)

    The main program has a globals hash table that is created first
    that stores everything the user creates and is open for the user
    to view and modify (including frames, threads, processes etc.)
    and then there are frames within it that are created when i.e. 
    functions are executed or whatever other custom defined behaviors
    are defined by the user.

    Frames can be thought of as sections of memory essentially, but,
    more specifically they are containing a hash table of values.

    Displaying a table does a linear search. Retrieval, insertion,
    deletion, are all on average tend to be constant time.

    All frames have:
    1. its full scope name
    2. dictionary of locals
*/
#include "lexer.c"

/*********************
*   Frame creation   *
*********************/
typedef struct FRAME_STRUCT
{
    char* frame_name;
    HashTable* locals; // can contain frames in here as well
} FrameType;

#define MAX_THREADS 10
FrameType* Threads[MAX_THREADS];
HashTable* globals; // globals has access to everything user defined

FrameType* frame_init(char* name)
{
    FrameType* frame = calloc(1, sizeof(struct FRAME_STRUCT));
    frame->frame_name = name;
    frame->locals = NULL;
    table_set(globals, name, frame);
    return frame;
}
void free_frame(FrameType* frame)
{
    free(frame->locals);
    table_delete(globals, frame->frame_name);
    free(frame);
}
/* 
    shorthand functions
*/
void store(char* key,void* value){table_set(globals, key,value);}
void* load(char* key){return table_get(globals, key);}
void del(char* key){table_delete(globals, key);}
void copy(char* key)
{
    void* value = load(key);
    void* value_copy = malloc(sizeof(value));
    memcpy(value_copy, value, sizeof(value));
    store(key, value_copy); // need a different way of storing
}

// a scope is a name of a frame
// if you do threading then you need to create a new frame for each thread
// frames are allocated when the function is called and freed when the function returns