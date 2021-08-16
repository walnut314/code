#include <stdio.h>
#include <stdint.h>

// http://www.ti.com/lit/ml/swrp161/swrp161.pdf
//
struct State {
 uint32_t out; // 2-bit output
 uint32_t delay; // time to delay in 1ms
 const struct State *next[4]; // Next if 2-bit input is 0-3
};

typedef const struct State State_t;

#define Center &fsm[0]
#define Left   &fsm[1]
#define Right  &fsm[2]

State_t fsm[3]={
    {0x03, 50, { Right, Left, Right, Center }}, // Center
    {0x02, 50, { Left, Center, Right, Center }}, // Left
    {0x01, 50, { Right, Left, Center, Center }} // Right
};

State_t *Spt; // pointer to the current state
uint32_t Input; // 00=off, 01=right, 10=left, 11=on
uint32_t Output; // 3=straight, 2=turn right, 1=turn left

int main(void)
{
    //Clock_Init48MHz();
    //Motor_Stop(); // initialize DC motors
    Spt = Center;
    while(1){
        Output = Spt->out; // set output from FSM
        //Motor_Output(Output); // do output to two motors
        //Clock_Delay1ms(Spt->delay); // wait
        //Input = Reflectance_Center(1000); // read sensors
        Input = 2;
        Spt = Spt->next[Input]; // next depends on input and state
    }
}


