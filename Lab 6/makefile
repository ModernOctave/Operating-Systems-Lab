IMG=squirrel

part1: tsfm.c
	gcc -lm -o tsfm tsfm.c
	./tsfm ${IMG}.ppm

part2_1a: tsfm_atomic.c
	gcc -o tsfm tsfm_atomic.c
	./tsfm ${IMG}.ppm

part2_1b: tsfm_semaphore.c
	gcc -o tsfm tsfm_semaphore.c
	./tsfm ${IMG}.ppm

part2_2: tsfm_shared_memory.c
	gcc -o tsfm tsfm_shared_memory.c
	./tsfm ${IMG}.ppm

part2_3: tsfm_pipe.c
	gcc -o tsfm tsfm_pipe.c
	./tsfm ${IMG}.ppm

test: part1 part2_1a part2_1b part2_2 part2_3

run: compile
	./tsfm ${img}.ppm

clean:
	rm -f tsfm output.ppm