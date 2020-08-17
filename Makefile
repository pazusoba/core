# set it to `mac` to use mac/linux command and use `win` for windows
PLATFORM=win

# assign different commands
ifeq ($(PLATFORM), mac)
	MAKE=make
	CLEAN=rm -rf **/*.out **/*.dSYM **/.DS_Store **/*.o **/*.so **/*.dll
	OUTPUT=./a.out
	MOVE=mv board_*.txt assets/
else ifeq ($(PLATFORM), win)
	MAKE=mingw32-make.exe
	CLEAN=del /s *.exe *.o *.so *.dll
	OUTPUT=./a.exe
	MOVE=move /y board_*.txt assets/
endif

# Ofast - increase the speed quite significantly (use it with causion), O3 should be good enough
# lpthread - thread
# flto - linking
OPTIMISATION=-Ofast -flto -lpthread
# shared arguments
GCC=g++ -Wall -Werror -std=c++11 $(OPTIMISATION)
CPP_FILES=lib/*.cpp

# build
build:
	$(GCC) main.cpp $(CPP_FILES)

test:
	$(GCC) test.cpp $(CPP_FILES)
	$(OUTPUT)

debug:
	$(GCC) -g main.cpp $(CPP_FILES)

# run the program
run:
	$(OUTPUT)

# run the program with sample 76 board
run76:
	$(OUTPUT) assets/sample_board_76.txt

# run the program with sample 65 board
run65:
	$(OUTPUT) assets/sample_board_65.txt

# run the program with sample 65 board, step 25 and size 10000
run10000:
	$(OUTPUT) assets/sample_board_65.txt 3 25 10000

# remove all exe files
clean:
	$(CLEAN)

# clean, build, run all in one
all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) run

# build a dll based on solver
dll:
	swig -c++ -python -py3 lib/solver.i
	g++ -c -fPIC -I"C:\Program Files\Python38\include" -L"C:\Program Files\Python38\libs" -lpython38 lib/solver_wrap.cxx
	g++ -c -fPIC lib/*.cpp
	g++ -shared -fPIC *.o -o pazusoba.so 

# get 20 boards of all sizes (76, 65, 54)
board_gen:
	python3 assets/board_gen.py 7x6 20
	python3 assets/board_gen.py 6x5 20
	python3 assets/board_gen.py 5x4 20
	$(MOVE)
