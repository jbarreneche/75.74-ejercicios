# Internet stream version (TCP protocol).
#

CC = g++
CFLAGS = -Wall -std=c++98
# SOURCES = $(wildcard *.cpp)
# OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS = 
EXECS = lanzador.cpp borrar_ipcs.cpp encuentro.cpp
BINS  = $(patsubst %.cpp,bin/%,$(EXECS))
LDFLAGS =

all: $(BINS)

.cpp.o:
	$(CC) -c $(CFLAGS) $<

$(BINS): $(EXECS)
	$(CC) $(LDFLAGS) $(patsubst bin/%,%.cpp,$@) -o $@

clean:
	rm -f *.o

cleanall: $(clean)
	rm -f bin/*
