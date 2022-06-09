#include "start.h"

#define SU 65
#define GIU 66

#define UNO '1' //49
#define DUE '2' //50
#define TRE '3' //51
#define QUATTRO '4' //52
#define CINQUE '5' //53

#define SPEED_UFO_INIZIO 100000 //centomila
#define SPEED_UFO_FINE 50000 //cinquantamila

void *disegnaStelle(WINDOW *finestra);
void generaStelle(WINDOW *finestra);
void stampaMenu();
void crediti();
void comandi();

void story();
void combatti();
void attenzione();
void opening();
void *welcome();
void *ufo();
void *musica();