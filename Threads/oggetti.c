#include "oggetti.h"

pthread_t tHit, tRocket, tMorto;
int rocket = ROCKET, hit = HIT, morto = MORTO;
pos pGiocatore, pNemici[M], pMissili[NUMERO_MISSILI+1], pBombe[M];

//modelli di astronave
char spriteGiocatore[SPRITE_XY][SPRITE_XY]={
    {"\\.."},
    {"==>"},
    {"/''"}
};

char spriteNemico1[SPRITE_XY][SPRITE_XY]={
    {"///"},
    {"HHH"},
    {"\\\\\\"}
};

char spriteNemico2[SPRITE_XY][SPRITE_XY]={
    {"@ @"},
    {"   "},
    {"@ @"}
};

char spriteMissile = '*';
char spriteBomba = 'O';

/**
 * Procedura che si occupa di produrre le coordinate del giocatore dato un input 
 * delle freccie su o giu, oppure lo spazio che manda un input nella pipe che viene
 * letto dalla procedura missile.
 * 
 * @param pipeOutCoordinate pipe di output delle coordinate
 * @param pipeOutNavicella  pipe di output delle coordinate
 */
void *giocatore(){
    int i;

    //inizializzo la variabile del giocatore
    pGiocatore.c='>';
    pGiocatore.id = 0;
    pGiocatore.x = GIOCATOREX_DEFAULT;
    pGiocatore.y = GIOCATOREY_DEFAULT;
    pGiocatore.tid = tGiocatore;

    //ciclo che legge continuamente l'input e modifica la posizione del giocatore di conseguenza
    while(true){
        char c;
        switch(c=getch()){
            case SU:
                if(pGiocatore.y > BORDOSUPERIORE) pGiocatore.y--;
                break;
            case GIU:
                if(pGiocatore.y< MAXY-2) pGiocatore.y++;
                break;
            case SPAZIO:
                missile();
                break;
        }
    }

    //pthread_cancel qua
    //devo aspettare con la join i thread dei missili
    pthread_cancel(tMissili[MISSILE_UNO]);
    pthread_cancel(tMissili[MISSILE_DUE]);
}

/**
 * Procedura che una volta letto l'input spazio dalla pipe si occupa 
 * di produrre le coordinate dei missili, sino a quando arrivano al 
 * bordo schermo o toccano un nemico, causandone la distruzione o 
 * l'evoluzione di livello.
 * 
 * @param pipeInCoordGiocatore pipe di lettura coordinate del giocatore
 * @param pipeOutCoordMissile pipe di scrittura delle coordinate dei missili
 */
void missile(){
    int i;

    //inizializzo i due missili
    for(i=0;i<NUMERO_MISSILI;i++){
        pMissili[i].c = '*';
        pMissili[i].id = i+1;
        pMissili[i].livello = 1;
        pMissili[i].attivo = true;
    }

    int id1 = MISSILE_UNO, id2 = MISSILE_DUE;
    //creo i due thread per i missili
    if(pthread_create(&tMissili[id1], NULL, &muoviMissile, &id1)){
        endwin();
        exit;
    } 

    if(pthread_create(&tMissili[id2], NULL, &muoviMissile, &id2)){
        endwin();
        exit;
    } 
}

