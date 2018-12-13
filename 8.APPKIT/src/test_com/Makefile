# Makefile for systems with GNU tools

OBJS = *.c 

.c.o:
	$(CC) $(CFLAGS) -O2 -c $*.c 

$(MODNAME): $(OBJS) 
	$(CC) $(CFLAGS) -lpthread -O2 -o $(MODNAME) $(OBJS) 

install:
	cp $(MODNAME) $(PREFIX)/

distclean:
	rm -f *.o *.swp  $(MODNAME)

clean:
	rm -f *.o *.swp  $(MODNAME)

