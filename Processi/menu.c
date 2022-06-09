#include "menu.h"

pid_t testo, pid_ufo, musica;

/**
 * Genera le stelle per la finestra presa in input
 * 
 * @param finestra finestra in cui devono essere generate le stelle
 */
void generaStelle(WINDOW *finestra){
    pos oggetto;
    oggetto.c = '.';
    int x, y;
    pid_t stelle;

    stelle=fork();
    switch(stelle){
        case -1:
            perror("Errore creazione fork stelle");

        case 0:
           while(true){
                //getmaxyx(stdscr, MAXY, MAXX);
                box(stdscr, ACS_VLINE, ACS_HLINE);

                if(generaNumero(0, 16830)%2 == 0){
                    while((oggetto.x>= 18 || oggetto.x <= 70) || (oggetto.y >= 5 || oggetto.y <= 16)){
                        oggetto.x = generaNumero(0, MAXX);
                        oggetto.y = generaNumero(1, MAXY);
                        usleep(800);
                    }
                }
                
                mvwaddch(finestra, oggetto.y, oggetto.x, oggetto.c);
                while((x >= 18 || x <= 70) || (y >= 5 || y <= 16)){
                    x = generaNumero(0, MAXX);
                    y = generaNumero(1, MAXY);
                }
                
                mvwaddch(finestra, y, x, ' ');
                wrefresh(finestra);
            } 
    }
}

/**
 * Stampa il menu del gioco per l'utente
 * 
 */
