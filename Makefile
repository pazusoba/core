# set it to `mac` or `win`
PLATFORM=win

# assign different commands
ifeq ($(PLATFORM), mac)
	MAKE=make
	CLEAN=rm -rf ../**/*.out ../**/*.dSYM ../**/.DS_Store ../**/*.o ../**/*.so ../**/*.dll ../**/*.pazusoba
	CLEAN_AUTOMATION=rm -rf automation/*.png automation/*.out automation/*.pazusoba automation/.ipynb*/
	OUTPUT=./a.out
	MOVE_BOARDS=mv board_*.txt assets/
	OUTPUT_PYTHON=pazusoba.out
	MOVE_PYTHON=mv $(OUTPUT_PYTHON) automation/
else ifeq ($(PLATFORM), win)
	MAKE=mingw32-make.exe
	CLEAN=del /s /q *.exe *.o *.so *.dll *.pazusoba
	CLEAN_AUTOMATION=del /s /q automation\*.out automation\*.pazusoba automation\.ipynb* automation\__py*
	OUTPUT=./a.exe
	MOVE_BOARDS=move /y board_*.txt assets/
	OUTPUT_PYTHON=pazusoba.exe
	MOVE_PYTHON=move /y $(OUTPUT_PYTHON) automation/
endif

# Ofast - increase the speed quite significantly (use it with causion), O3 should be good enough
# lpthread - thread
# flto - linking
OPTIMISATION=-Os -flto -pthread -lpthread
# shared arguments
GCC=g++ -Wall -Werror -std=c++11

# v1 or v2, decides the source to compile
VERSION=v1
CPP_FILES=core/$(VERSION)/*.cpp

build:
	$(GCC) $(OPTIMISATION) main_$(VERSION).cpp $(CPP_FILES)

test:
	$(GCC) $(OPTIMISATION) main_$(VERSION).cpp $(CPP_FILES) -o test
	./test

debug:
	$(GCC) -g main_$(VERSION).cpp $(CPP_FILES)

clean:
	$(CLEAN)
	$(CLEAN_AUTOMATION)

# compile for automation
python:
	$(GCC) $(OPTIMISATION) main_$(VERSION).cpp $(CPP_FILES) -o $(OUTPUT_PYTHON)
	$(MOVE_PYTHON)

# get 20 boards of all sizes (76, 65, 54)
board:
	python3 assets/board_gen.py 7x6 20
	python3 assets/board_gen.py 6x5 20
	python3 assets/board_gen.py 5x4 20
	$(MOVE_BOARDS)
