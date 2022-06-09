#include "libProgetto.h"
#define PASSO 1
#define SU 65
#define GIU 66
#define SPAZIO 32
#define Q 113

#define MORTO 0
#define ROCKET 1
#define HIT 2

#define GIOCATOREX_DEFAULT 8
#define GIOCATOREY_DEFAULT 15 //default 11
#define NUMERO_MISSILI 2
#define MISSILE_UNO 0
#define MISSILE_DUE 1
#define BORDO_NEMICI 4
#define PID_NON_DEFINITO -1

#define NEMICOX_DEFAULT 70 //default 95

#define BORDOSUPERIORE 3

#define LETTURA 0
#define SCRITTURA 1

//punteggi
#define NEMICO_COLPITO 100
#define NEMICO_MORTO 250
#define VITA_PERSA -70

//1 sec = 1.000.000 microsecondi
#define TEMPOMOV_NEMICI 300000 //default 300.000 secondo
#define TEMPOMOV_MISSILI 30000 //default 100.000
#define TEMPOMOV_BOMBE 50000 //default 10.000
#define TEMPOGEN_BOMBE 100000 //default 600.000
#define PASSO 1
#define LIM_GAMEINFO 4
#define PERMESSO 1
#define NEGATO 0
#define DELAY_TIMER 2 //default 2
#define ALIVE 100

//definizioni per l'utilizzo della grafica
#define SPRITE_XY 3 //grandezza righe e colonne sprite (3x3)
#define STAMPA 0
#define CANCELLA 1

//definizione modelli di astronave
char spriteGiocatore[SPRITE_XY][SPRITE_XY];
char spriteNemico1[SPRITE_XY][SPRITE_XY];
char spriteNemico2[SPRITE_XY][SPRITE_XY];
char spriteMissile;
char spriteBomba;

//funzione per stampare gli oggetti

void stampaOggetto(WINDOW *finestra, int dim_sprite, char oggetto[][dim_sprite], int posx, int posy, int mode);

//funzioni del gioco

void giocatore(int pipeOutCoordinate, int pipeOutNavicella);
void missile(pos pos_giocatore, int pipeOutCoordMissile);
void nemico(int pipeOutCoordinate, int pipeInNemici);
void bomba(pos nemico, int pipeOutCoordinate);
void controllo(int pipeInCoordinate, int pipeOutNemici, int pipeOutNavicella, int pipeOutCollisioni);

//funzioni di supporto
void refreshScoreFile(int score);
void gameover();
void gamewon();
void youlose();
void youwin();
_Bool esitoGioco(int vite, int punteggio, int nemiciRimanenti, pos *nemici);
_Bool isInvaso(pos *nemici);
void muoviNemici(pos nemico, int pipeOutCoordinate);
void inizializzaNemici(pos *nemici, int nemicoX);
void inizializzaBombe(pos *bombe);
void inizializzaMissili(pos *missili);
int generaNumero(int min, int max);
void soundsEffectManager(int mode);
