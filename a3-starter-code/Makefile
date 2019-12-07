
fs: compile
	g++ -Wall -std=c++11 -o fs FileSystem.cc

compile: FileSystem.cc execute
	g++ -Wall -std=c++11 FileSystem.o -o FileSystem

execute: FileSystem.cc
	g++ -c -Wall -std=c++11 FileSystem.cc

clean:
	rm -f FileSystem *.o
	rm -f fs

taring: FileSystem.cc README.md

	tar -cvf APLU-FileSystem.tar.gz FileSystem.cc FileSystem.h README.md Makefile