void *muoviMissile(void *arg){
    int missileID = *(int*)arg;

    if(pthread_create(&tHit, NULL, soundsEffectManager, &rocket)){
        exit;
    }

    if(missileID == MISSILE_UNO){
        tMissili[MISSILE_UNO] = pthread_self();
        pMissili[MISSILE_UNO].x = pGiocatore.x+3; //alto a destra
        pMissili[MISSILE_UNO].y = pGiocatore.y-1;

        do{
            pMissili[MISSILE_UNO].x+=3;
            if(pMissili[MISSILE_UNO].x%4){
                pMissili[MISSILE_UNO].y-=1;
            }
            
            usleep(TEMPOMOV_MISSILI);
        } while((pMissili[MISSILE_UNO].y > LIM_GAMEINFO || pMissili[MISSILE_UNO].x < MAXX));

        pthread_exit(NULL);

    } else if(missileID == MISSILE_DUE){
        tMissili[MISSILE_DUE] = pthread_self();
        pMissili[MISSILE_DUE].x = pGiocatore.x+3; //basso a destra
        pMissili[MISSILE_DUE].y = pGiocatore.y+1;

        do{
            pMissili[MISSILE_DUE].x+=3;
            if(pMissili[MISSILE_DUE].x%4){
                pMissili[MISSILE_DUE].y+=1;
            }

            usleep(TEMPOMOV_MISSILI);

        } while((pMissili[MISSILE_DUE].y < MAXY || pMissili[MISSILE_DUE].x < MAXX));

        pthread_exit(NULL);
    }
}

/**
 * Procedura produttore che crea le coordinate dei nemici, le manda nella pipe e 
 * vengono lette e consumate dal processo controllo che le stampa e pulisce 
 * lo schermo
 * 
 * @param pipeOutCoordinate pipe in scrittura delle coordinate del nemico che si muove
 * @param pipeInNemici
 */
void *nemico(void *arg){
    int stato, i = *(int*)arg, indice = i;
    _Bool generaBomba = true;
    tNemici[indice] = pthread_self();

    while(true){
        //animo i nemici
        usleep(TEMPOMOV_NEMICI);

        //movimento 1
        pthread_mutex_lock(&mtx);
        pNemici[i].y -= PASSO; //va su'
        pthread_mutex_unlock(&mtx);

        usleep(TEMPOMOV_NEMICI);

        //movimento 2
        pthread_mutex_lock(&mtx);
        pNemici[i].y += PASSO; //va giu'
        pthread_mutex_unlock(&mtx);

        usleep(TEMPOMOV_NEMICI);
        
        //movimento 3
        pthread_mutex_lock(&mtx);
        pNemici[i].y += PASSO; //va giu'
        pthread_mutex_unlock(&mtx);

        usleep(TEMPOMOV_NEMICI); 
        
        //movimento 4
        pthread_mutex_lock(&mtx);
        pNemici[i].y -= PASSO; //va su
        pthread_mutex_unlock(&mtx);

        usleep(TEMPOMOV_NEMICI);

        //movimento 5
        pthread_mutex_lock(&mtx);
        pNemici[i].x -= PASSO; //va avanti
        pthread_mutex_unlock(&mtx);

        if(generaBomba && rand()%5==0){
            if(pthread_create(&tBombe[i], NULL, bomba, &i)){
                endwin();
                exit;
            }
            
            pthread_detach(tBombe[i]);
            generaBomba = false;
        }

        usleep(50000*M);

        if(stato != ALIVE){
            generaBomba = true;
        } 
    } //parentesi while true
    pthread_exit(NULL);
}

/**
 * Procedura che legge un nemico e manda le coordinate della bomba nella pipe principale
 * 
 * @param nemico nemico mandato alla procedura su cui generare la bomba
 * @param pipeOutCoordinate pipe in cui scrivere le coordinate delle bombe prodotte
 */
void *bomba(void *arg){
    int i = *(int*)arg, indice = i;

    tBombe[indice] = pthread_self();
    
    pBombe[i].c = spriteBomba;
    pBombe[i].id = i;
    pBombe[i].livello = 0;

    pBombe[i].x = pNemici[i].x - 3;
    pBombe[i].y = pNemici[i].y + 1;

    do{
        pthread_mutex_lock(&mtx);
        pBombe[i].x--;
        pthread_mutex_unlock(&mtx);
        usleep(TEMPOMOV_BOMBE);
    } while(pBombe[i].x >= -1); 
    
    pthread_exit(NULL);
}

