all: readannot label2annot

readannot: annotio.o ctab.o readannot.o
	gcc -Wall annotio.o ctab.o readannot.o -o readannot

label2annot: annotio.o ctab.o label2annot.o
	gcc -Wall annotio.o ctab.o label2annot.o -o label2annot

annotio.o: annotio.c
	gcc -Wall -c annotio.c

ctab.o: ctab.c
	gcc -Wall -c ctab.c

label2annot.o: label2annot.c
	gcc -Wall -c label2annot.c

readannot.o: readannot.c
	gcc -Wall -c readannot.c


clean:
	rm -f readannot label2annot
	rm -f *.o