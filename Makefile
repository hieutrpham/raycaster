CC = cc
NAME = main
SFLAGS= -Wl,-rpath,./ -Wl,-rpath,./raylib-5.5_linux_amd64/lib/
CFLAGS = -Wall -Werror -Wextra -Wno-missing-field-initializers
DFLAGS = -MMD -MP
INCLUDE=-I./raylib-5.5_linux_amd64/include/
LIB=-L. -L./raylib-5.5_linux_amd64/lib/ -l:libraylib.a -lm

# sources
SRC = main.c game.c utils.c

# objects
OBJ = $(SRC:.c=.o)
OBJ_PATH = obj/
OBJS = $(addprefix $(OBJ_PATH), $(OBJ))

# dependancies
DEP = $(addprefix $(OBJ_PATH), $(SRC:.c=.d))

all: static

# -- hot reload compile --
hot: plug
	$(CC) $(CFLAGS) -DHOT_RELOAD $(SFLAGS) $(INCLUDE) -o main main.c $(LIB)

plug: plug.c
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -shared -o libplug.so plug.c $(LIB)

# -- static compile --
static: $(OBJ_PATH) $(NAME)

$(OBJ_PATH)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DFLAGS) -o $@ -c $<


$(OBJ_PATH):
	mkdir -p $(OBJ_PATH)

.SECONDARY: $(OBJS)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

-include $(DEP)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean static

debug:
	$(MAKE) CFLAGS="$(CFLAGS) -g" re

TEST_DIR = ./tests
test:
	$(MAKE) -C $(TEST_DIR)
	$(TEST_DIR)/test

.PHONY: all clean fclean re debug test
