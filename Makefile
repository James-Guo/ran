# Compiler & flags
CC = gcc
CFLAGS = -Wall -g

# Libraries to be linked (empty for now)
LIB = 

# Object files for each executable
OBJ1 = stage1.o data.o list.o
OBJ2 = stage2.o data.o list.o
OBJ3 = stage3.o data.o list.o
OBJ4 = stage4.o data.o list.o

# Define the executable names
EXE1 = dict1
EXE2 = dict2
EXE3 = dict3
EXE4 = dict4

# Targets for each executable
$(EXE1): $(OBJ1)
	$(CC) $(CFLAGS) -o $(EXE1) $(OBJ1) $(LIB)

$(EXE2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXE2) $(OBJ2) $(LIB)

$(EXE3): $(OBJ3)
	$(CC) $(CFLAGS) -o $(EXE3) $(OBJ3) $(LIB)

$(EXE4): $(OBJ4)
	$(CC) $(CFLAGS) -o $(EXE4) $(OBJ4) $(LIB)

# Compilation rules
stage1.o: stage1.c data.h list.h
	$(CC) $(CFLAGS) -c stage1.c

stage2.o: stage2.c data.h list.h
	$(CC) $(CFLAGS) -c stage2.c

stage3.o: stage3.c data.h list.h
	$(CC) $(CFLAGS) -c stage3.c

stage4.o: stage4.c data.h list.h
	$(CC) $(CFLAGS) -c stage4.c

data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

# Clean up generated files
clean: 
	rm -f $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(EXE1) $(EXE2) $(EXE3) $(EXE4)