/**
 * Procedura consumatore, che si occupa di leggere dalla pipe globale delle 
 * coordinate di tutti gli oggetti, pulendo lo schermo e aggiornando i caratteri 
 * stampati.
 * 
 * @param pipeInCoordinate pipe di lettura delle coordinate di tutti gli oggetti del gioco
 */
void *controllo(){
    int punteggio = 0, vite = 3, vettViteNemici[M], nemiciRimanenti = M, i, j, k, l, distrutti = 0, nemicoX = NEMICOX_DEFAULT, status1; //valori di default
    pos giocatoreVecchio, missiliVecchi[NUMERO_MISSILI+1], nemiciVecchi[M], bombeVecchie[M];

    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);

    //inizializzo le entita'
    inizializzaNemici(pNemici, nemicoX);
    inizializzaBombe(pBombe);
    inizializzaMissili(pMissili);

    //inizializzo il vettore delle vite dei nemici
    pthread_mutex_lock(&mtx);
    for(i=0;i<M;i++){
        vettViteNemici[i] = 3;
    }
    pthread_mutex_unlock(&mtx);

    WINDOW *gameArea = newwin(30, 100, 0, 0);
    
    //endwin();
    usleep(1000); //delay avvio funzione controllo -> bug healthPoints non stampate risolto?
    do{
        wattron(gameArea, COLOR_PAIR(1)); //color giallo
        mvwprintw(gameArea, 1, 18, "VITE: %d", vite);
        mvwprintw(gameArea, 1, 43, "PUNTEGGIO: %d", punteggio);
        mvwprintw(gameArea, 1, 75, "M x %d", nemiciRimanenti);

        wattron(gameArea, COLOR_PAIR(2)); //colori di default
        
        if(pGiocatore.x >= 0){
            stampaOggetto(gameArea, SPRITE_XY, spriteGiocatore, giocatoreVecchio.x, giocatoreVecchio.y, CANCELLA);
            giocatoreVecchio = pGiocatore;
            stampaOggetto(gameArea, SPRITE_XY, spriteGiocatore, pGiocatore.x, pGiocatore.y, STAMPA);
        }
                
        //controlla se e' missile 1 oppure 2 dal suo id
        wattron(gameArea, COLOR_PAIR(2));
        for(i=0;i<NUMERO_MISSILI;i++){
            if((pMissili[i].x >= 0) && (pMissili[i].y <= MAXY)){ 
                pthread_mutex_lock(&mtx);
                mvwaddch(gameArea, missiliVecchi[i].y, missiliVecchi[i].x, ' ');
                missiliVecchi[i] = pMissili[i];
                mvwaddch(gameArea, pMissili[i].y, pMissili[i].x, spriteMissile);
                pthread_mutex_unlock(&mtx);
            }
        }
            
        //non stampa se legge il valore inizializzato di default del vettore di bombe
        for(i=0;i<M;i++){
            if(pBombe[i].livello >= 0){
                wattron(gameArea, COLOR_PAIR(5));
                if(pBombe[i].x >= 0){ 
                    pthread_mutex_lock(&mtx);
                    mvwaddch(gameArea, bombeVecchie[i].y, bombeVecchie[i].x, ' ');
                    bombeVecchie[i] = pBombe[i];
                    mvwaddch(gameArea, pBombe[i].y, pBombe[i].x, spriteBomba);
                    pthread_mutex_unlock(&mtx);
                }
            }
        }

        //stampo i nemici
        for(i=0;i<M;i++){
            if(vettViteNemici[i] == 2){
                wattron(gameArea, COLOR_PAIR(1));
                stampaOggetto(gameArea, SPRITE_XY, spriteNemico2, nemiciVecchi[i].x, nemiciVecchi[i].y, CANCELLA);

                pthread_mutex_lock(&mtx);
                nemiciVecchi[i] = pNemici[i];
                pthread_mutex_unlock(&mtx);

                stampaOggetto(gameArea, SPRITE_XY, spriteNemico2, pNemici[i].x, pNemici[i].y, STAMPA);
            } else if(vettViteNemici[i] == 3){
                wattron(gameArea, COLOR_PAIR(3));
                stampaOggetto(gameArea, SPRITE_XY, spriteNemico1, nemiciVecchi[i].x, nemiciVecchi[i].y, CANCELLA);

                pthread_mutex_lock(&mtx);
                nemiciVecchi[i] = pNemici[i];
                pthread_mutex_unlock(&mtx);

                stampaOggetto(gameArea, SPRITE_XY, spriteNemico1, pNemici[i].x, pNemici[i].y, STAMPA);
            } else if(vettViteNemici[i] == 1){
                wattron(gameArea, COLOR_PAIR(4));
                stampaOggetto(gameArea, SPRITE_XY, spriteNemico2, nemiciVecchi[i].x, nemiciVecchi[i].y, CANCELLA);

                pthread_mutex_lock(&mtx);
                nemiciVecchi[i] = pNemici[i];
                pthread_mutex_unlock(&mtx);

                stampaOggetto(gameArea, SPRITE_XY, spriteNemico2, pNemici[i].x, pNemici[i].y, STAMPA);
            }
        }
        
        curs_set(0);
        wrefresh(gameArea);

        //controllo collisione giocatore-bombe
        for(i=0;i<M;i++){
            if(pBombe[i].id >= 0){
                if((pBombe[i].x >= pGiocatore.x && pBombe[i].x <= pGiocatore.x + 2) && (pBombe[i].y >= pGiocatore.y && pBombe[i].y <= pGiocatore.y + 2)){
                    vite-=1;

                    if(pthread_create(&tHit, NULL, soundsEffectManager, &hit)){
                        exit;
                    }

                    pBombe[i].id = -1;
                    pBombe[i].x = -1;
                    pBombe[i].y = -1;

                    pthread_cancel(tBombe[i]); //devo killare il thread della bomba
                    punteggio+=VITA_PERSA;
                    
                    pthread_mutex_lock(&mtx);
                    mvwprintw(gameArea, 1, 43, "                   ");
                    mvwaddch(gameArea, pBombe[i].y, pBombe[i].x, ' ');
                    wclear(gameArea);
                    pthread_mutex_unlock(&mtx);
                    
                    wattron(gameArea, COLOR_PAIR(2));
                    stampaOggetto(gameArea, SPRITE_XY, spriteGiocatore, pGiocatore.x, pGiocatore.y, STAMPA);
                }
            }
        }

        //controllo collisione missile-nemico
        for(j=0;j<NUMERO_MISSILI+1;j++){ //scorro l'array dei missili
            if(pMissili[j].attivo == true){ //controllo se il missile e' vivo
                for(i=0;i<M;i++){ //scorro l'array dei nemici ed effettuo i controlli delle collisioni, killando il missile se collide
                    if(vettViteNemici[i] > 0){ //il nemico deve essere vivo
                        if(pMissili[j].id != -1){ //il missile non deve essere quello inizializzato, ma deve avere dei valori suoi
                            if((pMissili[j].x >= pNemici[i].x && pMissili[j].x <= pNemici[i].x + 2) && (pMissili[j].y >= pNemici[i].y && pMissili[j].y <= pNemici[i].y + 2)){
                                vettViteNemici[i]-=1;
                                //system("aplay -q sounds/hit.wav");
                                if(pthread_create(&tHit, NULL, soundsEffectManager, &hit)){
                                    exit;
                                }

                                pthread_cancel(tMissili[j]); //devo killare il thread dei processi

                                pthread_mutex_lock(&mtx);
                                mvwprintw(gameArea, missiliVecchi[i].y, missiliVecchi[i].x, " ");
                                mvwprintw(gameArea, 1, 43, "                   ");
                                pthread_mutex_unlock(&mtx);

                                pMissili[j].x = -1;
                                pMissili[j].y = -1;
                                
                                punteggio+=NEMICO_COLPITO;
                                
                            }
                        }
                    } else if(vettViteNemici[i] == 0 && pNemici[i].attivo == true){
                        //system("aplay -q sounds/morto.wav");
                        if(pthread_create(&tHit, NULL, soundsEffectManager, &morto)){
                            exit;
                        }
                        pthread_cancel(tNemici[i]); //killo il nemico morto
                        pthread_cancel(tMissili[j]); //killo il proiettile che ha sbattuto
                        
                        //pthread_mutex_lock(&mtx);
                        //inizializzaMissili(pMissili);
                        nemiciRimanenti--;
                        punteggio+=NEMICO_MORTO;

                        

                        pthread_mutex_lock(&mtx);
                        mvwprintw(gameArea, 1, 43, "                   ");
                        mvwprintw(gameArea, missiliVecchi[i].y, missiliVecchi[i].x, " ");
                        pthread_mutex_unlock(&mtx);
                        stampaOggetto(gameArea, SPRITE_XY, spriteNemico1, pNemici[i].x, pNemici[i].y, CANCELLA); //forse si bugga
                       
                        pNemici[i].attivo = false;
                        //pthread_mutex_unlock(&mtx);
                    }
                }
            }
        }

        //pulisco le bombe morte
        for(i=0;i<MAXY+1;i++){
            mvwaddch(gameArea, i, 0, ' ');
            mvwaddch(gameArea, i, 10, ' '); 
        }
        
        //controllo collisione giocatore-nemico -> in esitoGioco con la funzione isInvaso();
    } while(esitoGioco(vite, punteggio, nemiciRimanenti, pNemici)); //collisione == true || healthPoints == 0;

    pthread_exit(NULL);
}

