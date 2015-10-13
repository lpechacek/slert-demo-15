all: network-listener message-processor result-logger

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

network-listener: network-listener.o shared_defs.h
	gcc network-listener.o -o $@

message-processor: message-processor.o shared_defs.h
	gcc message-processor.o -o $@

result-logger: result-logger.o
	gcc result-logger.o -o $@

clean:
	rm -f *.o
	rm -f network-listener message-processor result-logger
