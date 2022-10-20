# Grupo 58
# Luís Santos 56341
# Afonso Aleluia 56363
# Daniel Marques 56379

serialization.o = src/serialization.c
data.o = src/data.c
entry.o = src/entry.c
tree.o = src/tree.c
test_data = obj/data.o src/test_data.c
test_entry = obj/data.o obj/entry.o src/test_entry.c
test_tree = obj/data.o obj/entry.o obj/serialization.o obj/tree.o src/test_tree.c

headers = -I ./include
objectPath = obj
binaryPath = bin
flags = -g -c -o
CC = gcc 

all: bin/test_data bin/test_entry bin/test_tree

bin/test_tree: $(test_tree) 
	$(CC) $(test_tree) $(headers) -g -o $(binaryPath)/test_tree

bin/test_entry: $(test_entry) 
	$(CC) $(test_entry) $(headers) -g -o $(binaryPath)/test_entry

bin/test_data: $(test_data) 
	$(CC) $(test_data) $(headers) -g -o $(binaryPath)/test_data

obj/tree.o: $(tree.o) 
	$(CC) $(tree.o) $(headers) $(flags) $(objectPath)/tree.o

obj/serialization.o: $(serialization.o) 
	$(CC) $(serialization.o) $(headers) $(flags) $(objectPath)/serialization.o

obj/entry.o: $(entry.o) 
	$(CC) $(entry.o) $(headers) $(flags) $(objectPath)/entry.o

obj/data.o: $(data.o) 
	$(CC) $(data.o) $(headers) $(flags) $(objectPath)/data.o

clean: 
	rm -f ./obj/*.o
	rm -f ./bin/*
