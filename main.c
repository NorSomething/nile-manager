#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

// gcc -Wall -Wextra main.c fs.c ui.c -lncurses -o fm

// each window has relative coords (0,0 is start of THAT window)

void get_directories(char *dir_name, char *dir_collection[], int *l) {

    if (strlen(dir_name) == 0) {
        strcpy(dir_name, "/home/nirmal");
        
    }

    struct dirent *de;
    DIR *dr = opendir(dir_name);

    if (!dr) return;

    while((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        if (*l >= 10000) 
            break;

        dir_collection[(*l)++] = strdup(de->d_name); // does not work without strdup idk why
            //strdup to stop overwriting of the buffer, readdir gives reused buffer
            //strup allocates new memory to the heap, copies d_name into that memory, and gives a pointer to it
            //used here cuz char* stuff can sit in any memory (stack, heap, kernelowned, reused internal buffer) => so here each entry points to a differnet heap  memory
            
    }

    closedir(dr);
    
}

int cat(char *file_name, char *dir_name, char *contents_buffer) {

    struct stat st;

    char full_path[strlen(file_name)+strlen(dir_name)+2]; // +1 for / and otehr for null terminator

    int i;

    for (i = 0; i < strlen(dir_name); i++) {
        full_path[i] = dir_name[i];
    }

    full_path[i++] = '/';

    for (int j = 0; j < strlen(file_name); j++) {
        full_path[i++] = file_name[j];
    }

    full_path[i] = '\0';

    FILE *fptr = fopen(full_path, "r");

    if (!fptr) return -1;

    char c;
    int l = 0;

    while ((c = fgetc(fptr)) != EOF) {
        contents_buffer[l++] = c;
    }

    contents_buffer[l] = '\0';
    fclose(fptr);

    return 0;



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
    WINDOW *show_dir_win = newwin(ymax-15, (xmax-4)/2, 14, 2);
    WINDOW *cat_win = newwin(ymax-15, (xmax-4) - ((xmax-4)/2), 14, 2+(xmax-4)/2);
    refresh();

    box(main_win, 0, 0);
    box(heading_win, 0, 0);
    box(fields_win, 0, 0);
    box(show_dir_win, 0, 0);
    box(cat_win, 0, 0);

    wrefresh(main_win);
    wrefresh(heading_win);
    wrefresh(fields_win);
    wrefresh(show_dir_win);
    wrefresh(cat_win);  

    mvwprintw(heading_win, 2, (xmax-22-4)/2, "Nirmal's File Manager.");
   
    wrefresh(heading_win);
    
    mvwprintw(fields_win, 2, 2, "Path:");

    echo();
    mvwgetnstr(fields_win, 2, 9, dir_name, 99);
    noecho();


    int selected = 0; //current selected dir
    int offset = 0; // first visible dir

    int wy, wx; //dir window size
    getmaxyx(show_dir_win, wy, wx);

    int visible = wy - 2;   // minus borders 

    get_directories(dir_name, dir_collection, &l);

    keypad(stdscr, TRUE); //for arrow keys

    char contents_buffer[10000];

    
    while(1) { // draw loop

        int ch = getch();

        if (ch == KEY_DOWN && selected < l-1) {
            selected++;
        }
        if (ch == KEY_UP && selected > 0) {
            selected--;
        }

        //if cursor goes past visible region
        if (selected >= offset + visible) {
            offset++;
        }
        if (selected < offset) {
            offset--;
        }

        mvwprintw(heading_win, 2, 2, dir_name);
        wrefresh(heading_win);

        werase(show_dir_win); //clearing before drawing
        box(show_dir_win, 0, 0);

        //drawing
        for (int i = 0; i < visible && i + offset < l; i++) { // drawing what fits

            if (i + offset == selected) {
                wattron(show_dir_win, A_REVERSE); //window attribute on, highlighting
            }

            mvwprintw(show_dir_win, i+1, 4, dir_collection[i+offset]);
            
            wattroff(show_dir_win, A_REVERSE);

        }

        if (ch == KEY_SR) {

            werase(fields_win);
            box(fields_win, 0, 0);
            wrefresh(fields_win);
            mvwprintw(fields_win, 2, 2, "Path:");
            echo();
            mvwgetnstr(fields_win, 2, 9, dir_name, 99);
            noecho();
            refresh();
            wrefresh(fields_win);
        
            //resettings
            for (int i = 0; i < l; i++) {
                free(dir_collection[i]);
            }

            // need to free whenver we do strdup, malloc, calloc, realloc

            l = 0;
            selected = 0;
            offset = 0;

            werase(show_dir_win);
            box(show_dir_win, 0, 0);
            get_directories(dir_name, dir_collection, &l);
            wrefresh(show_dir_win);
            
            

        }
        


        if (ch == KEY_ENTER || ch == '\n') {
            
            werase(cat_win);
            box(cat_win, 0, 0);
            int temp = cat(dir_collection[selected], dir_name, contents_buffer); // arrays auto decay to pointers so no need to pass as address here?
            mvwprintw(cat_win, 1, 4, contents_buffer);
            wrefresh(cat_win);

        }


        wrefresh(fields_win);
        wrefresh(show_dir_win);

    }

    getch();
    endwin();


    return 0;
}

