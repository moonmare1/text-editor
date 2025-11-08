# Console Text Editor

A simple console text editor with clipboard support.

## Features

- Basic text editing
- Arrow key navigation
- Clipboard support (copy, cut, paste)
- Text selection
- Text scrolling
- Line numbers display

## Controls

- **Arrow Keys** - move cursor
- **Enter** - new line
- **Backspace** - delete character
- **Ctrl+C** - copy selected text
- **Ctrl+X** - cut selected text
- **Ctrl+V** - paste text from buffer
- **Ctrl+S** - start/end text selection
- **ESC** - exit

## Компиляция

```bash
gcc -o editor src/main.c src/editor.c -I src
```

## Использование

```bash
./editor
```