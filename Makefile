all:
	mkdir -p bin
	gcc -Wall -c common.c
	gcc -Wall -c client.c
	gcc -Wall -c server-mt.c
	gcc -Wall client.o common.o -o client
	gcc -Wall server-mt.c common.o -lpthread -o server-mt

clean:
	rm -f *.o
	rm -rf bin
