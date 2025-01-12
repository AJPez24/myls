myls: myls.c
	gcc -Wall -pedantic -o myls myls.c


old_myls: old_myls.c
	gcc -Wall -pedantic -o old_myls old_myls.c

.PHONY: clean
clean:
	rm -f myls
	rm -f old_myls