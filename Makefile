# set it to `mac` to use max/linux command and use `win` for windows
PLATFORM=win

# assign different commands
ifeq ($(PLATFORM), mac)
	MAKE=make
	CLEAN=rm *.out
	OUTPUT=./a.out
	MOVE=mv board_*.txt assets/
else ifeq ($(PLATFORM), win)
	MAKE=mingw32-make.exe
	CLEAN=del /s *.exe
	OUTPUT=./a.exe
	MOVE=move /y board_*.txt assets/
endif

# shared arguments
# Ofast - increase the speed quite significantly (use it with causion), O3 should be good enough
OPTIMISATION=-Ofast -flto
GCC=g++ -Wall -Werror -std=c++11 $(OPTIMISATION)
CPP_FILES=lib/solver.cpp lib/board.cpp lib/state.cpp lib/queue.cpp

# build
build:
	$(GCC) main.cpp $(CPP_FILES)

test:
	$(GCC) test.cpp $(CPP_FILES)
	$(OUTPUT)

# run the program
run:
	$(OUTPUT)

# run the program with sample 76 board
run76:
	$(OUTPUT) assets/sample_board_76.txt

# run the program with sample 65 board
run65:
	$(OUTPUT) assets/sample_board_65.txt

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
	python3 assets/board_gen.py 7x6 20
	python3 assets/board_gen.py 6x5 20
	python3 assets/board_gen.py 5x4 20
	$(MOVE)
