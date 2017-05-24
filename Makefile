# Declare the contents of the .PHONY variable as phony.
.PHONY: clean
.PHONY: all

# Make variables (CC, etc...)
CC       = gcc
LD       = ld
CFLAGS   = -Wall -lpthread
#CFLAGS   += -g -DDEBUG

#object files
OBJS     = 

#add a target to generate binary
TARGET   = pthread_rw_sem 
TARGET   += pthread_rw_cond

#install
all: ${TARGET}

# TODO: clean files
clean:
	rm -rf $(TARGET) *.o
