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

FLAGS=-std=c++11 -Wall -Wextra -pedantic -fopenmp -O2 -g
FILES=pazusoba.cpp

main:
	$(GCC) $(FLAGS) $(FILES) main.cpp -o pazusoba

test:
	$(GCC) $(FLAGS) $(FILES) test.cpp -o pazusoba-test
	./pazusoba-test

benchmark:
	$(GCC) $(FLAGS) $(FILES) benchmark.cpp -o pazusoba-benchmark
	./pazusoba-benchmark

so:
	$(GCC) $(FLAGS) $(FILES) api.cpp -shared -o libpazusoba.so

clean:
	rm pazusoba pazusoba-test
