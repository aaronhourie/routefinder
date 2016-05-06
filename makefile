CC = gcc
CFLAGS = -std=c99 -g -Wall -Werror -pedantic
OBDIR = obj/

routefinder: src/routefinder.c fwd_table.o
	$(CC) $(CFLAGS) src/routefinder.c obj/fwd_table.o -o routefinder

fwd_table.o: src/fwd_table.c 
	$(CC) $(CFLAGS) -c src/fwd_table.c -o obj/fwd_table.o

memcheck: routefinder
	valgrind --leak-check=full --show-leak-kinds=all --log-file="memcheck/dump.txt" ./routefinder in/input
	less memcheck/dump.txt

clean:
	-rm obj/fwd_table.o

spotless: clean
	-rm routefinder
