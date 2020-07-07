# This Makefile only works on windows machine. There need to be some tweaks to make it work on Linux or Mac
CPP_FILES=lib/solver.cpp lib/board.cpp lib/state.cpp
GCC=g++ -Wall -Werror
CLEAN=del /s *.exe
MAKE=mingw32-make.exe

# build
build:
	$(GCC) main.cpp $(CPP_FILES)

test:
	$(GCC) test.cpp $(CPP_FILES)
	./a.exe

# run the program
run:
	./a.exe

# run the program with sample 76 board
run76:
	./a.exe assets/sample_board_76.txt

# run the program with sample 65 board
run65:
	./a.exe assets/sample_board_65.txt

# remove all exe files
clean:
	$(CLEAN)

# clean, build, run all in one
all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) run

# get 20 boards of all sizes (76, 65, 54)
board_gen:
	python assets/board_gen.py 7x6 20
	python assets/board_gen.py 6x5 20
	python assets/board_gen.py 5x4 20
	move /y board_*.txt assets/
