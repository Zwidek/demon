#!/bin/bash

rm -r ~/Desktop/Projekt/FolderA
mkdir -p ~/Desktop/Projekt/FolderA/1/2/3
echo "1" > ~/Desktop/Projekt/FolderA/1/test1.txt
echo "1" > ~/Desktop/Projekt/FolderA/1/test2.txt
echo "1" > ~/Desktop/Projekt/FolderA/1/test3.txt

echo "1" > ~/Desktop/Projekt/FolderA/1/2/test1.txt

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" > ~/Desktop/Projekt/FolderA/1/2/3/heavyFile.txt

rm -r ~/Desktop/Projekt/FolderB
mkdir -p ~/Desktop/Projekt/FolderB/1/3/2

echo "1" > ~/Desktop/Projekt/FolderB/plikToDelete1.txt

echo "1" > ~/Desktop/Projekt/FolderB/1/test1.txt


echo "1" > ~/Desktop/Projekt/FolderB/1/3/plik0ToDelete.txt

echo "1" > ~/Desktop/Projekt/FolderB/1/3/2/plikToDelete2.txt
