.PHONY: all init run clean
all: init run clean
init:
	clang++ -Xpreprocessor -fopenmp -std=c++11 src/main.cpp -o ./raytracer -lomp
run:
	./raytracer
clean:
	rm -f ./raytracer
