all: matmul

matmul: matrix.cc main.cc
	mpicxx -o matmul main.cc matrix.cc

clean:
	rm -f *.o matmul