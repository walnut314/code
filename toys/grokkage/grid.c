#include <stdio.h>
#include <stdlib.h>
#include "grid.h"

struct cell {
    enum cell_type type;
    int data;
    int row;
    int col;
} cell;

struct grid {
    int dim;
    struct cell **cells; //[S_DIM][S_DIM];
} grid;


grid_ptr grid_new(int dim)
{
    int r, c;
    grid_ptr grid = malloc(sizeof(*grid));
    grid->dim = dim;
    grid->cells = (struct cell **) malloc(grid->dim * sizeof(struct cell));
    for (r = 0; r < grid->dim; ++r) {
        grid->cells[r] = (struct cell *) malloc(grid->dim * sizeof(struct cell));
    }
    for (r = 0; r < grid->dim; ++r) {
        for (c = 0; c < grid->dim; ++c) {
            grid->cells[r][c].row  = r;
            grid->cells[r][c].col  = c;
            grid->cells[r][c].data = 0;
            grid->cells[r][c].type = DATA;
        }
    }
    return grid;
}

void grid_show(grid_ptr grid)
{
    int r, c;
    if (grid == NULL) return;
    for (r = 0; r < grid->dim; ++r) {
        for (c = 0; c < grid->dim; ++c) {
            if (grid->cells[r][c].type == DATA) {
                printf(" %02d |", grid->cells[r][c].data);
            } else {
                printf("  M |");
            }
        }
        printf("\n");
    }
}

void grid_cell_encode(grid_ptr grid, int row, int col, int data, enum cell_type type)
{
    grid->cells[row][col].row  = row;
    grid->cells[row][col].col  = col;
    grid->cells[row][col].data = data;
    grid->cells[row][col].type = type;
}

void grid_cell_clr(grid_ptr grid, int row, int col)
{
    grid->cells[row][col].data = 0;
}

#if 0
void grid_destroy(grid_ptr *grid)
{
    int r;
    struct grid ***g = *g;
    for (r = 0; r < g->dim; ++r) {
        free(g->cells[r]);
    }
    g->dim = 0;
    free(g);
    grid = NULL;
}
#endif

#if 0
int main()
{
    grid_ptr g = grid_new(4);
    printf("grid: %d\n", g->dim);
    grid_show(g);
    //grid_destroy(&g);
}
#endif

