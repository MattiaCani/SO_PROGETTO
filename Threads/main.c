#include "menu.h"

int main(){
    initscr();
    noecho();
    curs_set(0);
    start_color();

    stampaMenu();
    echo();
    clear();    
    endwin();
    
    return 0;
}