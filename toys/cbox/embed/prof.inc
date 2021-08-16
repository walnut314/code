
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