/**
 * Prende in input lo score e decide se creare il file score.dat o leggerne uno esistente per poi sovrascriverlo
 * 
 * 
 * @param score punteggio
 */
void refreshScoreFile(int score){
    //apro il file e leggo 1024 bytes salvandoli in una stringa buff
    int datoLetto, BUFFER = 1024;
    FILE *dat;
    char buff[BUFFER];

    dat = fopen("score.dat", "rb"); //apro il file
    
    //controllo se il file esiste
    if(dat != NULL){
        //se il file esiste leggo il dato
        fread(&datoLetto, sizeof(int), 1, dat); //leggo il dato
        fclose(dat);

        ////printf("Il precedente highscore e': %d\n", datoLetto);

        //controllo il punteggio ottenuto con quello gia' nel file se presente. Se lo supero lo sovrascrivo
        if(score > datoLetto){
            dat = fopen("score.dat", "wb");
            fwrite(&score, sizeof(int), 1, dat);
            fclose(dat);
            ////printf("Nuovo highscore: %d\n", score);
        } else {
            ////printf("Nessun nuovo record...\n");
        }
    } else if(dat == NULL){
        //printf("Non esiste nessun file contenente il punteggio!\n");
        //il file non esiste e devo crearlo
        ////printf("Procedo con la sua creazione e col salvataggio di %d punti\n", score);
        dat = fopen("score.dat", "wb"); //riapri il file in scrittura per crearlo se non esiste
        fwrite(&score, sizeof(int), 1, dat); // scrivi il punteggio sul file
        fclose(dat); //chiudi il file aperto
    }
}

