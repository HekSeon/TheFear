@echo off
echo CMake yapilandirmasi baslatiliyor...

cmake -S . -B build -G "Visual Studio 18 2026" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo HATA: CMake yapilandirmasi basarisiz oldu.
    pause
    exit /b 1
)

echo.
echo Basarili! build\TheFear.slnx dosyasini acabilirsin.
pause