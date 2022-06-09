#include "oggetti.h"
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

pid_t pidHit = HIT, pidRocket = ROCKET, pidMorto = MORTO;

/**
 * Procedura che si occupa di produrre le coordinate del giocatore dato un input 
 * delle freccie su o giu, oppure lo spazio che manda un input nella pipe che viene
 * letto dalla procedura missile.
 * 
 * @param pipeOutCoordinate pipe di output delle coordinate
 * @param pipeOutNavicella  pipe di output delle coordinate
 */
void giocatore(int pipeOutCoordinate, int pipeOutNavicella){
    //fprintf(stderr, "PID giocatore: %d\n", getpid());
    pos pos_giocatore; //aggiungere valore letto
    int i, status1 = 0, status2 = 0;

    //inizializzo la variabile del giocatore
    pos_giocatore.c='>';
    pos_giocatore.id = 0;
    pos_giocatore.x = GIOCATOREX_DEFAULT;
    pos_giocatore.y = GIOCATOREY_DEFAULT;
    pos_giocatore.pid = getpid();

    //scrivo la posizione iniziale del giocatore nella pipe delle coordinate e nella pipe della naviella
    write(pipeOutCoordinate, &pos_giocatore, sizeof(pos_giocatore));
    write(pipeOutNavicella, &pos_giocatore, sizeof(pos_giocatore));

    //ciclo che legge continuamente l'input e modifica la posizione del giocatore di conseguenza
    while(true){
        char c;
        switch(c=getch()){
            case SU:
                if(pos_giocatore.y > BORDOSUPERIORE) pos_giocatore.y--;
                break;
            case GIU:
                if(pos_giocatore.y< MAXY-2) pos_giocatore.y++;
                break;
            case SPAZIO:
                missile(pos_giocatore, pipeOutCoordinate);
                break;
        }

        //pulisco gli zombie
        while(waitpid(-1, &status1, WNOHANG) > 0);

        //scrivo la coordinata nuova del giocatore in base all'input ricevuto
        write(pipeOutCoordinate, &pos_giocatore, sizeof(pos_giocatore));
    }
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
void missile(pos pos_giocatore, int pipeOutCoordMissile){
    //fprintf(stderr, "PID missile: %d\n", getpid());
    pos missili[NUMERO_MISSILI+1];
    int i, status1;

    //inizializzo i due missili
    for(i=0;i<NUMERO_MISSILI;i++){
        missili[i].c = '*';
        missili[i].id = i+1;
        missili[i].livello = 1;
        missili[i].attivo = true;
    }

    switch(pidRocket = fork()){
        case -1:
            perror("errore fork pidrocket");
        
        case 0:
            soundsEffectManager(ROCKET);
            break;
    }

    while(waitpid(-1, &status1, WNOHANG) > 0);

    switch(missili[MISSILE_UNO].pid = fork()){
        case -1:
            perror("Errore creazione fork missile 1");
            _exit(-1);

        case 0:
            missili[MISSILE_UNO].pid = getpid();
            missili[MISSILE_UNO].x = pos_giocatore.x+3; //alto a destra
            missili[MISSILE_UNO].y = pos_giocatore.y-1;

            do{
                missili[MISSILE_UNO].x+=3;
                if(missili[MISSILE_UNO].x%4){
                    missili[MISSILE_UNO].y-=1;
                }
                
                usleep(TEMPOMOV_MISSILI);
                write(pipeOutCoordMissile, &missili[MISSILE_UNO], sizeof(missili[MISSILE_UNO]));

            } while((missili[MISSILE_UNO].y > LIM_GAMEINFO || missili[MISSILE_UNO].x < MAXX));

            _exit(EXIT_SUCCESS);
    }
    
    switch(missili[MISSILE_DUE].pid = fork()){
        case -1:
            perror("Errore creazione fork missile 2");
            _exit(-1);

        case 0:
            missili[MISSILE_DUE].pid = getpid();
            missili[MISSILE_DUE].x = pos_giocatore.x+2; //basso a destra
            missili[MISSILE_DUE].y = pos_giocatore.y+1;

            do{
                missili[MISSILE_DUE].x+=3;
                if(missili[MISSILE_DUE].x%4){
                    missili[MISSILE_DUE].y+=1;
                }

                usleep(TEMPOMOV_MISSILI);
                write(pipeOutCoordMissile, &missili[MISSILE_DUE], sizeof(missili[MISSILE_DUE]));

            } while((missili[MISSILE_DUE].y < MAXY || missili[MISSILE_DUE].x < MAXX));

            _exit(EXIT_SUCCESS);
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
void nemico(int pipeOutCoordinate, int pipeInNemici){
    //fprintf(stderr, "PID nemico: %d\n", getpid());
    pos nemico;
    int stato;
    pid_t pid_bombe;
    _Bool generaBomba = true;

    //leggo un nemico dalla pipe
    read(pipeInNemici, &nemico, sizeof(nemico));
    nemico.pid = getpid();

    while(true){
        //animo i nemici
        usleep(TEMPOMOV_NEMICI);

        //movimento 1
        nemico.y -= PASSO; //va su'
        write(pipeOutCoordinate, &nemico, sizeof(nemico));

        usleep(TEMPOMOV_NEMICI);

        //movimento 2
        nemico.y += PASSO; //va giu'
        write(pipeOutCoordinate, &nemico, sizeof(nemico));

        usleep(TEMPOMOV_NEMICI);
        
        //movimento 3
        nemico.y += PASSO; //va giu'
        write(pipeOutCoordinate, &nemico, sizeof(nemico));

        usleep(TEMPOMOV_NEMICI); 
        
        //movimento 4
        nemico.y -= PASSO; //va su
        write(pipeOutCoordinate, &nemico, sizeof(nemico));

        usleep(TEMPOMOV_NEMICI);

        //movimento 5
        nemico.x -= PASSO; //va avanti
        write(pipeOutCoordinate, &nemico, sizeof(nemico));

        if(generaBomba && rand()%2==0){
            switch(pid_bombe = fork()){
                case -1:
                    perror("Errore creazione fork");
                    _exit(-1);

                case 0:
                    bomba(nemico, pipeOutCoordinate);
            }
            generaBomba = false;
            stato = ALIVE;
        }
        
        //pulisco gli zombie
        while(waitpid(-1, &stato, WNOHANG) > 0);

        if(stato != ALIVE){
            generaBomba = true;
        } 
    } //parentesi while true
    
    _exit(EXIT_SUCCESS);
}

/**
 * Procedura che legge un nemico e manda le coordinate della bomba nella pipe principale
 * 
 * @param nemico nemico mandato alla procedura su cui generare la bomba
 * @param pipeOutCoordinate pipe in cui scrivere le coordinate delle bombe prodotte
 */
void bomba(pos nemico, int pipeOutCoordinate){
    pos pos_bomba;
    
    pos_bomba.c = spriteBomba;
    pos_bomba.id = nemico.id;
    pos_bomba.livello = 0;
    pos_bomba.pid = getpid();

    pos_bomba.x = nemico.x - 3;
    pos_bomba.y = nemico.y + 1;

    do{
        pos_bomba.x--;
        write(pipeOutCoordinate, &pos_bomba, sizeof(pos_bomba));
        usleep(TEMPOMOV_BOMBE);
    } while(pos_bomba.x >= -1); 
    
    _exit(EXIT_SUCCESS);
}

/**
 * Procedura consumatore, che si occupa di leggere dalla pipe globale delle 
 * coordinate di tutti gli oggetti, pulendo lo schermo e aggiornando i caratteri 
 * stampati.
 * 
 * @param pipeInCoordinate pipe di lettura delle coordinate di tutti gli oggetti del gioco
 */
void controllo(int pipeInCoordinate, int pipeOutNemici, int pipeOutNavicella, int pipeOutCollisioni){
    //fprintf(stderr, "PID controllo: %d\n", getpid());
    pos nemico, giocatore, missili[NUMERO_MISSILI+1], nemici[M], bombe, vettBombe[M], valore_letto;
    int punteggio = 0, vite = 3, vettViteNemici[M], nemiciRimanenti = M, i, j, k, l, distrutti = 0, nemicoX = NEMICOX_DEFAULT, status1; //valori di default

    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);

    //inizializzo i nemici
    inizializzaNemici(nemici, nemicoX);

    //inizializzo le bombe
    inizializzaBombe(vettBombe);

    //inizializzo i missili
    inizializzaMissili(missili);

    //inizializzo il vettore delle vite dei nemici
    for(i=0;i<M;i++){
        vettViteNemici[i] = 3;
    }

    //uso il vettore di nemici inizializzati mandando le coordinate ad ogni nemico tramite N pipe
    for(i=0;i<M;i++){
        write(pipeOutNemici, &nemici[i], sizeof(nemici[i]));
    }

    WINDOW *gameArea = newwin(30, 100, 0, 0);
    
    //endwin();
    usleep(1000); //delay avvio funzione controllo -> bug healthPoints non stampate risolto?
    do{
        wattron(gameArea, COLOR_PAIR(1)); //color giallo
        mvwprintw(gameArea, 1, 18, "VITE: %d", vite);
        mvwprintw(gameArea, 1, 43, "PUNTEGGIO: %d", punteggio);
        mvwprintw(gameArea, 1, 75, "M x %d", nemiciRimanenti);

        wattron(gameArea, COLOR_PAIR(2)); //colori di default

        read(pipeInCoordinate, &valore_letto, sizeof(valore_letto));

        //fprintf(stderr, "Valore letto: %c, id: %d, x: %d, y:%d, livello: %d\n", valore_letto.c, valore_letto.id, valore_letto.x, valore_letto.y, valore_letto.livello);

        //pulisco il vecchio oggetto
        switch(valore_letto.c){
            case 'M':
                if(valore_letto.livello == 2){
                    wattron(gameArea, COLOR_PAIR(3));
                    if(nemici[valore_letto.id].x >= 0){
                        stampaOggetto(gameArea, SPRITE_XY, spriteNemico2, nemici[valore_letto.id].x, nemici[valore_letto.id].y, CANCELLA);
                    }
                    
                    nemici[valore_letto.id] = valore_letto;
                    break;
                } else if(valore_letto.livello == 1){
                    wattron(gameArea, COLOR_PAIR(4));
                    if(nemici[valore_letto.id].x >= 0){
                        stampaOggetto(gameArea, SPRITE_XY, spriteNemico1, nemici[valore_letto.id].x, nemici[valore_letto.id].y, CANCELLA);
                    }
                    
                    nemici[valore_letto.id] = valore_letto;
                    break;
                }
                
            case '>':
                if(giocatore.x >= 0){
                    //mvwaddch(gameArea, giocatore.y, giocatore.x, ' ');
                    stampaOggetto(gameArea, SPRITE_XY, spriteGiocatore, giocatore.x, giocatore.y, CANCELLA);
                }
                giocatore=valore_letto;
                break;

            case '*'://controlla se e' missile 1 oppure 2 dal suo id
            wattron(gameArea, COLOR_PAIR(2));
                if(valore_letto.id == 1){
                    if((missili[valore_letto.id].x >= 0) && (missili[valore_letto.id].y <= MAXY)){ 
                        mvwaddch(gameArea, missili[valore_letto.id].y, missili[valore_letto.id].x, ' ');
                    }
                    missili[valore_letto.id] = valore_letto;
                } else if(valore_letto.id == 2){
                    if((missili[valore_letto.id].x >= 0) && (missili[valore_letto.id].y <= MAXY)){ 
                        mvwaddch(gameArea, missili[valore_letto.id].y, missili[valore_letto.id].x, ' ');
                    }
                    missili[valore_letto.id] = valore_letto;
                }
                break;
            
            case 'O':
                //non stampa se legge il valore inizializzato di default del vettore di bombe
                if(valore_letto.livello >= 0){
                    wattron(gameArea, COLOR_PAIR(5));
                    if(bombe.x >= 0){ 
                        //mvwaddch(gameArea, bombe.y, bombe.x, ' ');
                        mvwprintw(gameArea, vettBombe[valore_letto.id].y, vettBombe[valore_letto.id].x, " ");
                    }
                    //bombe = valore_letto;
                    vettBombe[valore_letto.id] = valore_letto;
                }
                
                break;
        }
      
        //visualizzo l'oggetto nella posizione aggiornata
        if(valore_letto.c == '*'){
            mvwaddch(gameArea, valore_letto.y, valore_letto.x, valore_letto.c);
            //fprintf(stderr, "Ho letto il missile %d\n", valore_letto.pid);
        } else if(valore_letto.c == '>'){
            stampaOggetto(gameArea, SPRITE_XY, spriteGiocatore, giocatore.x, giocatore.y, STAMPA);
        } else if(valore_letto.c == 'M'){
            if(vettViteNemici[valore_letto.id] == 2){
                wattron(gameArea, COLOR_PAIR(1));
                stampaOggetto(gameArea, SPRITE_XY, spriteNemico2, nemici[valore_letto.id].x, nemici[valore_letto.id].y, STAMPA);
            } else if(vettViteNemici[valore_letto.id] == 3){
                wattron(gameArea, COLOR_PAIR(3));
                stampaOggetto(gameArea, SPRITE_XY, spriteNemico1, nemici[valore_letto.id].x, nemici[valore_letto.id].y, STAMPA);
            } else if(vettViteNemici[valore_letto.id] == 1){
                wattron(gameArea, COLOR_PAIR(4));
                stampaOggetto(gameArea, SPRITE_XY, spriteNemico2, nemici[valore_letto.id].x, nemici[valore_letto.id].y, STAMPA);
            }
        } else if(valore_letto.c == 'O'){
            //mvwaddch(gameArea, valore_letto.y, valore_letto.x, valore_letto.c);
            mvwprintw(gameArea, vettBombe[valore_letto.id].y, vettBombe[valore_letto.id].x, "%c", vettBombe[valore_letto.id].c);
        }
        
        curs_set(0);
        wrefresh(gameArea);

        //controllo collisione giocatore-bombe
        for(i=0;i<M;i++){
            if(vettBombe[i].id >= 0){
                if((vettBombe[i].x >= giocatore.x && vettBombe[i].x <= giocatore.x + 2) && (vettBombe[i].y >= giocatore.y && vettBombe[i].y <= giocatore.y + 2)){
                    vite-=1;
                    
                    switch(pidHit = fork()){
                    	case -1:
                    		perror("errore fork pidhit");
                    	
                    	case 0:
                    		soundsEffectManager(HIT);
                    		break;
                    }

                    while(waitpid(-1, &status1, WNOHANG) > 0);

                    vettBombe[i].id = -1;
                    kill(vettBombe[i].pid, SIGHUP);
                    punteggio+=VITA_PERSA;
                    mvwprintw(gameArea, 1, 43, "                   ");
                    wattron(gameArea, COLOR_PAIR(2));
                    stampaOggetto(gameArea, SPRITE_XY, spriteGiocatore, giocatore.x, giocatore.y, STAMPA);
                }
            }
        }

        //controllo collisione missile-nemico
        for(j=0;j<NUMERO_MISSILI+1;j++){ //scorro l'array dei missili
            if(missili[j].attivo == true){ //controllo se il missile e' vivo
                for(i=0;i<M;i++){ //scorro l'array dei nemici ed effettuo i controlli delle collisioni, killando il missile se collide
                    if(vettViteNemici[i] > 0){ //il nemico deve essere vivo
                        if(missili[j].id != -1){ //il missile non deve essere quello inizializzato, ma deve avere dei valori suoi
                            if((missili[j].x >= nemici[i].x && missili[j].x <= nemici[i].x + 2) && (missili[j].y >= nemici[i].y && missili[j].y <= nemici[i].y + 2)){
                                vettViteNemici[i]-=1;
                                
                                switch(pidHit = fork()){
                                    case -1:
                                        perror("errore fork pidhit");
                                    
                                    case 0:
                                        soundsEffectManager(HIT);
                                        break;
                                }

                                while(waitpid(-1, &status1, WNOHANG) > 0);

                                kill(missili[j].pid, SIGHUP);

                                missili[j].x = -1;
                                missili[j].y = -1;
                                
                                punteggio+=NEMICO_COLPITO;
                                mvwprintw(gameArea, 1, 43, "                   ");
                            }
                        }
                    } else if(vettViteNemici[i] == 0 && nemici[i].attivo == true){

                        switch(pidMorto = fork()){
                            case -1:
                                perror("errore fork pidmorto");
                            
                            case 0:
                                soundsEffectManager(MORTO);
                                break;
                        }

                        while(waitpid(-1, &status1, WNOHANG) > 0);

                        kill(nemici[i].pid, SIGHUP);
                        kill(missili[j].pid, SIGHUP);
                        inizializzaMissili(missili);
                        nemiciRimanenti--;

                        punteggio+=NEMICO_MORTO;
                        mvwprintw(gameArea, 1, 43, "                   ");
                        stampaOggetto(gameArea, SPRITE_XY, spriteNemico1, nemici[i].x, nemici[i].y, CANCELLA); //forse si bugga
                        nemici[i].attivo = false;
                    }
                }
            }
        }
        
        //controllo collisione giocatore-nemico -> in esitoGioco con la funzione isInvaso();

        //pulisco gli zombie
        while(waitpid(-1, &status1, WNOHANG) > 0);
    } while(esitoGioco(vite, punteggio, nemiciRimanenti, nemici)); //collisione == true || healthPoints == 0;
    while(waitpid(-1, &status1, WNOHANG) > 0);
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
    start_color();

    clear();

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
        return false;
    } else if(vite == 0 || isInvaso(nemici)){
        gameover();
        refreshScoreFile(punteggio);
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
 * Procedura che riceve il vettore dei nemici e la pipe su cui mandare le coordinate generate
 * 
 * @param nemici vettore dei nemici
 * @param pipeOutCoordinate pipe delle coordinate
 */
void muoviNemici(pos nemico, int pipeOutCoordinate){

    usleep(TEMPOMOV_NEMICI);

    //movimento 1
    nemico.y -= PASSO; //va su'
    write(pipeOutCoordinate, &nemico, sizeof(nemico));

    usleep(TEMPOMOV_NEMICI);

    //movimento 2
    nemico.y += PASSO; //va giu'
    write(pipeOutCoordinate, &nemico, sizeof(nemico));

    usleep(TEMPOMOV_NEMICI);
    
    //movimento 3
    nemico.y += PASSO; //va giu'
    write(pipeOutCoordinate, &nemico, sizeof(nemico));

    usleep(TEMPOMOV_NEMICI); 
    
    //movimento 4
    nemico.y -= PASSO; //va su
    write(pipeOutCoordinate, &nemico, sizeof(nemico));

    usleep(TEMPOMOV_NEMICI);

    //movimento 5
    nemico.x -= PASSO; //va avanti
    write(pipeOutCoordinate, &nemico, sizeof(nemico));
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
}

/**
 * @brief Questa procedura inizializza il vettore delle bombe a dei valori di default.
 * 
 * @param bombe vettore delle bombe
 */
void inizializzaBombe(pos *bombe){
    int i;
    for(i=0;i<M;i++){
        bombe[i].c = 'O';
        bombe[i].attivo = true;
        bombe[i].livello = -1;
        bombe[i].x = -1;
        bombe[i].y = -1;
        bombe[i].id = -1;
        bombe[i].pid = -1;
        bombe[i].delay = -1;
    }
}

/**
 * @brief Procedura che inizializza il vettore dei missili
 * 
 * @param missili vettore dei missili
 */
void inizializzaMissili(pos *missili){
    int i;
    for(i=0;i<NUMERO_MISSILI;i++){
        missili[i].c = '*';
        missili[i].x = -1;
        missili[i].y = -1;
        missili[i].id = i;
        missili[i].attivo = false;
        missili[i].livello = -1;
        missili[i].pid = -1;
    }
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
        for(j=0;j<dim_spritex;j++){
            for(k=0;k<dim_spritey;k++){
                daStampare = oggetto[j][k];
                mvwaddch(finestra, y, x, daStampare);
                x++;
            }
            x=posx;
		    y++;
        }
    } else if(mode == CANCELLA){
        for(j=0;j<dim_spritex;j++){
            for(k=0;k<dim_spritey;k++){
                daStampare = oggetto[j][k];
                mvwaddch(finestra, y, x, ' ');
                x++;
            }
            x=posx;
		    y++;
        }
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

void soundsEffectManager(int mode){
	switch(mode){
        case MORTO: system("aplay -q sounds/morto.wav 2> suono.txt");
            break;

        case ROCKET: system("aplay -q sounds/missile.wav 2> suono.txt");
            break;

        case HIT: system("aplay -q sounds/hit.wav 2> suono.txt");
            break;
    }

    _exit(EXIT_SUCCESS);
}
