@echo off
cd /d D:\Documentos\GitHub\ParralelWordCounter_MPI

:: Activar el entorno virtual
call .venv\Scripts\activate.bat

:: Limpiar archivos anteriores
rmdir /s /q build __pycache__ >nul 2>&1
del Controller.spec >nul 2>&1

:: Construir el .exe
pyinstaller --onefile --noconsole Controller.py

:: Mover el ejecutable
move /Y dist\Controller.exe Executables\

:: Limpiar restos
rmdir /s /q build dist __pycache__ >nul 2>&1

echo Build complete! Your EXE is in \Executables
pause