void stampaMenu(){
    char scelta;
    int status1;
    WINDOW *subwindowSpace = newwin(9,52,1,23);
    WINDOW *subwindowDefender = newwin(9,82,10,8);
    WINDOW *menuDialogBox = newwin(9,36,21,31);

    attenzione();
    clear();
    refresh();
    opening();

    do {
        while(waitpid(-1, &status1, WNOHANG) > 0);
        clear();
        refresh();
        init_pair(1, COLOR_GREEN, COLOR_BLACK); //carattere logo space
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);//carattere logo defender
        init_pair(3, COLOR_WHITE, COLOR_BLACK); //default
        init_pair(5, COLOR_BLUE, COLOR_BLACK);

        //box(subwindowSpace,0,0);
        wattron(subwindowSpace, COLOR_PAIR(1)); //scelgo il verde

        char *space2 = " $$$$$$   $$$$$$$    $$$$$$    $$$$$$   $$$$$$$$";
        char *space3 = "$$    $$  $$    $$  $$    $$  $$    $$  $$";
        char *space4 = "$$        $$    $$  $$    $$  $$        $$";
        char *space5 = " $$$$$$   $$$$$$$   $$$$$$$$  $$        $$$$$";
        char *space6 = "      $$  $$        $$    $$  $$        $$";
        char *space7 = "$$    $$  $$        $$    $$  $$    $$  $$";
        char *space8 = " $$$$$$   $$        $$    $$   $$$$$$   $$$$$$$$";


        mvwprintw(subwindowSpace, 1, 2, "%s", space2);
        mvwprintw(subwindowSpace, 2, 2, "%s", space3);
        mvwprintw(subwindowSpace, 3, 2, "%s", space4);
        mvwprintw(subwindowSpace, 4, 2, "%s", space5);
        mvwprintw(subwindowSpace, 5, 2, "%s", space6);
        mvwprintw(subwindowSpace, 6, 2, "%s", space7);
        mvwprintw(subwindowSpace, 7, 2, "%s", space8);

        attron(COLOR_PAIR(3));

        mvwprintw(stdscr, 7, 6, "==============");
        mvwprintw(stdscr, 8, 5, "===============");

        //newwin = nlinee, ncolonne, y, x
        //box(subwindowDefender, 0, 0);
        wattron(subwindowDefender, COLOR_PAIR(2)); //scelgo il giallo

        //7x100
        char *defender1 = "$$$$$$$   $$$$$$$$  $$$$$$$$  $$$$$$$$  $$    $$  $$$$$$$   $$$$$$$$  $$$$$$$ ";
        char *defender2 = "$$    $$  $$        $$        $$        $$$   $$  $$    $$  $$        $$    $$";
        char *defender3 = "$$    $$  $$        $$        $$        $$$$  $$  $$    $$  $$        $$    $$";
        char *defender4 = "$$    $$  $$$$$     $$$$$     $$$$$     $$ $$ $$  $$    $$  $$$$$     $$$$$$$ ";
        char *defender5 = "$$    $$  $$        $$        $$        $$  $$$$  $$    $$  $$        $$    $$";
        char *defender6 = "$$    $$  $$        $$        $$        $$   $$$  $$    $$  $$        $$    $$";
        char *defender7 = "$$$$$$$   $$$$$$$$  $$        $$$$$$$$  $$    $$  $$$$$$$   $$$$$$$$  $$    $$";

        mvwprintw(subwindowDefender, 1, 2, "%s", defender1);
        mvwprintw(subwindowDefender, 2, 2, "%s", defender2);
        mvwprintw(subwindowDefender, 3, 2, "%s", defender3);
        mvwprintw(subwindowDefender, 4, 2, "%s", defender4);
        mvwprintw(subwindowDefender, 5, 2, "%s", defender5);
        mvwprintw(subwindowDefender, 6, 2, "%s", defender6);
        mvwprintw(subwindowDefender, 7, 2, "%s", defender7);

        attron(COLOR_PAIR(3)); // torno ai colori di default

        mvwprintw(stdscr, 7, 77, "==============");
        mvwprintw(stdscr, 8, 77, "===============");
        mvwprintw(stdscr, 19, 60, "[Mattia Cani & Chiara Ganga]");

        //inizializzo lo score del file o lo leggo e lo stampo nel menu
        refreshScoreFile(0);
        int datoLetto;
        FILE *dat = fopen("score.dat", "rb");
        fread(&datoLetto, sizeof(int), 1, dat); //leggo il dato
        fclose(dat);
        mvwprintw(stdscr, 25, 7, "HIGHSCORE: %d", datoLetto);

        wrefresh(stdscr);
        wrefresh(subwindowSpace);
        wrefresh(subwindowDefender);
        //nlinee, ncolonne, y, x
        
        box(menuDialogBox, 0, 0);

        mvwprintw(menuDialogBox, 0, 9, "  Fai una scelta  ");
        mvwprintw(menuDialogBox, 2, 14, "1. Gioca");
        mvwprintw(menuDialogBox, 3, 13, "2. Comandi");
        mvwprintw(menuDialogBox, 4, 13, "3. Crediti");
        mvwprintw(menuDialogBox, 6, 15, "5. ESCI");
        wrefresh(menuDialogBox);

        switch(scelta = wgetch(menuDialogBox)){
            case UNO:
                wclear(subwindowSpace);
                wclear(subwindowDefender);
                wclear(stdscr);
                story();
                combatti();
                clear();
                start();
                break;

            case DUE:
                wclear(subwindowSpace);
                wclear(subwindowDefender);
                clear();
                comandi();
                break;

            case TRE:
                wclear(subwindowSpace);
                wclear(subwindowDefender);
                clear();
                crediti();
                break;

            case CINQUE:
                wclear(subwindowSpace);
                wclear(subwindowDefender);
                clear();
                kill(musica, SIGUSR1);
                endwin();
                system("killall aplay");
                break;
        }
    } while(scelta != CINQUE);  
}

/**
 * Stampa la schermata di aiuto, mostrando i comandi del gioco
 * 
 */
