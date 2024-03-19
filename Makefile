
CC      := gcc
FLAGS   := -std=c99 -O3 -Wall

FILES   := $(wildcard src/*.c)

#RCI EDUARDO CASANOVA E EDUARDO SILVESTRE 2023/24

all: bin COR COR2

bin:
	mkdir -p bin

COR: $(FILES) 
	$(CC) -lc -o bin/$@ $^ $(FLAGS)

COR2: $(FILES)
	$(CC) -lc -o COR $^ $(FLAGS)

clean: 
	rm -rf bin/*
	rm -rf COR