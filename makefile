CC=gcc
CFLAGS=-ansi -W  
EXEC=Ksecret

all: $(EXEC)

Ksecret:	main_test.o blowfish.o
					$(CC) -o $@ $^

blowfish.o:	blowfish.c
						$(CC) -o $@ -c $< $(CFLAGS)

main_test.o: 	main_test.c blowfish.h
							$(CC) -o $@ -c $< $(CFLAGS)

clean: 
				rm -rf *.o

mrproper:
				rm -rf $(EXEC)
					
