CC=gcc
CFLAGS= -Wall -pedantic -g 
INCLUDES = -I./include
LIB = -L./readline-8.2/prefix/lib
CCLINK = -lreadline

slash : slash.o commands.o star.o redirection.o
	$(CC) $(INCLUDES) -o slash slash.o commands.o star.o redirection.o $(CCLINK)

slash.o : slash.c commands.h star.h redirection.h
	$(CC) $(CFLAGGS) -c -o slash.o slash.c

commands.o : commands.c commands.h
	$(CC) -c $(CFLAGS) -o commands.o  commands.c

star.o : star.c star.h
	$(CC) -c $(CFLAGS) -o star.o star.c

redirection.o : redirection.c redirection.h
	$(CC) -c $(CFLAGS) -o redirection.o redirection.c

clean :
	rm -rf *.o

cleanall:
	rm -rf *.o slash