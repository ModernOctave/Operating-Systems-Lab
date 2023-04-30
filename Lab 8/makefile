build: fifo lru rand

test: ${pol}
	./${pol} 60 5 60 ${prog}.dat

fifo: fifo.cpp
	g++ -o fifo fifo.cpp

lru: lru.cpp
	g++ -o lru lru.cpp

rand: rand.cpp
	g++ -o rand rand.cpp