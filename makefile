src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

all: server

server: server.o wrap.o
	gcc server.o wrap.o -o server -Wall

%.o:%.c
	gcc -c $< -Wall

.PHONY: clean all
clean: 
	-rm -rf server $(obj)



