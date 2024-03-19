
CC      := gcc
FLAGS   := -std=c99 -O3 -Wall

FILES   := $(wildcard src/*.c)

#RCI EDUARDO CASANOVA E EDUARDO SILVESTRE 2023/24

all: bin COR 

bin:
	mkdir -p bin

COR: $(FILES) 
	$(CC) -lc -o bin/$@ $^ $(FLAGS) 

clean: 
	rm -rf bin/*