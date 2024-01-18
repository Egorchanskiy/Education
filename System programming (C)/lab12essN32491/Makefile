APP=lab12essN32491
SRCS=lab12essN32491.c
LIB=libessN32491.so
OBJS=lab12essN32491.o
СС=gcc
CFLAGS=-Wall -Wextra -Werror -g
LDFLAGS=-lm -ldl
LIBFLAGS=-shared -fPIC

.PHONY: all clean
all: $(APP) $(LIB)
clean:
	rm -rf *.o $(APP) $(LIB)
$(APP): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $^
$(LIB): libessN32491.c
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ 

