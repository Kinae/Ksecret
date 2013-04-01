CC=gcc

ifdef DEBUG 
	CFLAGS= -ansi -W -g    
else
	CFLAGS= -ansi -W  
endif

BINDIR=bin/
OBJDIR=obj/
SRCDIR=src/

EXEC=$(BINDIR)Ksecret
OBJ=$(OBJDIR)main.o $(OBJDIR)blowfish.o $(OBJDIR)encryption.o $(OBJDIR)decryption.o 

all: $(EXEC)

$(BINDIR)Ksecret:	$(OBJ)
									$(CC) -o $@ $^

$(OBJDIR)blowfish.o:	$(SRCDIR)blowfish.c $(SRCDIR)blowfish.h
											$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)encryption.o: $(SRCDIR)encryption.c $(SRCDIR)encryption.h
											$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)decryption.o: $(SRCDIR)decryption.c $(SRCDIR)decryption.h
											$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)main.o: $(SRCDIR)main.c $(SRCDIR)encryption.h $(SRCDIR)decryption.h
											$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean: 
				rm -rf $(OBJDIR)*.o

mrproper: clean
				rm -rf $(EXEC)
