/* usage
 *
 * static void Main_MCP_Run_Main_Loop(void)
 * {
 *     U32 frame; // Frame number
 *     TIMESTAMP frame_loop_timer; 
 * 
 *     TRACE_START(profiling_timer);
 *     Main_MCP_Run_Stage_2();
 *     TRACE_STOP("Stage 2", profiling_timer);
 * }
 *
 */

#include <stdio.h>
#include <string.h>

#define TRACE_END(msg, elapsed) \
{ \
    char buf[256] = {0}; \
    char tmp[32] = {0}; \
    strcat(buf, "\r\n");  \
    strncpy(buf, msg, strlen(msg)); \
    myftoa(elapsed, tmp);        \
    strcat((char *) buf, (char *) ": ");      \
    strcat(buf, tmp);      \
    strcat(buf, "\r\n\0");  \
    Stream_IO_Write(strlen(buf), buf, STREAM_STDIO); \
}

#define TRACE_START(timer) \
{ \
    RT_Timer_Get_Timestamp(&timer); \
}

#define TRACE_STOP(msg, timer) \
{ \
    static int tr_frame = 0; \
    F32 elapsed_time; \
    elapsed_time = RT_Timer_Get_Elapsed_Time(&timer); \
    if (tr_frame % 100 == 0) { \
        TRACE_END(msg, elapsed_time); \
    } \
    tr_frame++; \
}



#define PHASER_START2() \
{ \
    RT_Timer_Get_Timestamp(&stage_2_timer); \
}

#if 0
int myitoa(int value, char *sp, int radix)
{
    char tmp[16];
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);    
    if (sign) 
    {
        v = -value;
    } 
    else 
    {
        v = (unsigned) value;
    }

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix;
        if (i < 10)
        {
            *tp++ = i+'0';
        }
        else
        {
            *tp++ = i + 'a' - 10;
        }
    }

    int len = tp - tmp;

    if (sign) 
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
    {
        *sp++ = *--tp;
    }
    sp = '\0';

    return len;
}
#endif
char *strcat(char *string1, const char *string2)
{
   char       *s1 = string1;
   const char *s2 = string2;

   while (*s1) s1++; /* find end of string   */
   do
   {
       *s1++ = *s2++;
   }
   while (*s2 != 0); /* append second string */
   return string1;
   
} // mystrcat

void myftoa(float f, char *buf)
{
    int pos=0, ix, dp, num;
    if ( f < 0)
    {
        buf[pos++] = '-';
        f = -f;
    }
    dp = 0;
    while ( f >= 10.0f) 
    {
        f=f/10.0f;
        dp++;
    } 
    for (ix = 1; ix < 8; ix++)
    {
        num = f;
        f=f-num;
        if (num>9)
            buf[pos++]='#';
        else
            buf[pos++]='0'+num;
        if (dp==0) buf[pos++]='.';
        f=f*10.0f;
        dp--;
    }
}

#define INIT_STAGE2() \
{ \
    static int frame = 0; \
    static TIMESTAMP stage_2_timer = {0}; \
}

#define FINI_STAGE2() \
{ \
    frame++; \
}

#define PHASER_STOP2(msg) \
{ \
    profiling_time_elapsed = RT_Timer_Get_Elapsed_Time(&stage_2_timer); \
    if (frame/10 % 100 == 0) { \
    	char buf[256] = {0}; \
        char tmp[32] = {0}; \
        strcat(buf, "\r\n");  \
        strncpy(buf, msg, strlen(msg)); \
        myftoa(profiling_time_elapsed, tmp);        \
        strcat((char *) buf, (char *) ": ");      \
        strcat(buf, tmp);      \
        strcat(buf, "\r\n\0");  \
        Stream_IO_Write(strlen(buf), buf, STREAM_STDIO); \
    } \
}

