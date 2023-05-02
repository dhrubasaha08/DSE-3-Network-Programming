all: client server

client: client.c common.h unp.h
		gcc -o client client.c

server: server.c common.h unp.h
		gcc -o server server.c

clean:
		rm -f client server
