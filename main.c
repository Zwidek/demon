#include <syslog.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>
#include <signal.h>
#include <sys/mman.h>
#include <time.h>

void validateNumberOfArguments(int argc);
bool validateGivenPath(char* argv);
void deleteExtra(char* source, char* destination);
void makePath(char* path, char* fileName, char* result);
void copy(char* sourcePath, char* targetPath);
void forking();
bool excludeFileNames(struct dirent* file);
void synchronize(char* source, char* destination, bool recursion, int size);
void setTime(char* destinationFile, time_t modificationTime);
void copyBiggerFiles(char* source, char* destination);

bool ifSignal;
int daemonTime;

int main(int argc, char* argv[]) {


	//sprawdza ilośc argumentów (minimum 2)
 	validateNumberOfArguments(argc);
	
	char* source;
	char* destination;
	daemonTime = 300;
	int size = 256;
	
	bool recursion;
	/*		inicjalizacja programu
		wprowadzenie przez użytkownika argumentów:
	- główny katalog (&a)
	- katalog docelowy (&b)
	- rozmiar pliku (opcjonalnie, &s)
	- czas spania demona (opcjonalnie, &t - standardowo 300 sekund)
	- "-R" (opcjonalnie, rekurencyjna synchronizacja katalogów)
	*/
	int choice;
	while((choice = getopt (argc, argv, "a:b:t:s:R")) != -1)
	switch(choice)
	{
		case 'a':
			source = optarg;		
			break;
		case 'b':
			destination = optarg;
			break;
		case 't':
			daemonTime = atoi(optarg);
			if(time < 0)
			{
			printf("Podano nieprawidłowy czas: \"%d\"\n", daemonTime);
			exit(1); 
			}
			break;
		case 's':
			size = atoi(optarg);
			if(size <= 0)
			{
			printf("Podano nieprawidłowy rozmiar pliku: \"%d\"\n", size);
			exit(1);
			}
			break;
		case 'R':
			recursion = true; 
			break;
		case '?':
			printf("Nieznana opcja: \"%c\"\n", optopt);
			break;				
	}
	
	//sprawdza czy odpowiednie ścieżki zostały podane
	if(validateGivenPath(source) && validateGivenPath(destination))
	{
		printf("Podano prawidłowe ścieżki do pliku source oraz destination\n");
	}else
	{
		printf("Podano błędną ścieżkę do katalogu source lub destination\n");
		exit(1);
	}
	
	//forkowanie	
	forking();
	// główna pętla
	ifSignal = true;
	while(signal)
	{
		syslog(LOG_INFO, "Uśpienie demona...");	
		sleep(daemonTime); 	
		syslog(LOG_INFO, "Obudzenie się demona...");	
		deleteExtra(source, destination);
		synchronize(source, destination, recursion, size);
	}

	
	
	
}

void killDaemon(int signum) {
    syslog(LOG_INFO, "Otrzymano SIGTERM: %d", signum);
    ifSignal = false;
}

void wakeUpDaemon(int signum) {
    syslog(LOG_INFO, "Otrzymano SIGUSR1: %d", signum);
    daemonTime = 0;
}

