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
