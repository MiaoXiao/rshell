COMPILER = g++
CFLAGS = -Wall -Werror -ansi -pedantic
FILEPATH = ./src/main.cpp
OUTPATH = ./bin/rshell

all:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(FILEPATH) -o $(OUTPATH)
rshell:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(FILEPATH) -o $(OUTPATH)
clean:
	rm -rf ./bin
