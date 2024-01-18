APP=lab11essN32491
SRCS=lab11essN32491.c
OBJS=$(SRCS:.c=.o)
СС=gcc
CFLAGS=-Wall -Wextra -Werror -O3
LDFLAGS=-lm

.PHONY: all clean
all: $(APP)
clean:
	rm -rf *.o $(APP)
$(APP): $(OBJS)
	$(CC) -o $@ $^
.c.o:
	$(CC) $(CFLAGS) -c $<
