all: build

build: display.c battleship.c check.c
	gcc $^ -o battleship -lncurses

clean: battleship
	rm battleship
