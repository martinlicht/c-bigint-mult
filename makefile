
build:
	gcc -std=c99 -pedantic -W -Wall -Wformat -Wextra performance.c -o performance.out 
	gcc -std=c99 -pedantic -W -Wall -Wformat -Wextra example.c -o example.out

clean:
	rm *.out
