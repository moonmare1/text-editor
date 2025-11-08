#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "editor.h"

void showMenu() {
    system("cls");
    printf("Console Text Editor\n");
    printf("==================\n\n");
    printf("1. Open File\n");
    printf("2. Create New File\n");
    printf("3. Exit\n\n");
    printf("Enter your choice (1-3): ");
}

int main() {
    Editor editor;
    char filename[PATH_MAX];
    int choice;

    while (1) {
        showMenu();
        choice = _getch() - '0';
        
        if (choice == 3) { // Exit
            break;
        }
        
        system("cls");
        printf("Enter file path: ");
        gets_s(filename, PATH_MAX);
        
        initEditor(&editor);
        
        if (choice == 1) { // Open existing file
            if (!openFile(&editor, filename)) {
                printf("Error: Could not open file '%s'\n", filename);
                printf("Press any key to continue...");
                _getch();
                continue;
            }
        } else if (choice == 2) { // Create new file
            createNewFile(&editor, filename);
        }

        // Main editing loop
        while (1) {
            system("cls");
            displayEditor(&editor);

        // Обработка ввода
            int input = _getch();
            if (input == 27) { // ESC
                if (editor.isModified) {
                    printf("\nFile has unsaved changes. Save before exit? (y/n): ");
                    char choice = _getch();
                    if (choice == 'y' || choice == 'Y') {
                        saveFile(&editor);
                    }
                }
                break;
            } else if (input == 19) { // Ctrl+S
                saveFile(&editor);
            }

            handleInput(&editor, input);
        }

        cleanupEditor(&editor);
    }

    return 0;
}