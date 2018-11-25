CC=gcc
CFLAGS=-Wall -g

OBJS=main.o ooo.o

TARGET=core_simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm *.o
	rm $(TARGET)
