Demon synchronizujący dwa podkatalogi


Sposób uruchamiania i testowania projektu:

Stworzenie makefile.
final:
    gcc main.c -o start

Uruchomienie programu:
./start -a "source_path" -b "target_path" [-R (recursion)] [-s size] [-t time]
Przykład: ./start -a ~ /Desktop/Projekt/a -b ~ /Desktop/Projekt/ b - R -s 2048 -t 60

Testowanie programu za pomocą skryptu:
./test.sh


Zaimplementowane funkcje:

void killDeamon - uśpienie demona
void wakeUpDeamon - obudzenie demona
void forking - funkcja forkująca rodzica
void validateNumberOfArguments -  sprawdzenie ilości argumentów
void deleteExtra - funkcja usuwająca plik
void makePath - funkcja tworząca ścieżkę
void synchronize - funkcja synchronizująca podkatalogi
void copy - kopiowanie pliku z katalogu źródłowego do katalogu docelowego
void setTime - ustawienie czasu dla pliku 
