@echo off
cd /d D:\Documentos\GitHub\ParralelWordCounter_MPI

:: Activate the virtual environment
call .venv\Scripts\activate.bat

:: Clean up previous build files
rmdir /s /q build __pycache__ >nul 2>&1
del Controller.spec >nul 2>&1

:: Delete old .exe from Executables folder if it exists
if exist Executables\Controller.exe del /f /q Executables\Controller.exe

:: Build the new .exe
pyinstaller --onefile --noconsole Controller.py

:: Move the .exe to the final Executables folder
move /Y dist\Controller.exe Executables\

:: Remove leftover temp folders
rmdir /s /q build dist __pycache__ >nul 2>&1

echo Build complete! Your EXE is in \Executables
pause