# build
build:
	g++ -Wall -Werror main.cpp lib/solver.cpp

run:
	./a.exe

# remove all exe files
clean:
	del /s *.exe

all:
	mingw32-make.exe clean
	mingw32-make.exe build
	mingw32-make.exe run

# get 20 boards of all sizes
generate_board:
	python assets/board_gen.py 7x6 20
	python assets/board_gen.py 6x5 20
	python assets/board_gen.py 5x4 20
	move board_*.txt assets/
