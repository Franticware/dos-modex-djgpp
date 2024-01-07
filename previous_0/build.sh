#!/bin/bash

STRS=s
rm -f *.o *.s
i586-pc-msdosdjgpp-gcc -O2 -c main.c

for i in 0 1 2 3
do
rm -f modex$i.exe modex$i$STRS.exe
i586-pc-msdosdjgpp-gcc -O2 -c framebuf$i.c
i586-pc-msdosdjgpp-gcc -O2 -S -c framebuf$i.c
i586-pc-msdosdjgpp-as framebuf$i.s -o framebuf$i$STRS.o
sha1sum framebuf$i.o framebuf$i$STRS.o
i586-pc-msdosdjgpp-gcc main.o framebuf$i.o -o modex$i.exe
done
