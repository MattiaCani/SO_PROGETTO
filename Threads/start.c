#include "start.h"
int i;

/**
 * Crea il mutex per temporizzare tra loro i thread, crea i 3 thread principali (giocatore, missile, nemico)
 * 
 */
void start(){
    initscr();
    noecho();
    srand(time(NULL));
    curs_set(0);
    start_color();

    //inizializzo il mutex
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

    //creazione del thread controllo, padre di giocatore, nemici
    if(pthread_create(&tControllo, NULL, &controllo, NULL)){
        endwin();
        exit;
    }

    //creazione del thread giocatore
    if(pthread_create(&tGiocatore, NULL, &giocatore, NULL)){
        endwin();
        exit;
    }
    
    int arguments[M];
    for(i=0;i<M;i++){
        arguments[i] = i;
    }

    //creazione di M thread nemici
    for(i=0;i<M;i++){
        if(pthread_create(&tNemici[i], NULL, &nemico, &arguments[i])){
            endwin();
            exit;
        }
    }

    //attendo la terminazione dei M thread nemici
    for(i=0;i<M;i++){
        pthread_join(tNemici[i], NULL);
        pthread_cancel(tNemici[i]);
    }
    
    //attendo la terminazione del thread giocatore
    pthread_join(tGiocatore, NULL);
    pthread_cancel(tGiocatore);

    pthread_join(tControllo, NULL);
    pthread_cancel(tControllo);

    //elimino il mutex
    pthread_mutex_destroy(&mtx);
}