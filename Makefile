COMPILER = g++
CFLAGS = -Wall -Werror -ansi -pedantic
FILEPATH = ./src/main.cpp
FILEPATH2 = ./src/Kirb.cpp
OUTPATH = ./bin/rshell
OUTPATH2 = ./bin/Kirb.o

all:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(FILEPATH) $(FILEPATH2) -o $(OUTPATH)  
rshell:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(FILEPATH) -o $(OUTPATH)
clean:
	rm -rf ./bin
