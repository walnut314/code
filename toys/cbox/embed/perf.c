
TIMESTAMP prof_timer;     // IO timer
F32 prof_elapsed_time = 0.0f; // IO time elapsed

int main()
{
    while (1) {

        RT_Timer_Get_Timestamp(&prof_timer); // start the timer
        do_something();
        prof_elapsed_time = RT_Timer_Get_Elapsed_Time(&prof_timer); // stop timer

        stream_puts(STREAM_STDIO, (U8 *)" , timing: ");
        stream_ftoa(STREAM_STDIO, prof_elapsed_time, printf_buffer);
        stream_puts(STREAM_STDIO, (U8 *)"\r\n");
        
    }
}
