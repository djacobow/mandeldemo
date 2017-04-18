fractizer: fractizer.c
	gcc -g -O3 -std=c11 -lm fractizer.c -o fractizer

.PHONY: clean
clean:
	rm fractizer 

