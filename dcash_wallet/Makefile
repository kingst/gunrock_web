all: dcash

CC = g++
CFLAGS = -g -Werror -Wall -I/usr/local/opt/openssl@1.1/include -I include -I ../shared/include
LDFLAGS = -L/usr/local/opt/openssl@1.1/lib -lssl -lcrypto -pthread
VPATH = ../shared

OBJS = wallet.o MySocket.o HTTPClientResponse.o HttpClient.o MySslSocket.o WwwFormEncodedDict.o Base64.o StringUtils.o

-include $(OBJS:.o=.d)

dcash: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS) 

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
	rm -f dcash *.o *~ core.* *.d
