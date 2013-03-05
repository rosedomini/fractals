
all: fractal doc test

fractal:
	mkdir -p ../bin
	rm -rf ../bin/fractal
	rm -rf fractal
	rm -rf *.o
	gcc *.c -Wall -lSDL -o ../bin/fractal

doc:
	mkdir -p ../doc
	doxygen doxygen-conf.txt

config:
	mkdir -p ../bin
	rm -rf ../bin/Configuration.html
	cp Configuration.html ../bin/Configuration.html

test: config
	mkdir -p ../bin
	rm -rf ../bin/fractal.sh
	cp fractal.sh ../bin/fractal.sh
	../bin/fractal.sh

clean:
	rm -rf *.o
	rm -rf *~

very-clean: clean
	rm -rf ../bin
	rm -rf ../doc