void comandi(){
    initscr();
    curs_set(0);
    noecho();
    start_color();

    attron(COLOR_PAIR(3));

    box(stdscr, 0, 0);

    char *comandi1 = " e88~-_    ,88~-_        e    e           e      888b    | 888~-_   888";
    char *comandi2 = "d888   \\  d888   \\      d8b  d8b         d8b     |Y88b   | 888   \\  888";
    char *comandi3 = "8888     88888    |    d888bdY88b       /Y88b    | Y88b  | 888    | 888";
    char *comandi4 = "8888     88888    |   / Y88Y Y888b     /  Y88b   |  Y88b | 888    | 888";
    char *comandi5 = "Y888   /  Y888   /   /   YY   Y888b   /____Y88b  |   Y88b| 888   /  888";
    char *comandi6 = " '88_-~    `88_-~   /          Y888b /      Y88b |    Y888 888_-~   888";

    mvprintw(2, 15, comandi1);
    mvprintw(3, 15, comandi2);
    mvprintw(4, 15, comandi3);
    mvprintw(5, 15, comandi4);
    mvprintw(6, 15, comandi5);
    mvprintw(7, 15, comandi6);
    //nlinee, ncolonne, y, x
    WINDOW *controlSubwindow = newwin(10, 40, 10, 30);
    box(controlSubwindow, 0, 0);
    wattron(controlSubwindow, COLOR_PAIR(3));
    mvwprintw(controlSubwindow, 0, 13, "   TASTIERA   ");
    mvwprintw(controlSubwindow, 4, 4, "FRECCIA SU/GIU - MUOVI NAVICELLA");
    mvwprintw(controlSubwindow, 6, 10, "SPAZIO - SPARA MISSILI");

    wrefresh(stdscr);
    wrefresh(controlSubwindow);
    
    mvprintw(28, 60, "Premi un tasto per continuare . . .");
    getch();

    clear();
}

/**
 * Contiene informazioni sugli autori e sulla versione del programma
 * 
 */
void crediti(){
    initscr();
    curs_set(0);
    noecho();

    box(stdscr, 0, 0);

    char *unica1 = ":::    ::: ::::    ::: ::::::::::: ::::::::      :::    ";
    char *unica2 = ":+:    :+: :+:+:   :+:     :+:    :+:    :+:   :+: :+:  ";
    char *unica3 = "+:+    +:+ :+:+:+  +:+     +:+    +:+         +:+   +:+ ";
    char *unica4 = "+#+    +:+ +#+ +:+ +#+     +#+    +#+        +#++:++#++:";
    char *unica5 = "+#+    +#+ +#+  +#+#+#     +#+    +#+        +#+     +#+";
    char *unica6 = "#+#    #+# #+#   #+#+#     #+#    #+#    #+# #+#     #+#";
    char *unica7 = " ########  ###    #### ########### ########  ###     ###";

    mvprintw(2, 22, unica1);
    mvprintw(3, 22, unica2);
    mvprintw(4, 22, unica3);
    mvprintw(5, 22, unica4);
    mvprintw(6, 22, unica5);
    mvprintw(7, 22, unica6);
    mvprintw(8, 22, unica7);
    
    mvprintw(10, 18, "Universita' degli Studi di Cagliari - Anno Accademico 2021/2022");
    mvprintw(12, 23, "Facolta' di Scienze - Corso di Laurea in Informatica");
    mvprintw(14, 36, "Corso di Sistemi Operativi");
    mvprintw(16, 32, "SPACE DEFENDER v1.0 [Pipes Version]");
    mvprintw(18, 33, "Autori: Mattia Cani & Chiara Ganga");

    mvprintw(28, 60, "Premi un tasto per continuare . . .");
    getch();

    clear();
}

/**
 * Stampa la schermata dove mostra una breve introduzione, assieme alle stelle
 * 
 */
void story(){
    int x, y;
    pos oggetto;
    char c;

    WINDOW *stelle = newwin(20, 80, 2, 10);
    WINDOW *introduzione = newwin(7, 50, 22, 25);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    wattron(introduzione, COLOR_PAIR(1));

    oggetto.c = '.';

    while(true){
        //getmaxyx(stdscr, MAXY, MAXX);
        box(stelle, ACS_VLINE, ACS_HLINE);
        //box(introduzione, ACS_VLINE, ACS_HLINE);

        if(generaNumero(0, 16830)%350 == 0){
            do{
                oggetto.x = generaNumero(0, MAXX);
                oggetto.y = generaNumero(1, MAXY);
                usleep(800);
            } while(oggetto.y > 20);
        }
        
        mvwaddch(stelle, oggetto.y, oggetto.x, oggetto.c);

        x = generaNumero(0, MAXX);
        y = generaNumero(1, MAXY);
        
        mvwaddch(stelle, y, x, ' ');

        mvwprintw(introduzione, 2, 2, "    Era una notte tranquilla a Cagliari....                ...fino a quando gli alieni\n              decisero di attaccare");
        mvprintw(28, 65, "Premi spazio per continuare . . .");
        timeout(0);

        c = getch();

        if(c == ' '){
            clear();
            delwin(stelle);
            delwin(introduzione);
            break;
        }

        refresh();
        wrefresh(stelle);
        wrefresh(introduzione);
    }
}

