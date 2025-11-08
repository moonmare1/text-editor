@echo off
echo Compiling Console Text Editor...
gcc -o editor src/main.c src/editor.c -I src
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo You can run the editor by typing 'editor'
) else (
    echo Compilation failed!
    pause
)