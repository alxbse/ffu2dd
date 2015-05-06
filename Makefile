TARGET = ffu2dd 
CFLAGS = -g
LDFLAGS = -lm
ODIR = obj

SRC = main.o
OBJ = $(patsubst %,%,$(SRC))

%.o: %.c 
	gcc -c -o $@ $< $(CFLAGS)
$(TARGET): $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o
