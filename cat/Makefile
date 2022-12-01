CC = gcc
CFLAGS = -Wall -Werror -Wextra

.PHONY: clean rebuild check run linter valgrind docker tests docker linter_fix

all : s21_cat

s21_cat : s21_cat.o s21_utils.o
	@echo "======="
	@echo "LINKING"
	@echo "======="
	@$(CC) $(CFLAGS) $^ -o $@	
	@echo "===="
	@echo "DONE"
	@echo "===="

s21_cat.o : s21_cat.c s21_cat.h
	@$(CC) $(CFLAGS) -c $^

s21_utils.o : ../common/s21_utils.c ../common/s21_utils.h
	@$(CC) $(CFLAGS) -c $^

run : s21_cat
	@./s21_cat $(args)

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

tests : s21_cat
	@bash ./tests.sh

valgrind : s21_cat
	@bash ./test_leaks.sh

clean :
	@echo "========"
	@echo "CLEANING"
	@echo "========"
	@rm -f *.o *.gch
	@rm -f s21_cat
	@rm -f .clang-format
	@rm -f *.log
	@rm -f ../common/*.gch
	@echo "===="
	@echo "DONE"
	@echo "===="

rebuild :
	$(MAKE) clean
	$(MAKE) all
