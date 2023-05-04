all: client server

client: client.c unp.h
		gcc -o client client.c

server: server.c unp.h
		gcc -o server server.c

clean:
		rm -f client server dump.txt

tdump:	tdump
		dos2unix tdump
