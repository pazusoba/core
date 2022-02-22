ifeq ($(OS),Windows_NT) # windows check
    detected_OS := Windows
else
    detected_OS := $(shell uname)
	is_darwin := $(filter Darwin,$(detected_OS))
endif

# check if this is macOS
ifeq ($(is_darwin),Darwin)
	GCC=clang++
else
	GCC=g++
endif

FLAGS=-std=c++11 -Wall -Werror -fopenmp -O2
FILES=pazusoba.cpp

binary:
	$(GCC) $(FLAGS) $(FILES) -o pazusoba