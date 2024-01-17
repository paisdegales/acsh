project=acsh

src_folder=src
lib_folder=lib
obj_folder=obj

src_files=$(wildcard $(src_folder)/*.c)
lib_files=$(wildcard $(lib_folder)/*.h)
obj_files=$(subst $(src_folder),$(obj_folder),$(src_files:.c=.o))
include=$(lib_folder)

CFLAGS=-Wall -g -Wpedantic -Werror
CC=gcc

all: | objdir

objdir:
	mkdir -p $(obj_folder)

all: $(project)

$(project): $(obj_files)
	@$(CC) $^ -o $@ $(CFLAGS)

$(obj_folder)/%.o: $(src_folder)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) -I $(include)

run:
	@./$(project)

val:
	@valgrind --leak-check=full --show-leak-kinds=all -s ./$(project)

clean:
	@rm $(obj_files) $(project)
	@rmdir $(obj_folder)

debug:
	@gdb $(project)

man:
	man 7 signal 2 sigprocmask sigaction gdb

echo:
	@echo "Folders:" "$(src_folder)" "$(lib_folder)" "$(obj_folder)" 
	@echo "Files:" "$(src_files)" "$(lib_files)" "$(obj_files)" 
