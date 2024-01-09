#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "./termal.h"
#include "./patterns.h"
#include "./terminal_automata.h"

struct Cell_t {
    char val[5];
    int bg;
    int fg;
};

struct Window_t {
    int x;
    int y;
    int width;
    int height;
    int size;
    struct Cell_t *cells;
};

int term_width, term_height;

char *REP[] = {
    [ALIVE]="█",
    //[ALIVE]="#",
    [DYING]="&",
    [DEAD]="."
};


int REP_COLOR[][9] = {
    [GOL] = {
        [ALIVE]=WHITE,
        [DYING]=DEFAULT,
        [DEAD]=BLACK
    },
    [SEEDS] = {
        [ALIVE]=WHITE,
        [DYING]=DEFAULT,
        [DEAD]=BLACK
    },
    [BBRAIN] = {
        [ALIVE]=YELLOW,
        [DYING]=RED,
        [DEAD]=CYAN
    },
    [DAN] = {
        [ALIVE]=WHITE,
        [DYING]=DEFAULT,
        [DEAD]=BLACK
    },
    [MAZE] = {
        [ALIVE]=BLUE,
        [DYING]=DEFAULT,
        [DEAD]=BLACK
    }
};

char *AUTOMATON_NAME[] = {
    [GOL] = "Game Of Life",
    [SEEDS] = "SEEDS",
    [BBRAIN] = "Brian's BRAIN",
    [DAN] = "Day And Night",
    [MAZE] = "MAZE"
};

int *create_grid(int w, int h){
    return(int *)calloc(w*h, sizeof(int));
}

struct Window_t *create_window(int w, int h, int x, int y){
    struct Window_t *win = (struct Window_t *)calloc(1, sizeof(struct Window_t));
    if (win == NULL)
        return win;
    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;
    win->size = w*h;
    win->cells = (struct Cell_t *)calloc(win->size, sizeof(struct Cell_t));
    for (int i = 0; i < w*h; i++){
        win->cells[i].val[0] = ' ';
        win->cells[i].fg = WHITE;
    }

    if (win->cells == NULL){
        free(win);
        win = NULL;
    }

    return win;
}

struct Window_t *destroy_window(struct Window_t *win){
    if (win != NULL){
        if (win->cells != NULL)
            free(win->cells);
        free(win);
    }
    win = NULL;
    return win;
}

int put_char(struct Window_t *win, char *c, int fg, int bg, int x, int y){
    int i, n = 0;
    if (win == NULL)
        return 0;

    i = y*win->width + x;
    if (!(0 <= i && i < win->size))
        return 0;
    while((win->cells[i].val[n++] = *(c++)) != '\0' && n < 5);
    win->cells[i].fg = fg;
    win->cells[i].bg = bg+10;

    return 1;
}

int put_str(struct Window_t *win, char *s, int fg, int bg, int x, int y){
    int i;
    if (win == NULL)
        return 0;

    i = y*win->width + x;
    if (!(0 <= i && i < win->size))
        return 0;
    while(*s != '\0' && i < win->size){
        win->cells[i].fg = fg;
        win->cells[i].bg = bg+10;
        win->cells[i++].val[0] = *(s++);
    }

    return 1;
}

// TODO: Fazer para renderizar com cores
// TODO: Fazer para renderizar com payloads
// TODO: Fazer para renderizar com diff entre iteraões
void render_window(struct Window_t *win){
    pushCursor();
    for (int i = 0; i < win->height; i++){
        moveCursor(win->x, win->y+i);
        for (int j = 0; j < win->width; j++)
            printf(win->cells[i*win->width + j].val);
    }
    fflush(stdout);
    popCursor();
}

