CFLAGS = -O2 -Wall
objects = main.o
target = rc-panel-info-memory

%.o : %.c
	$(CC) $(CFLAGS) -c $<

$(target) : $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(target)

.PHONY : clean
clean: 
	rm -f $(objects) $(target)
