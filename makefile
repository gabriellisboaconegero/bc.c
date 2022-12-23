CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJS = bc.o

bcc: $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm *.o bcc