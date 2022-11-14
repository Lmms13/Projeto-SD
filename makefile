# Grupo 58
# Luís Santos 56341
# Afonso Aleluia 56363
# Daniel Marques 56379

data.o = include/data.h
entry.o = include/entry.h include/data.h  
tree.o = include/tree.h include/entry.h include/data.h 
sdmessage.pb-c.o = include/sdmessage.pb-c.h
client_stub.o = include/client_stub.h
message.o = include/message-private.h
network_client.o = include/network_client.h include/network-private.h
network_server.o = include/network_server.h include/network-private.h
tree_client.o = include/network_client.h include/message-private.h include/client_stub.h 
tree_server.o = include/tree_skel.h include/network_server.h
tree_skel.o = include/tree_skel.h
#proto = src/sdmessage.proto

client-lib.o = data.o entry.o sdmessage.pb-c.o message.o client_stub.o network_client.o
tree-server = data.o entry.o tree.o message.o tree_skel.o network_server.o sdmessage.pb-c.o
tree-client = data.o entry.o message.o client_stub.o network_client.o sdmessage.pb-c.o

headers = -I ./include
libraries = -L ./lib
objectPath = object
binaryPath = binary
libraryPath = lib
sourcePath = source
flags = -g -c -o
CC = gcc 

all: $(libraryPath)/client-lib.o $(binaryPath)/tree-client $(binaryPath)/tree-server

%.o: $(sourcePath)/%.c
	$(CC) $(headers) -g -o $(objectPath)/$@ -c $<

lib/client-lib.o: $(client-lib.o)
	ld -r $(headers) $(addprefix $(objectPath)/,$^) -o $(libraryPath)/client-lib.o

binary/tree-client: $(tree_client)
	$(CC) $(libraryPath)/client-lib.o $(sourcePath)/tree_client.c $(headers) $(addprefix $(objectPath)/,$^) -lprotobuf-c -g -o $(binaryPath)/tree-client

binary/tree-server: $(tree-server)
	$(CC) $(sourcePath)/tree_server.c $(headers) $(addprefix $(objectPath)/,$^) -lprotobuf-c -g -o $(binaryPath)/tree-server

# proto: $(proto)
# 	protoc $(proto) --c_out=.
# 	mv src/sdmessage.pb-c.h ./inc

clean: 
	rm -f ./$(objectPath)/*.o
	rm -f ./$(binaryPath)/*
	rm -f ./$(libraryPath)/*
