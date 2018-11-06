CXX=g++
CPPFLAGS=-std=c++11
LDFLAGS=-L/usr/local/lib/ncl
LDLIBS=-lncl

all: nexus2phyjson

nexus2phyjson: nexus2phyjson.o
	$(CXX) -o nexus2phyjson nexus2phyjson.o $(LDLIBS) $(LDFLAGS)

clean:
	rm nexus2phyjson.o
