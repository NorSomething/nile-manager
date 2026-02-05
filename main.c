#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <dirent.h>
// gcc -Wall -Wextra main.c fs.c ui.c -lncurses -o fm

// each window has relative coords (0,0 is start of THAT window)

void get_directories(char *dir_name, char *dir_collection[], int *l) {

    struct dirent *de;
    DIR *dr = opendir(dir_name);

    if (!dr) return;

    if (strlen(dir_name) == 0) {
        strcpy(dir_name, ".");
        
    }

    while((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        if (*l >= 10000) 
            break;

        dir_collection[(*l)++] = strdup(de->d_name); // does not work without strdup idk why
            //strdup to stop overwriting of the buffer, readdir gives reused buffer
            
    }

    closedir(dr);
    
}

int test() {
   return 67; 
}

int main() {

    initscr();
    noecho();
    cbreak();

    int ymax, xmax;
    
    char dir_name[100];
    char *dir_collection[10000];
    int l = 0; //if *l = 0 then null pointer so seg fault

    getmaxyx(stdscr, ymax, xmax);

    WINDOW *main_win = newwin(ymax, xmax, 0, 0);
    WINDOW *heading_win = newwin(5, xmax-4, 2, 2);
    WINDOW *fields_win = newwin(5, xmax-4, 8, 2);
    WINDOW *show_dir_win = newwin(ymax-15, xmax-4, 14, 2);
    refresh();

    box(main_win, 0, 0);
    box(heading_win, 0, 0);
    box(fields_win, 0, 0);
    box(show_dir_win, 0, 0);

    wrefresh(main_win);
    wrefresh(heading_win);
    wrefresh(fields_win);
    wrefresh(show_dir_win);

    mvwprintw(heading_win, 2, (xmax-22-4)/2, "Nirmal's File Manager.");
   
    wrefresh(heading_win);
    
    mvwprintw(fields_win, 2, 2, "Path:");

    echo();
    mvwgetnstr(fields_win, 2, 9, dir_name, 99);
    noecho();

    get_directories(dir_name, dir_collection, &l);


    for (int i = 0; i < l; i++) {
        mvwprintw(show_dir_win, i+1, 4, dir_collection[i]);
    }

    wrefresh(fields_win);
    wrefresh(show_dir_win);
    getch();
    endwin();


    return 0;
}

