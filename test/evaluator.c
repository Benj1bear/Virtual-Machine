#include "../virtual machine/parser.c"
#define INPUT_LIMIT 1000

char* start_up_info="";

int main()
{
    char input[INPUT_LIMIT];
    printf("%s\n>>> ",start_up_info);
    eval_loop(input,INPUT_LIMIT);
    return 0;
}