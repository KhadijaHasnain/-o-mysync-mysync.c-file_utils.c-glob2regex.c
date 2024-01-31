CC = gcc
CFLAGS = -std=c11 -Wall 
# Set C_INCLUDE_PATH
C_INCLUDE_PATH := /usr/include/x86_64-linux-gnu

# Export the C_INCLUDE_PATH variable
export C_INCLUDE_PATH

all: mysync

mysync: mysync.c file_utils.c glob2regex.c
	@export C_INCLUDE_PATH=/usr/include/x86_64-linux-gnu
	$(CC) $(CFLAGS) -o mysync mysync.c file_utils.c glob2regex.c

clean:
	rm -f mysync
