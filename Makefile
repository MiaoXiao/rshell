GCC = g++
CFLAGS = -Wall -Werror -ansi -pedantic
FILEPATH = ./src/main.cpp
FILEPATHLS = ./src/ls.cpp
FILEPATH2 = ./src/Kirb.cpp
OUTPATH = ./bin/rshell
OUTPATHLS = ./bin/ls
OUTPATH2 = ./bin/Kirb.o

all: ls
	mkdir -p ./bin
	$(GCC) $(CFLAGS) $(FILEPATH) $(FILEPATH2) -o $(OUTPATH)  
rshell:
	mkdir -p ./bin
	$(GCC) $(CFLAGS) $(FILEPATH) $(FILEPATH2) -o $(OUTPATH)
ls:
	mkdir -p ./bin
	$(GCC) $(CFLAGS) $(FILEPATHLS) -o $(OUTPATHLS)
clean:
	rm -rf ./bin
