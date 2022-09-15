A daemon that synchronizes two subdirectories

How to run and test:

final:
    gcc main.c -o start
    
./start -a "source_path" -b "target_path" [-R (recursion)] [-s size] [-t time]
Przykład: ./start -a ~ /Desktop/Projekt/a -b ~ /Desktop/Projekt/ b - R -s 2048 -t 60

Testing by using script:
./test.sh
