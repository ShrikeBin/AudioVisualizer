CC = gcc
CFLAGS = -Iinclude -Ilib -O2 -std=c17 -Wall
LIBS = -lm -lpthread -ldl

# The final executable name
TARGET = visualizer

# All source files
SRCS = src/visualizer.c lib/kiss_fft.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)