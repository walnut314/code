#include <stdio.h>
#include <stdint.h>

const struct State {
 uint32_t Out; // 6-bit output
 uint32_t Time; // 1 ms units
 uint32_t Next[4]; // list of next states
};

typedef const struct State State_t;

#define goN 0
#define waitN 1
#define goE 2
#define waitE 3

State_t FSM[4] = {
    {0x21, 30000, {goN,waitN,goN,waitN}},
    {0x22, 5000, {goE,goE,goE,goE}},
    {0x0C, 30000, {goE,goE,waitE,waitE}},
    {0x14, 5000, {goN,goN,goN,goN}}
};

void main(void) {
    uint32_t cs; // index of current state
    uint32_t input; // car sensor input
    Traffic_Init(); // initialize ports and timer
    cs = goN; // initial state
    while(1){
        // 1) set lights to current state's Out
        P4->OUT = (P4->OUT&~0x3F)|(FSM[cs].Out);
        // 2) specified wait for this state
        Clock_Delay1ms(FSM[cs].Time);
        // 3) input from car detectors
        input = (P5->IN&0x03);
        // 4) next depends on state and input
        cs = FSM[cs].Next[input];
    }
}

