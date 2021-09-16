# 046267 Computer Architecture - Winter 20/21 - HW #2

cacheSim: cacheSim.h cacheSim.cpp
	g++ -std=c++11 -Wall -Werror -DNDEBUG --pedantic-errors -o cacheSim cacheSim.cpp

.PHONY: clean
clean:
	rm -f *.o
	rm -f cacheSim
