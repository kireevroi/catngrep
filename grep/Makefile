CC = gcc
CFLAGS = -Wall -Werror -Wextra

.PHONY: clean rebuild check run linter valgrind docker tests docker linter_fix

all : s21_grep

s21_grep_debug : s21_grep.o s21_utils.o
	$(CC) $(CFLAGS) $^ -o s21_grep -g

s21_grep : s21_grep.o s21_utils.o
	@echo "======="
	@echo "LINKING"
	@echo "======="
	@$(CC) $(CFLAGS) $^ -o $@	
	@echo "===="
	@echo "DONE"
	@echo "===="

s21_grep.o : s21_grep.c s21_grep.h
	@$(CC) $(CFLAGS) -c $^

s21_utils.o : ../common/s21_utils.c ../common/s21_utils.h
	@$(CC) $(CFLAGS) -c $^

run : s21_grep
	@./s21_grep $(args)

check : linter docker valgrind tests cppcheck

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem ../

linter :
	@echo "======="
	@echo "LINTING"
	@echo "======="
	@cp ../../materials/linters/.clang-format ./.clang-format
	@clang-format -n -style=google *.c *.h
	@cp .clang-format ../common/
	@clang-format -n -style=google ../common/*.c ../common/*.h
	@rm ../common/.clang-format
	@rm -f .clang-format
	@echo "===="
	@echo "DONE"
	@echo "===="

linter_fix :
	@echo "======="
	@echo "LINTING"
	@echo "======="
	@cp ../../materials/linters/.clang-format ./.clang-format
	@clang-format -i -style=google *.c *.h
	@cp .clang-format ../common/
	@clang-format -i -style=google ../common/*.c ../common/*.h
	@rm ../common/.clang-format
	@rm -f .clang-format
	@echo "===="
	@echo "DONE"
	@echo "===="

docker :
	@sudo bash ../../materials/build/run.sh

tests : s21_grep
	@bash ./tests.sh

valgrind : s21_grep_debug
	@bash ./test_leaks.sh

clean :
	@echo "========"
	@echo "CLEANING"
	@echo "========"
	@rm -f *.o *.gch
	@rm -f s21_grep
	@rm -f .clang-format
	@rm -f *.log
	@rm -f ../common/*.gch
	@echo "===="
	@echo "DONE"
	@echo "===="

rebuild :
	$(MAKE) clean
	$(MAKE) all
