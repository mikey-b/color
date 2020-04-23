test: FORCE
	g++ test.cc color.cc -O3 -fopenmp -lgd -Wall -o test
	./test

demo: FORCE
	g++ demo.cc color.cc -Ofast -fopenmp -lgd -Wall -DNDEBUG -o demo
	./demo

FORCE:
