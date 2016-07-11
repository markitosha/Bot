.PHONY: all clean install uninstall

all: src/main.cpp 
	g++ -g src/main.cpp -o bot

clean:
	rm -rf bot *.o
	rm -rf bot *.dSYM