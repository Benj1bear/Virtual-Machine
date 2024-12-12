#include "../virtual machine/memory.c"

int main()
{
    globals=table_init();
    FrameType* frame = frame_init("test");
    printf("%s\n", frame->frame_name);
    FrameType* temp_frame = table_get(globals, "test");
    printf("frame_name: %s\n", frame->frame_name);
    printf("frame_name: %s\n", temp_frame->frame_name);
    return 0;
}