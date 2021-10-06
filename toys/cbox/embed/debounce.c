#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define PIN_ANALOG_IN   (1)
#define MIN_HIGH        (3)
#define SAMPLE_SIZE     (8)

int analogRead(int pin)
{
    return 4;
}

bool switch_on()
{
    bool state = false;
    int i, sum = 0;
    for (i = 0; i < SAMPLE_SIZE; i++) {
        sum += analogRead(PIN_ANALOG_IN);
    }
    sum /= SAMPLE_SIZE;
    printf("sum: %d\n", sum);
    if (sum >= MIN_HIGH) {
        state = true;
    }
    return state;
}

int main()
{
    printf("switch is: %s\n", switch_on() ? "on" : "off");
}
