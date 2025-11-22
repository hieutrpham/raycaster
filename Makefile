INCLUDE=-I./raylib-5.5_linux_amd64/include/
LIB=-L./raylib-5.5_linux_amd64/lib/ -l:libraylib.a -lm
FLAGS=-Wall -Wextra -g

all: main.c
	cc $(FLAGS) $(INCLUDE) -o main main.c $(LIB)
	./main