//funkcja forkująca rodzica
void forking()
{
    syslog(LOG_INFO, "Forkowanie");
    //forkowanie rodzica
    pid_t pid, sid;
    pid = fork();
    if(pid < 0){
        syslog(LOG_ERR, "Nie udało się rozdzielić procesu");
        exit(1);
    }
    if(pid > 0){
        exit(0);
    }
    
    //wyświetla pid w momencie zatrzymania programu kill &getpid();
    printf("Process PID = \"%d\" został uruchomiony\n", getpid());
    
    //zmiana maski plików
    umask(0);
    //tworzenie SIDa dla dziecka
    sid = setsid();
    if(sid < 0){
        syslog(LOG_ERR, "Nie udało się utworzyć sesji dla potomka");
        exit(1);
    }
    //zmiana katalogu
    if((chdir("/")) < 0) {
        syslog(LOG_ERR, "Nie udało się zmienić katalogu");
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
     (SIGUSR1, wakeUpDaemon);
    signal(SIGTERM, killDaemon);
}

//sprawdza czy liczba argumentów jest poprawna (minimum 2)
void validateNumberOfArguments(int argc)
{
	if(argc < 3)
	{
		printf("Podano nieprawidłową ilość argumentów: \"%d\"\n", argc);
		syslog(LOG_INFO, "Nie udało się zmienić katalogu");
		exit(1);
	}		
}

//sprawdza czy została podana prawidłowa ścieżka oraz waliduje czy nie został wskazany inny rodzaj pliku niż katalog
bool validateGivenPath(char* argv)
{
	DIR* dir = opendir(argv);
	if(!dir)
	{
		syslog(LOG_INFO, "Błędna ścieżka do pliku została podana");
		return false;
	}
	
	else if(ENOENT == errno)
	{
		syslog(LOG_INFO, "Błędna ścieżka do pliku została podana");
		return false;
	}
	syslog(LOG_INFO, "Podano prawidłową ścieżkę");
	return true;
}

// usuwanie
void deleteExtra(char* source, char* destination)
{
	DIR* sourceDirectory = opendir(source);
	DIR* destinationDirectory = opendir(destination);	
	struct dirent* file = NULL;
	char sourcePath[300];
	char destinationPath[300];

	while(file = readdir(destinationDirectory))
	{
		bool check = excludeFileNames(file); // sprawdza pliki z nazwa ".", ".."
		
		//decyzja po typie pliku - regularny lub katalog
		switch(file->d_type)
		{	
				//regularny plik
			case DT_REG:
				makePath(source, file->d_name, destinationPath);
				if(open(destinationPath, O_RDONLY)<0)
				{
					makePath(destination, file->d_name, destinationPath);		
					remove(destinationPath);
					syslog(LOG_INFO,"Plik \"%s\" został usunięty", destinationPath);
				}
				break;
				//katalog z wykluczeniem ('.', '..')
			case DT_DIR:
				if(check == false) continue; 
				makePath(source, file->d_name, sourcePath);	
				makePath(destination, file->d_name, destinationPath);		
				deleteExtra(sourcePath, destinationPath);
				//sprawdza czy plik docelowy znajduje się w ścieżce źródłowej, jeśli nie to usuwamy 
				if(open(sourcePath, O_RDONLY)<0) 
				{
					remove(destinationPath);
					syslog(LOG_INFO,"Plik \"%s\" został usunięty", destinationPath);
				}
		} 			
	}
}

//zwraca false dla plików z nazwa ".", ".."
bool excludeFileNames(struct dirent* file)
{
	if(strcmp( file->d_name, "." )==0 || strcmp( file->d_name, ".." )==0)
		return false;
	return true;
}
	
// tworzy ścieżkę
void makePath(char* path, char* fileName, char* result)
{
	strcpy(result,path);
	strcat(result,"/");
	strcat(result,fileName);
}

void synchronize(char* source, char* destination, bool recursion, int size){
	DIR* sourceFolder = opendir(source);
	DIR* destinationFolder = opendir(destination);
	struct dirent* file = NULL;
	char sourcePath[200];
	char destinationPath[200];
	
	struct stat sourceFileInfo;
	struct stat destinationFileInfo;
				
	while(file = readdir(sourceFolder))
	{
		bool check = excludeFileNames(file);
		makePath(source, file->d_name, sourcePath);
		makePath(destination, file->d_name, destinationPath);
			
		stat(sourcePath, &sourceFileInfo);
		stat(destinationPath, &destinationFileInfo);
		
		if(open(destinationPath, O_RDONLY)<0)
		{
			switch(file->d_type)
			{
				case DT_REG:
					if(sourceFileInfo.st_size < size)
						copy(sourcePath, destinationPath);
					copyBiggerFiles(sourcePath, destinationPath);
					
					setTime(destinationPath,sourceFileInfo.st_mtime);
					break;
				case DT_DIR:
					if(check == false) continue;
					mkdir(destinationPath, 0755);
					synchronize(sourcePath, destinationPath, recursion, size);
					break;
				default:
					syslog(LOG_INFO, "Niezdefiniowany plik");
			}
		}	
		else if(open(destinationPath, O_RDONLY)>=0 && sourceFileInfo.st_mtime!=destinationFileInfo.st_mtime)
		{ 
			if(file->d_type == DT_REG)
			{
				remove(destinationPath);
				if(sourceFileInfo.st_size < size)
					copy(sourcePath, destinationPath);
				copyBiggerFiles(sourcePath, destinationPath);	
				
				setTime(destinationPath,sourceFileInfo.st_mtime);
			}
			else if(file->d_type == DT_DIR && check && recursion == true)
			{
				synchronize(sourcePath, destinationPath, recursion, size);			
			}
		}
	}
}

void copy(char* source, char* target){
	int readFromSource, writeToDestination, buffered;
	char size[100];
	readFromSource = open(source, O_RDONLY);
	writeToDestination = open(target, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	syslog(LOG_INFO,"Plik został skopiowany z  \"%s\" do \"%s\"",source,target);
	do
	{
		buffered = read(readFromSource, size, 100);
	}while(buffered);
	close(readFromSource);
	close(writeToDestination);
}

void copyBiggerFiles(char* source, char* destination){
    int read, write;
    struct stat stat;
    off_t len, ret;
    read = open(source, O_RDONLY);
    len = stat.st_size;
    write = open(destination, O_CREAT | O_WRONLY | O_TRUNC, 0777);
    do {
        ret = copy_file_range(read, NULL, write, NULL, len, 0);
        len -= ret;
    } while (len > 0 && ret > 0);
    
    syslog(LOG_INFO,"Plik został skopiowany z  \"%s\" do \"%s\"",source,destination);
    close(read);
    close(write);
}

void setTime(char* destinationFile, time_t modificationTime){
	struct utimbuf time;
	time.actime = modificationTime;
	time.modtime = modificationTime;
	if(utime(destinationFile, &time)<0){
    		syslog(LOG_ERR,"Wystąpił błąd podczas ustawiania czasu pliku: \"%s\"",destinationFile);
		exit(1);
	}
	syslog(LOG_INFO,"Prawidłowo został ustawiony czas pliku \"%s\"",destinationFile);
}

