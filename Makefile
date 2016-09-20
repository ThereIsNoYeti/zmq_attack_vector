OBJDIR =out/obj
LDIR =lib
IDIR =include
SRCDIR =src
OUTDIR =out

CC=gcc
CFLAGS=-I$(IDIR)

TO_MAKE = czmq_leak_c

LIBS=-lzmq -lczmq

_DEPS =
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = $(TO_MAKE).o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TO_MAKE): $(OBJ)
	gcc -o $(OUTDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o $(OUTDIR)/$(TO_MAKE)
