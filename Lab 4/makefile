FILE = tests/process3.dat

build: build-sjf build-srtf

sjf: build-sjf
	./sjf $(FILE)

build-sjf: sjf.cpp
	g++ -o sjf sjf.cpp

srtf: build-srtf
	./srtf $(FILE)

build-srtf: srtf.cpp
	g++ -o srtf srtf.cpp

clean:
	rm -f sjf srtf