/**
 * Schermata che invita il giocatore a prendere parte al gioco
 * 
 */
void combatti(){
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    
    mvprintw(2, 25, "Allarme... Allarme... Questa non e' un'esercitazione!");
    mvprintw(3, 20, "Il Ministero della Difesa ha dichiarato lo status di emergenza!");
    mvprintw(4, 22, "A tutti gli uomini, pronti a decollare da Elmas Aeroporto!");
    mvprintw(5, 39, "DOBBIAMO COMBATTERE!!!");
    
    attron(COLOR_PAIR(1));

    char *alieno1 =  "         .////                       ////         ";
    char *alieno2 =  "         /@@@@                       @@@@         ";
    char *alieno3 =  "              @@@@@             @@@@@             ";
    char *alieno4 =  "              @@@@@             @@@@@             ";
    char *alieno5 =  "         /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@         ";
    char *alieno6 =  "         /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@         ";
    char *alieno7 =  "     @@@@@@@@@     @@@@@@@@@@@@@     @@@@@@@@@    ";
    char *alieno8 =  "     @@@@@@@@@     @@@@@@@@@@@@@     @@@@@@@@@    ";
    char *alieno9 =  "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    char *alieno10 = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    char *alieno11 = "@@@@@    /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@";
    char *alieno12 = "@@@@@    /@@@@                       @@@@     @@@@";
    char *alieno13 = "@@@@@    /@@@@                       @@@@     @@@@";
    char *alieno14 = "              @@@@@@@@@   @@@@@@@@@@@             ";
    char *alieno15 = "              @@@@@@@@@   @@@@@@@@@@@             ";

    mvprintw(7, 25, alieno1);
    mvprintw(8, 25, alieno2);
    mvprintw(9, 25, alieno3);
    mvprintw(10, 25, alieno4);
    mvprintw(11, 25, alieno5);
    mvprintw(12, 25, alieno6);
    mvprintw(13, 25, alieno7);
    mvprintw(14, 25, alieno8);
    mvprintw(15, 25, alieno9);
    mvprintw(16, 25, alieno10);
    mvprintw(17, 25, alieno11);
    mvprintw(18, 25, alieno12);
    mvprintw(19, 25, alieno13);
    mvprintw(20, 25, alieno14);
    mvprintw(21, 25, alieno15);

    attron(COLOR_PAIR(2));

    mvprintw(25, 27, "QUESTI BESTIONI NON SI SCONFIGGERANNO DA SOLI!");
    mvprintw(28, 65, "Premi spazio per continuare . . .");
    
    refresh();

    timeout(-1);
    getch();

    clear();
    refresh();
}

/**
 * Schermata che consente di conoscere le grandezze consigliate per il terminale, per evitare bug grafici
 * 
 */
void attenzione(){
    box(stdscr, 0, 0);

    //nlinee, ncolonne, y, x
    WINDOW *attenzione = newwin(17,43, 3, 27);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    wattron(attenzione, COLOR_PAIR(1));

    box(attenzione, 0, 0);

    char *cartello1="                  @@@@@@                  ";
    char *cartello2="                 @@,.,.@@                 ";
    char *cartello3="               @@....,..@@&             ";
    char *cartello4="              @@,........@@              ";
    char *cartello5="            *@@,..(@@@@..,@@*            ";
    char *cartello6="           @@,....@@@@@....,@@           ";
    char *cartello7="          @@,.....@@@@@,.....@@          ";
    char *cartello8="        &@#.......@@@@@.......@@@     ";
    char *cartello9="       @@.........*@@@@........,@@       ";
    char *cartello10="     #@@..........@@@&...........@@,     ";
    char *cartello11="    @@,............,..,,........../@@    ";
    char *cartello12="  ,@@.............@@@@@*............@@   ";
    char *cartello13=" @@*...............@@@&.............,@@@";
    char *cartello14="@@,...................................,@#";
    char *cartello15=" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ ";

    mvprintw(2, 30, cartello1);
    mvprintw(3, 30, cartello2);
    mvprintw(4, 30, cartello3);
    mvprintw(5, 30, cartello4);
    mvprintw(6, 30, cartello5);
    mvprintw(7, 30, cartello6);
    mvprintw(8, 30, cartello7);
    mvprintw(9, 30, cartello8);
    mvprintw(10, 30, cartello9);
    mvprintw(11, 30, cartello10);
    mvprintw(12, 30, cartello11);
    mvprintw(13, 30, cartello12);
    mvprintw(14, 30, cartello13);
    mvprintw(15, 30, cartello14);
    mvprintw(16, 30, cartello15);

    mvprintw(20, 39, "ATTENZIONE COMANDANTE!");
    mvprintw(21, 17, "Prima di iniziare a giocare si consiglia di impostare il terminale");
    mvprintw(22, 17, "con grandezza [100x30] per evitare malfunzionamenti e/o bug grafici");

    mvprintw(28, 58, "Premi un tasto per avviare il gioco . . .");
    wrefresh(attenzione);
    refresh();
    getch();
}

