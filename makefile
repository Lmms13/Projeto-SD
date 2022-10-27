# Grupo 58
# Luís Santos 56341
# Afonso Aleluia 56363
# Daniel Marques 56379

data.o = src/data.c
entry.o = src/entry.c
tree.o = src/tree.c
client-lib.o = obj/data.o obj/entry obj/message.o client_stub.o network_client.o sdmessage.pb-c.o
tree-server = tree_server.o data.o entry.o tree.o message.o tree_skel.o network_server.o sdmessage.pb-c.o
tree-client = tree_client.o data.o entry.o message.o client_stub.o network_client.o sdmessage.pb-c.o

headers = -I ./include
objectPath = obj
binaryPath = bin
flags = -g -c -o
CC = gcc 

all: obj/client-lib.o bin/tree-client bin/tree-server

obj/client-lib.o

obj/tree.o: $(tree.o) 
	$(CC) $(tree.o) $(headers) $(flags) $(objectPath)/tree.o

obj/entry.o: $(entry.o) 
	$(CC) $(entry.o) $(headers) $(flags) $(objectPath)/entry.o

obj/data.o: $(data.o) 
	$(CC) $(data.o) $(headers) $(flags) $(objectPath)/data.o

clean: 
	rm -f ./obj/*.o
	rm -f ./bin/*
