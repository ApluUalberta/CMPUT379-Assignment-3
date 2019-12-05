compile: FileSystem.cc execute
	g++ -Wall -std=c++11 FileSystem.o -o FileSystem

execute: FileSystem.cc
	g++ -c -Wall -std=c++11 FileSystem.cc

clean:
	rm -f FileSystem FileSystem.o

taring: FileSystem.cc readme.md

	tar -cvf APLU-FileSystem.tar.gz FileSystem.cc readme.md Makefile