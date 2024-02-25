
CC      := gcc
FLAGS   := -std=c99 -O3 -Wall

FILES   := $(wildcard src/*.c)



all: bin COR

bin:
	mkdir -p bin

COR: $(FILES)
	$(CC) -lc -o bin/$@ $^ $(FLAGS) 

clean: 
	rm -rf bin/*