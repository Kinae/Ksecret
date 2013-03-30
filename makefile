CC=gcc
CFLAGS=-ansi -W  

BINDIR=bin/
OBJDIR=obj/
SRCDIR=src/

EXEC=$(BINDIR)Ksecret

all: $(EXEC)

$(BINDIR)Ksecret:	$(OBJDIR)main_test.o $(OBJDIR)blowfish.o
					$(CC) -o $@ $^

$(OBJDIR)blowfish.o:	$(SRCDIR)blowfish.c
						$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)main_test.o: 	$(SRCDIR)main_test.c $(SRCDIR)blowfish.h
							$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean: 
				rm -rf $(OBJDIR)*.o

mrproper: clean
				rm -rf $(EXEC)
					