/**
 * Stampa il messaggio di benvenuto al gioco di Space Defender
 * 
 */
void welcome(){
    int i = 0, j = 40, altezzaSD = 20; //altezzaSD = altezza messaggio benvenuto
    char *string = "       Welcome         \n";
    //mvprintw(20, 20, "%s", string);
    usleep(7020000);
    int lenght = strlen(string);

    //il ciclo visualizza gradualmente la frase
    for(i=0;i<lenght;i++){
        mvprintw(altezzaSD, j, "%c", string[i]);
        refresh();
        usleep(50000);
        j++;
    }

    sleep(2);

    _exit(EXIT_SUCCESS);
}

/**
 * Stampa l'animazione dell'ufo sullo schermo
 * 
 */
void ufo(){
    int i;
    long int speed;

    char *ufo1 = "       _.---._       ";
    char *ufo2 = "     .'       '.     ";
    char *ufo3 = " _.-~===========~-._ ";
    char *ufo4 = "(___________________)";
    char *ufo5 = "      \\_______/      ";

    char *raggio1 = "     /         \\";
    char *raggio2 = "   /             \\";
    char *raggio3 = " /                 \\";

    speed = SPEED_UFO_INIZIO;

    for(i=-20;i<150;i++){
        clear();
        mvprintw(3, i, ufo1);
        mvprintw(4, i, ufo2);
        mvprintw(5, i, ufo3);
        mvprintw(6, i, ufo4);
        mvprintw(7, i, ufo5);
        usleep(speed);

        if(i == 40){
            //sleep(3);
            mvprintw(8, i, raggio1);
            refresh();
            usleep(100000);
            mvprintw(9, i, raggio2);
            refresh();
            usleep(100000);
            mvprintw(10, i, raggio3);
            refresh();
            usleep(2500000);
            speed = SPEED_UFO_FINE;
        }
        refresh();
    }
    clear();
    refresh();
}

/**
 * Unisce varie funzioni per creare la schermata di inizio gioco prima del menu' con la musica e le animazioni
 * 
 */
void opening(){
    char c;
    int status1;
    clear();
    refresh();

    switch(musica = fork()){
        case -1:
            perror("Errore fork musica");
        case 0:
            system("aplay -q sounds/start1.wav");
            while(true){
                system("aplay -q sounds/ost.wav");
                system("aplay -q sounds/start.wav");
            }
            break;

    }

    while(waitpid(-1, &status1, WNOHANG) > 0);

    switch(testo = fork()){
        case -1:
            perror("Errore fork testo");

        case 0:
            welcome();
            _exit(EXIT_SUCCESS);
            break;
    }

    while(waitpid(-1, &status1, WNOHANG) > 0);

    switch(pid_ufo = fork()){
                case -1:
                    perror("Errore fork ufo");

                case 0:
                    ufo();
                    _exit(EXIT_SUCCESS);
                    break;

                default:
                    sleep(9);
                    clear();
                    refresh();
                    sleep(2);
                    kill(pid_ufo, 1);
                    kill(testo, 1);      
    }
    
    //pulisco gli zombie
    while(waitpid(-1, &status1, WNOHANG) > 0);
}