/**
 * Procedura che si occupa di stampare la schermata di game over
 */
void gameover(){
    char c;
    initscr();
    curs_set(0);
    noecho();
    //clear();

    pthread_mutex_lock(&mtx);
    box(stdscr, 0, 0);

    char *gameOver1 = " _____   ___  ___  ___ _____   _____  _   _ ___________ ";
    char *gameOver2 = "|  __ \\ / _ \\ |  \\/  ||  ___| |  _  || | | |  ___| ___ \\";
    char *gameOver3 = "| |  \\// /_\\ \\| .  . || |__   | | | || | | | |__ | |_/ / ";
    char *gameOver4 = "| | __ |  _  || |\\/| ||  __|  | | | || | | |  __||    / ";
    char *gameOver5 = "| |_\\ \\| | | || |  | || |___  \\ \\_/ /\\ \\_/ / |___| |\\ \\";
    char *gameOver6 = " \\____/\\_| |_/\\_|  |_/\\____/   \\___/  \\___/\\____/\\_| \\_|";

    mvprintw(2, 21, gameOver1);
    mvprintw(3, 21, gameOver2);
    mvprintw(4, 21, gameOver3);
    mvprintw(5, 21, gameOver4);
    mvprintw(6, 21, gameOver5);
    mvprintw(7, 21, gameOver6);

    char *lapide2 =  "               .";
    char *lapide3 =  "              -|-";
    char *lapide4 =  "               |";
    char *lapide5 =  "           .-'~~~`-.";
    char *lapide6 =  "         .'         `.";
    char *lapide7 =  "         |  R  I  P  |";
    char *lapide8 =  "         |  soldato  |";
    char *lapide9 =  "         |           |";
    char *lapide10 = "       \\\\|           |//";
    char *lapide11 = "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";

    mvprintw(11, 13, lapide2);
    mvprintw(12, 13, lapide3);
    mvprintw(13, 13, lapide4);
    mvprintw(14, 13, lapide5);
    mvprintw(15, 13, lapide6);
    mvprintw(16, 13, lapide7);
    mvprintw(17, 13, lapide8);
    mvprintw(18, 13, lapide9);
    mvprintw(19, 13, lapide10);
    mvprintw(20, 13, lapide11);

    //box(dialogBox, 0, 0);
    mvprintw(16, 60, "              Sei morto!");
    mvprintw(25, 63, "Tra 5 secondi tornerai al menu . . .");
    refresh();
    pthread_mutex_unlock(&mtx);
    youlose();
    sleep(5);
}

