CC = gcc
CFLAGS = -Wall -Wextra -std=c11
CLIBS = -lm
OBJS = bc.o

bcc: $(OBJS)
	$(CC) -o $@ $^ $(CLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm *.o bcc
