# build
build:
	g++ main.cpp lib/solver.cpp

run:
	./a.exe

# remove all exe files
clean:
	del /s *.exe

all:
	mingw32-make.exe clean
	mingw32-make.exe build
	mingw32-make.exe run
