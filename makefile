# Grupo 58
# Luís Santos 56341
# Afonso Aleluia 56363
# Daniel Marques 56379

data.o = include/data.h
entry.o = include/entry.h include/data.h  
tree.o = include/tree.h include/entry.h include/data.h 
client-lib.o = data.o entry.o sdmessage.pb-c.o message.o client_stub.o network_client.o
tree-server = data.o entry.o tree.o message.o tree_skel.o network_server.o sdmessage.pb-c.o
tree-client = data.o entry.o message.o client_stub.o network_client.o sdmessage.pb-c.o
#proto = src/sdmessage.proto
sdmessage.pb-c.o = include/sdmessage.pb-c.h
client_stub.o = include/client_stub.h
message.o = include/message-private.h
network_client.o = include/network_client.h
network_server.o = include/network_server.h
tree_client.o = include/network_client.h include/message-private.h include/client_stub.h 
tree_server.o = include/tree_skel.h include/network_server.h
tree_skel.o = include/tree_skel.h

headers = -I ./include
libraries = -L ./lib
objectPath = obj
binaryPath = bin
libraryPath = lib
sourcePath = src
flags = -g -c -o
CC = gcc 

all: lib/client-lib.o bin/tree-client bin/tree-server

%.o: $(sourcePath)/%.c
	$(CC) $(headers) -g -o $(objectPath)/$@ -c $<

lib/client-lib.o: $(client-lib.o)
	ld -r $(headers) $(addprefix $(objectPath)/,$^) -o $(libraryPath)/client-lib.o

bin/tree-client: $(tree_client)
	$(CC) $(libraryPath)/client-lib.o src/tree_client.c $(headers) $(addprefix $(objectPath)/,$^) -lprotobuf-c -g -o $(binaryPath)/tree-client

bin/tree-server: $(tree-server)
	$(CC) src/tree_server.c $(headers) $(addprefix $(objectPath)/,$^) -lprotobuf-c -g -o $(binaryPath)/tree-server

# obj/tree.o: $(tree.o) 
# 	$(CC) $(tree.o) $(headers) $(flags) $(objectPath)/tree.o

# obj/entry.o: $(entry.o) 
# 	$(CC) $(entry.o) $(headers) $(flags) $(objectPath)/entry.o

# obj/data.o: $(data.o) 
# 	$(CC) $(data.o) $(headers) $(flags) $(objectPath)/data.o

# obj/client_stub.o: $(client_stub.o)
# 	$(CC) $(client_stub.o) $(headers) $(flags) $(objectPath)/client_stub.o

# obj/sdmessage.pb-c.o: $(sdmessage.pb-c.o)
# 	$(CC) $(sdmessage.pb-c.o) $(headers) -lprotobuf-c $(flags) $(objectPath)/sdmessage.pb-c.o

# obj/message.o: $(message.o)
# 	$(CC) $(message.o) $(headers) $(flags) $(objectPath)/message.o

# obj/network_client.o: $(network_client.o)
# 	$(CC) $(network_client.o) $(headers) $(libraries) $(flags) $(objectPath)/network_client.o

# obj/network_server.o: $(network_server.o)
# 	$(CC) $(network_server.o) $(headers) $(flags) $(objectPath)/network_server.o

# obj/tree_client.o: $(tree_client.o)
# 	$(CC) $(tree_client.o) $(headers) $(flags) $(objectPath)/tree_client.o

# obj/tree_server.o: $(tree_server.o)
# 	$(CC) $(tree_server.o) $(headers) $(flags) $(objectPath)/tree_server.o

# obj/tree_skel.o: $(tree_skel.o)
# 	$(CC) $(tree_skel.o) $(headers) $(flags) $(objectPath)/tree_skel.o


# proto: $(proto)
# 	protoc $(proto) --c_out=.
# 	mv src/sdmessage.pb-c.h ./inc

clean: 
	rm -f ./$(objectPath)/*.o
	rm -f ./$(binaryPath)/*
	rm -f ./$(libraryPath)/*
