CC = gcc 
CLFAGS = -g -Wall -lpthread
LIBS = #none
OUTPUT = main
HEADERS = main.h

all: $(OUTPUT)

main: main.c $(HEADERS)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $<

clean:
	@rm -f $(OUTPUT)
