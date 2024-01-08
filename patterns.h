#ifndef PATTERNS_H_
#define PATTERNS_H_
#define DEAD 0
#define ALIVE 1
#define DYING 2

// retorna se conseguiu colocar o glider [GOL]
int make_glider(int *grid, int w, int h, int x1, int y1){
    int x2 = x1+2;
    int y2 = y1+2;
    if (!((0 <= x1 && x2 < w) &&
        (0 <= y1 && y2 < h)))
        return 0;

    grid[(y1+0)*w+x2] = ALIVE;
    grid[(y1+1)*w+(x1+0)] = ALIVE;
    grid[(y1+1)*w+x2] = ALIVE;
    grid[y2*w+(x1+1)] = ALIVE;
    grid[y2*w+x2] = ALIVE;

    return 1;
}
// retorna se conseguiu colocar o oscillator [SEEDS]
int make_oscillator(int *grid, int w, int h, int x1, int y1){
    int x2 = x1+2;
    int y2 = y1+2;
    if (!((0 <= x1 && x2 < w) &&
        (0 <= y1 && y2 < h)))
        return 0;

    grid[(y1+0)*w+x1] = ALIVE;
    grid[(y1+1)*w+x2] = ALIVE;
    grid[y2*w+(x1+1)] = ALIVE;

    return 1;
}

// retorna se conseguiu colocar o oscillator [BBRAIN]
int make_oscillator2(int *grid, int w, int h, int x1, int y1){
    int x2 = x1+3;
    int y2 = y1+3;
    if (!((0 <= x1 && x2 < w) &&
        (0 <= y1 && y2 < h)))
        return 0;

    grid[(y1+0)*w+(x1+1)] = DYING;
    grid[(y1+1)*w+(x1+1)] = ALIVE;
    grid[(y1+1)*w+(x1+2)] = ALIVE;
    grid[(y1+1)*w+x2] = DYING;
    grid[(y1+2)*w+(x1+0)] = DYING;
    grid[(y1+2)*w+(x1+1)] = ALIVE;
    grid[(y1+2)*w+(x1+2)] = ALIVE;
    grid[y2*w+(x1+2)] = DYING;

    return 1;
}

// retorna se conseguiu colocar o rocket [DAN]
int make_rocket(int *grid, int w, int h, int x1, int y1){
    int x2 = x1+7;
    int y2 = y1+4;
    if (!((0 <= x1 && x2 < w) &&
        (0 <= y1 && y2 < h)))
        return 0;

    grid[(y1+0)*w+(x1+1)] = ALIVE;
    grid[(y1+0)*w+(x1+2)] = ALIVE;

    grid[(y1+1)*w+(x1+0)] = ALIVE;
    grid[(y1+1)*w+(x1+2)] = ALIVE;
    grid[(y1+1)*w+(x1+3)] = ALIVE;

    grid[(y1+2)*w+(x1+0)] = ALIVE;
    grid[(y1+2)*w+(x1+1)] = ALIVE;
    grid[(y1+2)*w+(x1+2)] = ALIVE;
    grid[(y1+2)*w+(x1+3)] = ALIVE;
    grid[(y1+2)*w+(x1+4)] = ALIVE;
    grid[(y1+2)*w+(x1+5)] = ALIVE;
    grid[(y1+2)*w+(x1+6)] = ALIVE;
    grid[(y1+2)*w+(x1+7)] = ALIVE;

    grid[(y1+3)*w+(x1+0)] = ALIVE;
    grid[(y1+3)*w+(x1+2)] = ALIVE;
    grid[(y1+3)*w+(x1+3)] = ALIVE;

    grid[(y1+4)*w+(x1+1)] = ALIVE;
    grid[(y1+4)*w+(x1+2)] = ALIVE;

    return 1;
}
#endif
