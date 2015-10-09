TARGETS=chatroom arraylist.o linkedlist.o

chatroom: chatroom.c arraylist.o linkedlist.o
	clang -g -pthread -o chatroom chatroom.c arraylist.o linkedlist.o

arraylist.o: arraylist.h arraylist.c
	clang -g -c arraylist.c arraylist.h

linkedlist.o: linkedlist.h linkedlist.c
	clang -g -c linkedlist.c linkedlist.h

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
