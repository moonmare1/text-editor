#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "editor.h"

void initEditor(Editor* editor) {
    memset(editor, 0, sizeof(Editor));
    strcpy(editor->lines[0], "");
    editor->numLines = 1;
    editor->cursorX = 0;
    editor->cursorY = 0;
    editor->scrollOffset = 0;
    editor->isSelecting = 0;
    editor->isModified = 0;
    editor->currentFile[0] = '\0';
}

void cleanupEditor(Editor* editor) {
    // Очистка редактора
    memset(editor, 0, sizeof(Editor));
}

void displayEditor(Editor* editor) {
    int screenHeight = 20; // Количество видимых строк
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Display text lines
    for (int i = editor->scrollOffset; i < editor->scrollOffset + screenHeight && i < editor->numLines; i++) {
        // Display line number
        printf("%3d |", i + 1);

        // Display line content with selection highlighting
        for (int j = 0; editor->lines[i][j]; j++) {
            if (editor->isSelecting && isPositionSelected(editor, j, i)) {
                SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            } else {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            putchar(editor->lines[i][j]);
        }
        
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        printf("\n");
    }

    // Display status information
    printf("\n--- File: %s %s| Line: %d, Column: %d ---\n", 
           editor->currentFile[0] ? editor->currentFile : "New File",
           editor->isModified ? "(Modified) " : "",
           editor->cursorY + 1, 
           editor->cursorX + 1);
    printf("ESC - Exit | Ctrl+S - Save | Ctrl+C - Copy | Ctrl+X - Cut | Ctrl+V - Paste\n");
    
    // Установка позиции курсора
    COORD cursorPos = {editor->cursorX + 5, editor->cursorY - editor->scrollOffset};
    SetConsoleCursorPosition(hConsole, cursorPos);
}

int isPositionSelected(Editor* editor, int x, int y) {
    if (!editor->isSelecting) return 0;

    int startY = editor->selectionStartY;
    int endY = editor->selectionEndY;
    int startX = editor->selectionStartX;
    int endX = editor->selectionEndX;

    if (startY > endY || (startY == endY && startX > endX)) {
        // Swap if selection is reversed
        int tempX = startX;
        int tempY = startY;
        startX = endX;
        startY = endY;
        endX = tempX;
        endY = tempY;
    }

    if (y < startY || y > endY) return 0;
    if (y == startY && x < startX) return 0;
    if (y == endY && x > endX) return 0;
    
    return 1;
}

void handleInput(Editor* editor, int input) {
    if (input == 224) { // Специальные клавиши
        switch (_getch()) {
            case 72: moveCursor(editor, 0, -1); break; // Вверх
            case 80: moveCursor(editor, 0, 1);  break; // Вниз
            case 75: moveCursor(editor, -1, 0); break; // Влево
            case 77: moveCursor(editor, 1, 0);  break; // Вправо
        }
    } else if (input == 8) { // Backspace
        deleteChar(editor);
    } else if (input == 13) { // Enter
        newLine(editor);
    } else if (input == 3) { // Ctrl+C
        copySelection(editor);
    } else if (input == 24) { // Ctrl+X
        cutSelection(editor);
    } else if (input == 22) { // Ctrl+V
        pasteText(editor);
    } else if (input == 19) { // Ctrl+S - начать выделение
        if (!editor->isSelecting) {
            startSelection(editor);
        } else {
            clearSelection(editor);
        }
    } else if (input >= 32 && input <= 126) { // Печатные символы
        insertChar(editor, (char)input);
    }
}

void moveCursor(Editor* editor, int dx, int dy) {
    int newX = editor->cursorX + dx;
    int newY = editor->cursorY + dy;

    // Обновление выделения, если оно активно
    if (editor->isSelecting) {
        editor->selectionEndX = newX;
        editor->selectionEndY = newY;
    }

    // Проверка границ
    if (newY >= 0 && newY < editor->numLines) {
        int lineLen = strlen(editor->lines[newY]);
        if (newX >= 0 && newX <= lineLen) {
            editor->cursorX = newX;
            editor->cursorY = newY;

            // Автоматическая прокрутка
            if (newY < editor->scrollOffset) {
                editor->scrollOffset = newY;
            } else if (newY >= editor->scrollOffset + 20) {
                editor->scrollOffset = newY - 19;
            }
        }
    }
}

void insertChar(Editor* editor, char ch) {
    int len = strlen(editor->lines[editor->cursorY]);
    if (len < MAX_LINE_LENGTH - 1) {
        // Shift characters right
        for (int i = len; i > editor->cursorX; i--) {
            editor->lines[editor->cursorY][i] = editor->lines[editor->cursorY][i-1];
        }
        // Insert new character
        editor->lines[editor->cursorY][editor->cursorX] = ch;
        editor->lines[editor->cursorY][len + 1] = '\0';
        editor->cursorX++;
        editor->isModified = 1;
    }
}

void deleteChar(Editor* editor) {
    if (editor->cursorX > 0) {
        int len = strlen(editor->lines[editor->cursorY]);
        // Сдвигаем символы влево
        for (int i = editor->cursorX - 1; i < len; i++) {
            editor->lines[editor->cursorY][i] = editor->lines[editor->cursorY][i+1];
        }
        editor->cursorX--;
    } else if (editor->cursorY > 0) {
        // Объединяем текущую строку с предыдущей
        int prevLen = strlen(editor->lines[editor->cursorY-1]);
        strcat(editor->lines[editor->cursorY-1], editor->lines[editor->cursorY]);
        
        // Удаляем текущую строку
        for (int i = editor->cursorY; i < editor->numLines - 1; i++) {
            strcpy(editor->lines[i], editor->lines[i+1]);
        }
        editor->numLines--;
        editor->cursorY--;
        editor->cursorX = prevLen;
    }
}

void newLine(Editor* editor) {
    if (editor->numLines < MAX_LINES - 1) {
        // Сдвигаем строки вниз
        for (int i = editor->numLines; i > editor->cursorY + 1; i--) {
            strcpy(editor->lines[i], editor->lines[i-1]);
        }
        
        // Разделяем текущую строку
        strcpy(editor->lines[editor->cursorY + 1], editor->lines[editor->cursorY] + editor->cursorX);
        editor->lines[editor->cursorY][editor->cursorX] = '\0';
        
        editor->numLines++;
        editor->cursorY++;
        editor->cursorX = 0;
    }
}

void copySelection(Editor* editor) {
    if (!editor->isSelecting) return;

    int startY = editor->selectionStartY;
    int endY = editor->selectionEndY;
    int startX = editor->selectionStartX;
    int endX = editor->selectionEndX;

    if (startY > endY || (startY == endY && startX > endX)) {
        // Swap if selection is reversed
        int tempX = startX;
        int tempY = startY;
        startX = endX;
        startY = endY;
        endX = tempX;
        endY = tempY;
    }

    // Копируем выделенный текст в буфер обмена
    editor->clipboard[0] = '\0';
    int clipPos = 0;

    for (int y = startY; y <= endY; y++) {
        int lineStart = (y == startY) ? startX : 0;
        int lineEnd = (y == endY) ? endX : strlen(editor->lines[y]);

        for (int x = lineStart; x < lineEnd && clipPos < CLIPBOARD_SIZE - 1; x++) {
            editor->clipboard[clipPos++] = editor->lines[y][x];
        }

        if (y != endY && clipPos < CLIPBOARD_SIZE - 1) {
            editor->clipboard[clipPos++] = '\n';
        }
    }
    editor->clipboard[clipPos] = '\0';
}

void cutSelection(Editor* editor) {
    copySelection(editor);
    // TODO: Implement deletion of selected text
    clearSelection(editor);
}

void pasteText(Editor* editor) {
    char* text = editor->clipboard;
    while (*text && *text != '\0') {
        if (*text == '\n') {
            newLine(editor);
        } else {
            insertChar(editor, *text);
        }
        text++;
    }
}

void startSelection(Editor* editor) {
    editor->isSelecting = 1;
    editor->selectionStartX = editor->cursorX;
    editor->selectionStartY = editor->cursorY;
    editor->selectionEndX = editor->cursorX;
    editor->selectionEndY = editor->cursorY;
}

void clearSelection(Editor* editor) {
    editor->isSelecting = 0;
}

int openFile(Editor* editor, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    editor->numLines = 0;
    while (fgets(editor->lines[editor->numLines], MAX_LINE_LENGTH, file)) {
        // Remove newline character
        size_t len = strlen(editor->lines[editor->numLines]);
        if (len > 0 && editor->lines[editor->numLines][len-1] == '\n') {
            editor->lines[editor->numLines][len-1] = '\0';
        }
        editor->numLines++;
        if (editor->numLines >= MAX_LINES) break;
    }

    if (editor->numLines == 0) {
        editor->numLines = 1;
        editor->lines[0][0] = '\0';
    }

    strcpy(editor->currentFile, filename);
    editor->isModified = 0;
    fclose(file);
    return 1;
}

int saveFile(Editor* editor) {
    FILE* file = fopen(editor->currentFile, "w");
    if (!file) {
        return 0;
    }

    for (int i = 0; i < editor->numLines; i++) {
        fprintf(file, "%s\n", editor->lines[i]);
    }

    editor->isModified = 0;
    fclose(file);
    return 1;
}

void createNewFile(Editor* editor, const char* filename) {
    strcpy(editor->currentFile, filename);
    editor->numLines = 1;
    editor->lines[0][0] = '\0';
    editor->isModified = 0;
}