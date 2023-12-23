CC = gcc
CFLAGS = -Wall -pedantic -Werror -std=c99
target = rc-panel-info-memory
source = main.c
release = -O2
debug = -g
valgrind = -ggdb3

$(target): Makefile $(source)
#	@echo "sestavuji"
	$(CC) -o $(target) $(source) $(release) $(CFLAGS)

.PHONY : clean
clean:
#	@echo "provadim clean"
	rm -f $(target)

valgrind: Makefile $(source)
	@echo "valgrind"
	$(CC) -o $(target) $(source) $(valgrind) $(CFLAGS)

runvg: valgrind
	@echo "spoustim test valgrind"
	valgrind -s --leak-check=full  --show-leak-kinds=all --track-origins=yes --log-file=vg.txt ./rc-panel-info-memory
	cat vg.txt

install: $(target)
	@echo "Instaluji"
	cp -f $(target) /usr/local/bin
	sync
