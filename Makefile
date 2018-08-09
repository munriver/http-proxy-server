CC = gcc
CFLAGS = -Wall -g -std=gnu99

SOURCES = proxy.c
TARGET = proxy


objects = $(SOURCES:.c=.o)

proxy: $(objects) 
	$(CC) $(CFLAGS) -o $(TARGET) $(objects)

all: $(TARGET)

clean: 
	@rm -f *.o $(TARGET) 

tags:
	@ctags $(SOURCES)

submit: clean
	@tar zcf pa1.tar.gz $(SOURCES) $(HEADERS) README Makefile
