#ifndef _STACK_INC_
#define _STACK_INC_

#ifdef _STACK_INIT // define this in primary module
    int cur_trace = 0;
    int max_trace = 0;
#else
    extern int cur_trace;
    extern int max_trace;
#endif

//#define INIT() \
//{ \
//    int cur_trace = 0; \
//    int max_trace = 0; \
//}

#define ENTER() \
{ \
    ++cur_trace; \
    if (cur_trace > max_trace) { \
        max_trace = cur_trace; \
    } \
}

#define EXIT() \
{ \
    --cur_trace; \
}

#define SHOW() \
{ \
    printf("cur: %d\n", cur_trace); \
    printf("max: %d\n", max_trace); \
}


#endif //_STACK_INC_