/**
 * Procedura che stampa il messaggio di gamewon all'utente
 */
void gamewon(){
    char c;
    initscr();
    noecho();
    curs_set(0);
    
    pthread_mutex_lock(&mtx);
    box(stdscr, 0, 0);

    char *message = "Complimenti, hai vinto! Questa medaglia e' per te, accettala!";

    mvprintw(6, 21, message);

    char *medaglia1 =  " _______________";
    char *medaglia2 =  "|@@@@|     |####|";
    char *medaglia3 =  "|@@@@|     |####|";
    char *medaglia4 =  "|@@@@|     |####|";
    char *medaglia5 =  "\\@@@@|     |####/";
    char *medaglia6 =  " \\@@@|     |###/";
    char *medaglia7 =  "  `@@|_____|##'";
    char *medaglia8 =  "       (O)";
    char *medaglia9 =  "    .-''''''-.";
    char *medaglia10 = "  .'  * ** *  `.";
    char *medaglia11 = " :  *        *  :";
    char *medaglia12 = ": ~  MEDAGLIA  ~ :";
    char *medaglia13 = ": ~  D' ONORE  ~ :";
    char *medaglia14 = " :  *        *  :";
    char *medaglia15 = "  `.  * ** *  .'";
    char *medaglia16 = "    `-......-'";

    mvprintw(10, 13, medaglia1);
    mvprintw(11, 13, medaglia2);
    mvprintw(12, 13, medaglia3);
    mvprintw(13, 13, medaglia4);
    mvprintw(14, 13, medaglia5);
    mvprintw(15, 13, medaglia6);
    mvprintw(16, 13, medaglia7);
    mvprintw(17, 13, medaglia8);
    mvprintw(18, 13, medaglia9);
    mvprintw(19, 13, medaglia10);
    mvprintw(20, 13, medaglia11);
    mvprintw(21, 13, medaglia12);
    mvprintw(22, 13, medaglia13);
    mvprintw(23, 13, medaglia14);
    mvprintw(24, 13, medaglia15);
    mvprintw(25, 13, medaglia16);
    
    mvprintw(16, 45, "Grazie per aver giocato a Space Defender!");
    mvprintw(25, 63, "Tra 5 secondi tornerai al menu . . .");
    refresh();
    pthread_mutex_unlock(&mtx);
    youwin();
    sleep(5);
}

