PROJECT=Acsh

SRC_FOLDER=src
LIB_FOLDER=lib
OBJ_FOLDER=obj

SRC_FILES=$(wildcard $(SRC_FOLDER)/*.c)
LIB_FILES=$(wildcard $(LIB_FOLDER)/*.h)
OBJ_FILES=$(subst $(SRC_FOLDER),$(OBJ_FOLDER),$(SRC_FILES:.c=.o))

FLAGS=-Wall -g -Wpedantic -Werror
CC=gcc
INCLUDE=$(LIB_FOLDER)

all: | OBJDIR

OBJDIR:
	mkdir -p $(OBJ_FOLDER)

all: $(PROJECT)

$(PROJECT): $(OBJ_FILES)
	@$(CC) $^ -o $@ $(FLAGS)

$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.c $(LIB_FILES)
	$(CC) -c $< -o $@ $(FLAGS) -I $(INCLUDE)

run:
	@./$(PROJECT)

val:
	@valgrind --leak-check=full --show-leak-kinds=all -s ./$(PROJECT)

clean:
	@rm $(OBJ_FILES) $(PROJECT)
	@rmdir $(OBJ_FOLDER)

debug:
	@gdb $(PROJECT)

man:
	man 7 signal 2 sigprocmask sigaction gdb

echo:
	@echo "Folders:" "$(SRC_FOLDER)" "$(LIB_FOLDER)" "$(OBJ_FOLDER)" 
	@echo "Files:" "$(SRC_FILES)" "$(LIB_FILES)" "$(OBJ_FILES)" 
