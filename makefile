LIB = -L/usr/local/lib -lpng12
INC=/usr/include/libpng12 /usr/include/png++

INC_FLAGS=$(INC:%=-I%)

default: main

main: renderer.cpp
	g++ -Ofast -std=c++14 -Wall -Wextra $(INC_FLAGS) -c renderer.cpp
	g++ -o renderer renderer.o -lpng12
clean:
	rm -r renderer renderer.o
