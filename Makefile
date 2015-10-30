CFLAGS=-g

all: network-sender network-listener message-processor noise-maker result-logger

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

network-sender: network-sender.o shared_defs.h
	gcc $(CFLAGS) network-sender.o -o $@

network-listener: network-listener.o shared_defs.h
	gcc $(CFLAGS) network-listener.o -o $@

message-processor: message-processor.o shared_defs.h
	gcc $(CFLAGS) message-processor.o -o $@

noise-maker: noise-maker.o shared_defs.h
	gcc $(CFLAGS) noise-maker.o -o $@

result-logger: result-logger.o
	gcc $(CFLAGS) result-logger.o -o $@

clean:
	rm -f *.o
	rm -f network-sender network-listener message-processor result-logger
