#ifndef EDITOR_H
#define EDITOR_H

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 200
#define CLIPBOARD_SIZE 10000
#define PATH_MAX 260

typedef struct {
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int numLines;
    int cursorX;
    int cursorY;
    int scrollOffset;
    char clipboard[CLIPBOARD_SIZE];
    int selectionStartX;
    int selectionStartY;
    int selectionEndX;
    int selectionEndY;
    int isSelecting;
    char currentFile[PATH_MAX];
    int isModified;
} Editor;

// Вспомогательные функции
int isPositionSelected(Editor* editor, int x, int y);

// Основные функции редактора
void initEditor(Editor* editor);
void cleanupEditor(Editor* editor);
void displayEditor(Editor* editor);
void handleInput(Editor* editor, int input);

// File operations
int openFile(Editor* editor, const char* filename);
int saveFile(Editor* editor);
void createNewFile(Editor* editor, const char* filename);
void showMenu(void);

// Editing functions
void insertChar(Editor* editor, char ch);
void deleteLine(Editor* editor);
void deleteChar(Editor* editor);
void newLine(Editor* editor);

// Функции буфера обмена
void copySelection(Editor* editor);
void cutSelection(Editor* editor);
void pasteText(Editor* editor);

// Функции выделения текста
void startSelection(Editor* editor);
void updateSelection(Editor* editor);
void clearSelection(Editor* editor);

// Функции навигации
void moveCursor(Editor* editor, int dx, int dy);
void scrollEditor(Editor* editor, int direction);

#endif