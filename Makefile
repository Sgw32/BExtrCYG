CC=g++
CFLAGS=-c -Wall -std=c++0x -pedantic
LDFLAGS=
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bextr

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	cp ./bextr ../../BufrExtract
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o
