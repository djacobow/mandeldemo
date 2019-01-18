 swig -perl fractizer.i   
 gcc -c `perl -MConfig -e 'print join(" ", @Config{qw(ccflags optimize cccdlflags)}, "-I$Config{archlib}/CORE")'` fractizer.c fractizer_wrap.c
 gcc `perl -MConfig -e 'print $Config{lddlflags}'` fractizer.o fractizer_wrap.o -o fractizer.so
