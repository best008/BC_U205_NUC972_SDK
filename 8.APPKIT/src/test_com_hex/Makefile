OBJS = *.c

.c.o:
	$(CC) $(CFLAGS) -O2  -c $*.c 
	
all:$(OBJS)
	$(CC) $(CFLAGS) -l pthread ${LDFLAGS} -O2 -o $(MODNAME) $(OBJS) 

install:
	cp -rf  $(MODNAME)	$(PREFIX)/$(MODNAME)

distclean:
	rm -f *.o *.so* *.swp 

clean:
	rm -f *.o *.so* *.swp 
