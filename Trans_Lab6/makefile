CC = gcc
CFLAGS = -Wall -g -std=c99
LDFLAGS = -lm
RM = rm -f

tml : tml.o fileoperations.o
	$(CC) $(LDFLAGS) -o $@ $^

tml.o fileoperations.o: %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY:  clean

clean:
	$(RM) *.o tml
