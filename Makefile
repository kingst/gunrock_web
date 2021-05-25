all: gunrock_web

CC = g++
CFLAGS = -g -Werror -Wall -I include -I shared/include -I/usr/local/opt/openssl@1.1/include
LDFLAGS = -L/usr/local/opt/openssl@1.1/lib -lssl -lcrypto -pthread
VPATH = shared

OBJS = gunrock.o MyServerSocket.o MySocket.o HTTPRequest.o HTTPResponse.o http_parser.o HTTP.o HttpService.o HttpUtils.o AccountService.o FileService.o TransferService.o dthread.o WwwFormEncodedDict.o StringUtils.o Base64.o AuthService.o DepositService.o HttpClient.o HTTPClientResponse.o MySslSocket.o

-include $(OBJS:.o=.d)

gunrock_web: $(OBJS)
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
	rm -f gunrock_web *.o *~ core.* *.d
