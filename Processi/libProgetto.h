#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#define MAXX 100 //numero colonne schermo
#define MAXY 29 //numero righe schermo
#define M 8 //numero di nemici (ogni colonna ha 4 nemici)

typedef struct{
    char c;
    int x;
    int y;
    int id;
    int livello;
    _Bool attivo;
    _Bool bomba;
    pid_t pid;
    int delay;
} pos;

//anziche' attivo fare vettore di elementi attivi