# Nile - A Terminal File Manager (ncurses)
 
A simple terminal-based file manager written in C using the ncurses library.

Nile allows basic filesystem navigation and file operations directly inside the terminal using keyboard controls.

# Demo
https://github.com/user-attachments/assets/8e1da1a2-6296-4897-9e8a-e01b68de0f84

## Features
* Browse directories interactively
* Scroll through files using arrow keys
* Open directories
* View file contents (cat style)
* Change working path dynamically
* Clean terminal UI using ncurses windows
* Create new files

## Compilation and Running:
```bash
gcc -w main.c -lncurses -o fm  
./fm
```

## ToDo:
- [ ] Delete Files and directories.
- [ ] Copy and Move functionalities.
- [ ] Data about the files/dirs. 
- [ ] Colors?
