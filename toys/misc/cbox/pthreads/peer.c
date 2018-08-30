#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define ARRAY_SIZE 10

typedef int matrix_t[ARRAY_SIZE][ARRAY_SIZE];

typedef struct {
    int         id;
    int         size;
    int         row;
    int         column;
    matrix_t    *MA;
    matrix_t    *MB;
    matrix_t    *MC;
} matrix_work_order_t;


void mult(int size,
          int row,
          int column,
          matrix_t MA,
          matrix_t MB,
          matrix_t MC) 
{
    int position;

    MC[row][column] = 0;
    for (position = 0; position < size; position++) {
        MC[row][column] = MC[row][column] +
            ( MA[row][position] * MB[position][column] );
    }
}

void peer_mult(matrix_work_order_t *work_orderp)
{
    mult(work_orderp->size,
         work_orderp->row,
         work_orderp->column,
         *(work_orderp->MA),
         *(work_orderp->MB),
         *(work_orderp->MC));

    free(work_orderp);
}

int main()
{
    int size = ARRAY_SIZE, row, column, id, i;
    matrix_t MA, MB, MC;
    matrix_work_order_t *work_orderp;
    pthread_t peer[size*size];

    // fill in matrices
    
    for (row = 0; row < size; row++) {
        for (column = 0; column < size; column++) {

            id = column + (row*10);
            
            work_orderp = (matrix_work_order_t *) malloc(sizeof(matrix_work_order_t));
            work_orderp->id     = id;
            work_orderp->size   = size;
            work_orderp->row    = row;
            work_orderp->column = column;
            work_orderp->MA     = &MA;
            work_orderp->MB     = &MB;
            work_orderp->MC     = &MC;

            pthread_create(&(peer[id]), NULL, (void *) peer_mult, (void *) work_orderp);

        }
    }

    // wait for peers to finish
    for (i = 0; i < (size*size); i++) {
        pthread_join(peer[i], NULL);
    }

    // print matrix
    printf("MATRIX: resulting matrix C is:\n");
    for (row = 0; row < size; row++) {
        for (column = 0; column < size; column++) {
            printf("%5d ", MC[row][column]);
            mult(size, row, column, MA, MB, MC);
        }
        printf("\n");
    }

    return 0;
}

