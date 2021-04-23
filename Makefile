all: gunrock_web

CC = g++
CFLAGS = -g -Werror -Wall -I include
LDFLAGS = -pthread

OBJS = gunrock.o MyServerSocket.o MySocket.o HTTPRequest.o HTTPResponse.o http_parser.o HTTP.o HttpService.o HttpUtils.o FileService.o dthread.o

-include $(OBJS:.o=.d)

gunrock_web: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(OBJS)

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
	rm -f gunrock_web *.o *~ core.* *.d