void init_grid(int *grid, int w, int h){
    memset(grid, DEAD, sizeof(int)*w*h);
    for (int i = 0; i < w*h; i++){
        if (!(rand() % 3))
            //grid[i] = ALIVE;
            grid[i] = DEAD;
    }
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

double timestamp(){
  struct timespec tp;
  clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
  return ( (double)tp.tv_sec + (double)tp.tv_nsec*1.0e-9 );
}

void exit_celular_automata(int s){
    (void)s;
    char msg[] = "TERMINAL AUTOMATA - Obrigdo por usar!";
    size_t msg_sz = sizeof(msg);
    struct Window_t *win = create_window(msg_sz, 1, term_width / 2 - msg_sz / 2, term_height / 2);
    clearScreen(FULL);
    put_str(win, msg, DEFAULT, DEFAULT, 0, 0);
    render_window(win);
    destroy_window(win);
    moveCursor(1, term_height - 1);
    EXIT;
}

void render(int *grid, int w, int h, int curr_automaton){
    (void)curr_automaton;
    for (int i = 0; i < h; i++){
        moveCursor((term_width - w)/2, 1+(term_height - h)/2 + i);
        for (int j = 0; j < w; j++){
            printf(REP[grid[i*w+j]]);
        }
    }
    //printf(ESC"[m");
    fflush(stdout);
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
    double time, last_gen_time;
    int quit = 0;
    struct Event e;
    int c, grid_width, grid_height;
    int *grid;
    int paused = 1, step = 0, curr_automaton, render_sig;
    struct Window_t *draw_win;
    int (*curr_neighbors_count)(int *, int, int, int, int);

    if (!isatty(STDINF))
        KILL("%s", "A entrada nao eh um terminal");
    if (!isatty(STDOUTF))
        KILL("%s", "A saida nao eh um terminal");

    getTerminalSize(&term_width, &term_height);

    grid_height = HEIGHT;
    grid_width = WIDTH;
    //grid_height = term_height;
    //grid_width = term_width;
    grid_height = MIN(grid_height, term_height - 1);

    draw_win = create_window(grid_width, grid_height, (term_width - grid_width)/2, 1+(term_height - grid_height)/2);

    setSigIntHandler(exit_celular_automata);
    setRawTerminal();
    disableCursor();
    setMouseEvents(MOUSE_BUTTON);

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
    
    curr_automaton = GOL;
    grid = create_grid(grid_width, grid_height);
    init_grid(grid, grid_width, grid_height);
    curr_neighbors_count = neighbors_count;
    make_glider(grid, grid_width, grid_height, 10, 10);
    //make_oscillator(grid, grid_width, grid_height, 10, 10);
    //make_oscillator2(grid, grid_width, grid_height, 10, 10);
    //make_rocket(grid, grid_width, grid_height, 10, 10);

    last_gen_time = timestamp();
    render_sig = 1;

    while (!quit){
        time = timestamp();
        if ((time - last_gen_time >= 1.0/GENPS && !paused) || step){
            next_gen(grid, grid_width, grid_height, STATE_TABLE[curr_automaton], curr_neighbors_count);
            last_gen_time = timestamp();
            step = 0;
            render_sig = 1;
        }

        if (render_sig){
            clearScreen(FULL);
            char msg[100];
            sprintf(msg, "AUTOMATO EM EXECUÇÂO: %s%s", AUTOMATON_NAME[curr_automaton],
                    paused ? " (pausado - ESPAÇO para continuar)":"");
            size_t msg_len = strlen(msg);
            moveCursor(term_width/2 - msg_len/2, 0);
            printf("%s", msg);
            fflush(stdout);

            for (int i = 0; i < draw_win->height; i++)
                for (int j = 0; j < draw_win->width; j++)
                    put_char(draw_win, REP[grid[i*draw_win->width+j]], DEFAULT, DEFAULT, j, i);

            //render(grid, grid_width, grid_height, curr_automaton);
            render_window(draw_win);
            render_sig = 0;
        }

        c = getEvent(&e);
        render_sig = 1;
        switch(c){
            case 'q':
                quit = 1;
                break;
            case 'r':
                init_grid(grid, grid_width, grid_height);
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
            case ARROW_LEFT:
                curr_automaton = MOD_DEC(curr_automaton, 5);
                break;
            case MOUSE:
                if (paused && e.button == B1 && e.action == B_PRESSED){
                    set_grid_cell(grid, grid_width, grid_height,
                            e.x-draw_win->x,
                            e.y-draw_win->y, ALIVE);
                }
                break;
            default:
                render_sig = 0;
        }
    }

    free(grid);
    destroy_window(draw_win);
    exit_celular_automata(0);

    return 0;
}
