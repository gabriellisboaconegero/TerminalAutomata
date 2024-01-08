#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "./termal.h"
#include "./patterns.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define WIDTH 100
#define HEIGHT 25

#define DEAD 0
#define ALIVE 1
#define DYING 2

#define GOL 0
#define SEEDS 1
#define BBRAIN 2
#define DAN 3
#define MAZE 4

#define FPS 30
#define GENPS 10

int term_width, term_height;
char *REP[] = {
    [ALIVE]="█",
    //[ALIVE]="#",
    [DYING]="&",
    [DEAD]=" "
};

char *AUTOMATON_NAME[] = {
    [GOL] = "Game Of Life",
    [SEEDS] = "SEEDS",
    [BBRAIN] = "Brian's BRAIN",
    [DAN] = "Day And Night",
    [MAZE] = "MAZE"
};

int *init_grid(int w, int h){
    int *grid = (int *)calloc(w*h, sizeof(int));
    memset(grid, DEAD, sizeof(int)*w*h);
    for (int i = 0; i < w*h; i++){
        if (!(rand() % 3))
            //grid[i] = ALIVE;
            grid[i] = DEAD;
    }

     return grid;
}

int neighbors_count(int *grid, int w, int h, int x, int y){
    int res = 0;
    int dx, dy;
    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            if (i == 0 && j == 0)
                continue;
            dx = (x + j + w) % w;
            dy = (y + i + h) % h;
            res += grid[dy*w + dx] == ALIVE;
        }
    }

    return res;
}

void next_gen(
        int *grid, int w, int h,
        int state_table[9][9],
        int (*neighbors_count)(int *, int, int, int, int))
{
    int *copy = (int *)calloc(w*h, sizeof(int));
    memcpy(copy, grid, sizeof(int)*w*h);
    int r;
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            r = neighbors_count(grid, w, h, j, i);
            copy[i*w+j] = state_table[grid[i*w+j]][r];
        }
    }
    memcpy(grid, copy, sizeof(int)*w*h);
    free(copy);
}

void render(int *grid, int w, int h){
    for (int i = 0; i < h; i++){
        moveCursor((term_width - w)/2, 1+(term_height - h)/2 + i);
        for (int j = 0; j < w; j++){
            printf("%s", REP[grid[i*w+j]]);
        }
        fflush(stdout);
    }
}

double timestamp(){
  struct timespec tp;
  clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
  return ( (double)tp.tv_sec + (double)tp.tv_nsec*1.0e-9 );
}

void exit_celular_automata(int s){
    (void)s;
    char msg[] = "CELULAR AUTOMATA - Clique qualquer tecla para sair";
    size_t msg_sz = sizeof(msg);
    clearScreen(FULL);
    moveCursor(term_width / 2 - msg_sz / 2, term_height / 2);
    printf("%s\n", msg);
    moveCursor(1, term_height - 1);
    usleep(10000);
    EXIT;
}

int set_grid_cell(int *grid, int w, int h, int x, int y, int state){
    if (!(0 <= x && x < w) ||
        !(0 <= y && y < h))
        return 0;

    grid[y*w+x] = state;
    return 1;
}

int main(int argc, char **argv){
    (void)argc;
    (void)argv;
    double time, last_render, last_gen_time;
    int quit = 0;
    struct Event e;
    int c, grid_width, grid_height;
    int *grid;
    int paused = 1, step = 0, curr_automaton;
    int (*curr_neighbors_count)(int *, int, int, int, int);

    if (!isatty(STDINF))
        KILL("%s", "A entrada nao eh um terminal");
    if (!isatty(STDOUTF))
        KILL("%s", "A saida nao eh um terminal");

    setSigIntHandler(exit_celular_automata);
    setRawTerminal();
    disableCursor();
    getTerminalSize(&term_width, &term_height);
    setMouseEvents(MOUSE_BUTTON);

    //grid_height = HEIGHT;
    //grid_width = WIDTH;
    grid_height = term_height;
    grid_width = term_width;
    grid_height = MIN(grid_height, term_height - 1);

    int STATE_TABLE[][9][9] = {
        [GOL]= {
            [ALIVE]={DEAD, DEAD, ALIVE, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD},
            [DYING]={DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
            [DEAD]={DEAD, DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD}
        },
        [SEEDS] = {
            [ALIVE]={DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
            [DYING]={DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
            [DEAD]={DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD}
        },
        [BBRAIN] = {
            [ALIVE]={DYING, DYING, DYING, DYING, DYING, DYING, DYING, DYING, DYING},
            [DYING]={DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
            [DEAD]={DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD}
        },
        [DAN] = {
            [ALIVE]={DEAD, DEAD, DEAD, ALIVE, ALIVE, DEAD, ALIVE, ALIVE, ALIVE},
            [DYING]={DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
            [DEAD]={DEAD, DEAD, DEAD, ALIVE, DEAD, DEAD, ALIVE, ALIVE, ALIVE}
        },
        [MAZE] = {
            [ALIVE]={DEAD, ALIVE, ALIVE, ALIVE, ALIVE, ALIVE, DEAD, DEAD, DEAD},
            [DYING]={DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
            [DEAD]={DEAD, DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD}
        },
    };
    
    curr_automaton = MAZE;
    grid = init_grid(grid_width, grid_height);
    curr_neighbors_count = neighbors_count;
    //make_glider(grid, grid_width, grid_height, 10, 10);
    //make_oscillator(grid, grid_width, grid_height, 10, 10);
    //make_oscillator2(grid, grid_width, grid_height, 10, 10);
    //make_rocket(grid, grid_width, grid_height, 10, 10);

    last_gen_time = last_render = timestamp();

    while (!quit){
        c = getEvent(&e);
        switch(c){
            case 'q':
                quit = 1;
                break;
            case 'n':
                if (paused)
                    step = 1;
                break;
            case ' ':
                paused = !paused;
                break;
            case ARROW_RIGHT:
                curr_automaton = MOD_INC(curr_automaton, 5);
                break;
            case MOUSE:
                if (paused && e.button == B1 && e.action == B_PRESSED){
                    set_grid_cell(grid, grid_width, grid_height,
                            e.x-(term_width - grid_width)/2,
                            e.y-(term_height - grid_height)/2, ALIVE);
                }
                break;
        }
        time = timestamp();
        if ((time - last_gen_time >= 1.0/GENPS && !paused) || step){
            next_gen(grid, grid_width, grid_height, STATE_TABLE[curr_automaton], curr_neighbors_count);
            last_gen_time = timestamp();
            step = 0;
        }

        if (time - last_render >= 1.0/FPS){
            clearScreen(FULL);
            char msg[100];
            sprintf(msg, "AUTOMATO EM EXECUÇÂO: %s%s", AUTOMATON_NAME[curr_automaton],
                    paused ? " (pausado - ESPAÇO para continuar)":"");
            size_t msg_len = strlen(msg);
            moveCursor(term_width/2 - msg_len/2, 0);
            printf("%s", msg);
            fflush(stdout);

            render(grid, grid_width, grid_height);
            last_render = timestamp();
        }
    }

    free(grid);
    exit_celular_automata(0);

    return 0;
}
