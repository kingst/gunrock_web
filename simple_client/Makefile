all: test1

CC = g++
CFLAGS = -g -Werror -Wall -I include
LDFLAGS = -pthread

OBJS = MySocket.o HTTPResponse.o HttpClient.o

-include $(OBJS:.o=.d)

test1: test1.o $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) test1.o $(OBJS)

%.d: %.c
	@set -e; gcc -MM $(CFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@;
	@[ -s $@ ] || rm -f $@

%.d: %.cpp
	@set -e; $(CC) -MM $(CFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@;
	@[ -s $@ ] || rm -f $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f test1 *.o *~ core.* *.d
