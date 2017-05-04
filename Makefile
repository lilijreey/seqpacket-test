all: server client goclient

server: server.c
	gcc server.c -o server

client: client.c
	gcc client.c -o client

goclient: goclient.go
	go build goclient.go

clean:
	rm -f server client goclient server.o client.o
