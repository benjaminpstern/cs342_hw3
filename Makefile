TARGETS=chatroom arraylist.o

chatroom: chatroom.c arraylist.o
	clang -g -pthread -o chatroom chatroom.c arraylist.o

arraylist.o: arraylist.h arraylist.c
	clang -g -c arraylist.c arraylist.h

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