/**
 * @brief riproduce il feedback sonoro della vincita
 * 
 */
void youlose(){
    system("aplay -q sounds/youlose.wav");
}

/**
 * @brief riproduce il feedback sonoro della vincita
 * 
 */
void youwin(){
    system("aplay -q sounds/youwin.wav");
}

/**
 * Procedura che controlla se si ha vinto o si ha perso, stampando a schermo il relativo messaggio
 * 
 */
_Bool esitoGioco(int vite, int punteggio, int nemiciRimanenti, pos *nemici){
    if(nemiciRimanenti == 0){
        gamewon();
        refreshScoreFile(punteggio);
        killAllThreads(GAMEWON);
        return false;
    } else if(vite == 0 || isInvaso(nemici)){
        gameover();
        refreshScoreFile(punteggio);
        killAllThreads(GAMEOVER);
        return false;
    } else {
        return true;
    }
}

/**
 * Funzione che controlla se il giocatore e' stato invaso dai nemici attivi
 * 
 * @param nemici array dei nemici
 * @return _Bool true se il giocatore e' stato invaso
 */
_Bool isInvaso(pos *nemici){
    int i;
    for(i=0;i<M;i++){
        if(nemici[i].attivo == true){
            if(nemici[i].x == GIOCATOREX_DEFAULT+2){
                return true;
            } 
        }
    }
    return false;
}

/**
 * Procedura che inizializza i nemici ai valori di default (inizio partita)
 * 
 * @param nemici vettore dei nemici
 * @param nemicoX posizione X da dove iniziare a generare le coordinate delle file di nemici
 */
void inizializzaNemici(pos *nemici, int nemicoX){
    int i;
    //inizializzo i nemici ai valori di default e li mando nella pipe per essere stampati
    pthread_mutex_lock(&mtx);
    for(i=0;i<M;i++){
        nemici[i].c = 'M';
        nemici[i].x = nemicoX; //dove viene generato il primo nemico

        if(i == 0){
            nemici[i].y = 6; //altezza del primo nemico
        } else{
            nemici[i].y = nemici[i-1].y + 6; //ogni nemico successivo ha altezza 3 più in basso del precedente (default 3)
        }
        
        nemici[i].id = i;
        nemici[i].attivo = true; //l'entita' e' attiva
        nemici[i].livello = 2;
        nemici[i].bomba = false;

        //se oltrepassa il bordo di giu', torna ad altezza 1 e a sinistra di 2 per stampare i nemici ordinati
        if(nemici[i].y >= 27){
            nemici[i].y = 6;
            nemici[i].x = nemici[i-1].x - 7; //nuova colonna a sinistra (default 3)
            nemicoX -= 7;
        }
    }
    pthread_mutex_unlock(&mtx);
}

/**
 * @brief Questa procedura inizializza il vettore delle bombe a dei valori di default.
 * 
 * @param bombe vettore delle bombe
 */
void inizializzaBombe(pos *bombe){
    int i;
    pthread_mutex_lock(&mtx);
    for(i=0;i<M;i++){
        bombe[i].c = 'O';
        bombe[i].attivo = true;
        bombe[i].livello = -1;
        bombe[i].x = -1;
        bombe[i].y = -1;
        bombe[i].id = -1;
        bombe[i].tid = -1;
        bombe[i].delay = -1;
    }
    pthread_mutex_unlock(&mtx);
}

