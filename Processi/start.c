#include "start.h"

int pipeCoordinate[2], pipeNavicella[2], pipeNemici[2], pipeCollisioni[2];
pid_t pid_nemico[M], pid_giocatore;
int i;

/**
 * Crea le pipe per permettere ai processi di comunicare tra loro, crea i 3 processi principali (giocatore, missile, nemico)
 * 
 */
void start(){
    initscr();
    noecho();
    srand(time(NULL));
    curs_set(0);
    start_color();

    //creazione delle pipe per consentire ai processi di comunicare tra loro
    if(pipe(pipeCoordinate)==-1){
        perror("Errore nella creazione della pipe");
        _exit(1);
    }

    if(pipe(pipeNavicella)==-1){
        perror("Errore nella creazione della pipe");
        _exit(1);
    }

    if(pipe(pipeNemici)==-1){
        perror("Errore nella creazione della pipe");
        _exit(1);
    }

    if(pipe(pipeCollisioni)==-1){
        perror("Errore nella creazione della pipe");
        _exit(1);
    }
    
    //creazione di M processi nemici
    for(i=0;i<M;i++){
        switch(pid_nemico[i] = fork()){
        case -1:
            perror("Errore nell'esecuzione della fork");
            exit(1);

        case 0:
            close(pipeCoordinate[LETTURA]);
            close(pipeNemici[SCRITTURA]);
            close(pipeCollisioni[SCRITTURA]);
            nemico(pipeCoordinate[SCRITTURA], pipeNemici[LETTURA]);
        }
    }
    
    //creazione del processo giocatore
    switch(pid_giocatore = fork()){
        case -1:
            perror("Errore nell'esecuzione della fork");
            exit(1);
        case 0:
            close(pipeCoordinate[LETTURA]);
            close(pipeNavicella[SCRITTURA]);
            giocatore(pipeCoordinate[SCRITTURA], pipeNavicella[LETTURA]); 
    }
    
    //creazione del processo controllo, padre di giocatore, nemici e timerBombe
    close(pipeCoordinate[SCRITTURA]);
    close(pipeNemici[LETTURA]);
    close(pipeNavicella[LETTURA]);
    close(pipeCollisioni[LETTURA]);
    controllo(pipeCoordinate[LETTURA], pipeNemici[SCRITTURA], pipeNavicella[SCRITTURA], pipeCollisioni[SCRITTURA]);
            
    //waitpid(pid_missile, NULL, WNOHANG);
    //waitpid(pid_giocatore, NULL, WNOHANG);
    //waitpid(pid_nemico, NULL, WNOHANG);

    for(i=0;i<M;i++){
        kill(pid_nemico[i], SIGUSR1);
    }
    
    kill(pid_giocatore, SIGUSR1);
}