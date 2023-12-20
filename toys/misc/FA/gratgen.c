#include <stdio.h>

char *vegs[] = {
    "dude",
    "wusup"
};
char *prots[] = {
    "homi",
    "owzit"
};
char *gratits[] = {
    "wooda",
    "dawg"
};

void meals()
{
    printf("B: 8 oz yog, 6 oz berries, 2 oz oats\n");
    printf("L: 2 eggs, 6 oz %s, 6 oz salad, 1 T oil\n", vegs[0]);
    printf("D: 6 oz %s, 6 oz %s, 6 oz salad, 1 T oil\n\n", prots[1], vegs[1]);
}

void grats()
{
    printf("          Grats:\n");
    printf("1. %s\n", gratits[0]);
    printf("2. %s\n", gratits[1]);
    printf("3. %s\n", gratits[0]);
    printf("4. %s\n", gratits[1]);
    printf("5. %s\n", gratits[0]);
}

int main()
{
    printf("automatic gratitude/meal generator\n\n");

    meals();
    grats();
    
}