/**
 * @brief Procedura che inizializza il vettore dei missili
 * 
 * @param missili vettore dei missili
 */
void inizializzaMissili(pos *missili){
    int i;
    pthread_mutex_lock(&mtx);
    for(i=0;i<NUMERO_MISSILI;i++){
        missili[i].c = '*';
        missili[i].x = -1;
        missili[i].y = -1;
        missili[i].id = i;
        missili[i].attivo = false;
        missili[i].livello = -1;
        missili[i].tid = -1;
    }
    pthread_mutex_unlock(&mtx);
}

/**
 * Procedura che stampa o cancella un oggetto in una finestra, prese le sue coordinate.
 * 
 * @param finestra finestra in cui stampare o cancellare
 * @param dim_sprite dimensione dello sprite (riga e colonne)
 * @param oggetto sprite da stampare (matrice di caratteri)
 * @param posx coord x in cui stampare o cancellare
 * @param posy coord y in cui stampare o cancellare
 * @param mode modalita' STAMPA o CANCELLA
 */
void stampaOggetto(WINDOW *finestra, int dim_sprite, char oggetto[][dim_sprite], int posx, int posy, int mode){
    int x, y, j, k;
    char daStampare;
    int dim_spritex = dim_sprite, dim_spritey = dim_sprite;

    x = posx;
    y = posy;

    if(mode == STAMPA){
        pthread_mutex_lock(&mtx);
        for(j=0;j<dim_spritex;j++){
            for(k=0;k<dim_spritey;k++){
                daStampare = oggetto[j][k];
                mvwaddch(finestra, y, x, daStampare);
                x++;
            }
            x=posx;
		    y++;
        }
        pthread_mutex_unlock(&mtx);
    } else if(mode == CANCELLA){
        pthread_mutex_lock(&mtx);
        for(j=0;j<dim_spritex;j++){
            for(k=0;k<dim_spritey;k++){
                daStampare = oggetto[j][k];
                mvwaddch(finestra, y, x, ' ');
                x++;
            }
            x=posx;
		    y++;
        }
        pthread_mutex_unlock(&mtx);
    }
}

/**
 * Genera un numero dato un intervallo min e max compresi 
 * 
 * @param min intervallo min
 * @param max intervallo max
 * @return numero generato
 */
int generaNumero(int min, int max){
    int numeroGenerato;
    return numeroGenerato = min + rand()%(max-min+1);
}

/**
 * @brief killa tutti i threads del gioco
 * 
 */
void killAllThreads(int mode){
    int i;
    
    if(mode == GAMEWON){
        pthread_mutex_unlock(&mtx);
        for(i=0;i<M;i++){
            if(pNemici[i].id < 0){
                pthread_cancel(tNemici[i]);
            }
        }

        //da fare il ciclo for per chiudere tutte le bombe
        for(i=0;i<M;i++){
            pthread_cancel(tBombe[i]);
        }

        for(i=0;i<NUMERO_MISSILI+1;i++){
            pthread_cancel(tMissili[i]);
        }

        pthread_cancel(tGiocatore);
        pthread_cancel(tControllo);
    } else if(mode == GAMEOVER){
        pthread_mutex_unlock(&mtx);

        for(i=0;i<M;i++){
            pthread_cancel(tNemici[i]);
            pthread_cancel(tBombe[i]);
        }

        for(i=0;i<NUMERO_MISSILI+1;i++){
            pthread_cancel(tMissili[i]);
        }

        pthread_cancel(tGiocatore);
        pthread_cancel(tControllo);
    }
}

void *soundsEffectManager(void *arg){
    int mode = *(int*)arg;

    switch(mode){
        case MORTO: system("aplay -q sounds/morto.wav");
            break;

        case ROCKET: system("aplay -q sounds/missile.wav");
            break;

        case HIT: system("aplay -q sounds/hit.wav");
            break;
    }
}