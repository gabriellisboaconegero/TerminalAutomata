#ifndef TERMINAL_AUTOMATA_H_
#define TERMINAL_AUTOMATA_H_
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

enum TermColor {
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    DEFAULT,
};

#endif
