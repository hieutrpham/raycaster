INCLUDE=-I./raylib-5.5_linux_amd64/include/
LIB=-L. -L./raylib-5.5_linux_amd64/lib/ -l:libraylib.so -lm
FLAGS=-Wall -Wextra -Werror
SFLAGS= -Wl,-rpath,./ -Wl,-rpath,./raylib-5.5_linux_amd64/lib/

all: main.c plug
	cc $(FLAGS) $(SFLAGS) $(INCLUDE) -o main main.c $(LIB)

plug: plug.c
	cc $(FLAGS) $(INCLUDE) -fPIC -shared -o libplug.so plug.c $(LIB)
