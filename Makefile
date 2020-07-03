# This Makefile only works on windows machine. There need to be some tweaks to make it work on Linux or Mac

# build
build:
	g++ -Wall -Werror main.cpp lib/solver.cpp

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
	del /s *.exe

# clean, build, run all in one
all:
	mingw32-make.exe clean
	mingw32-make.exe build
	mingw32-make.exe run

# get 20 boards of all sizes (76, 65, 54)
board_gen:
	python assets/board_gen.py 7x6 20
	python assets/board_gen.py 6x5 20
	python assets/board_gen.py 5x4 20
	move /y board_*.txt assets/
