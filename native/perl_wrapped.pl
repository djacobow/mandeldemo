#!/usr/bin/perl -w

use strict;
use warnings qw(all);
use Data::Dumper;
use Inline C =>
    Config =>
        CCFLAGS => "-g -O0",
        INC => '.',
        TYPEMAPS => 'perl_typemaps',
        ENABLE => "AUTOWRAP";

use Inline "C";
Inline->init;


my $params = new_fparams();
my $width = 120;
my $height = 60;
fr_set_x_pels($params,$width);
fr_set_y_pels($params,$height);
fr_show($params);

my $output = fr_calc($params);

my $olines = [];
for (my $j=0;$j<$height;$j++) {
    my $line = '';
    for (my $i=0;$i<$width;$i++) {
        my $v = $output->[$j*$width+$i];
        my $s = $v >= 200 ? '*' : ' ';
        $line .= $s;
    };
    push(@$olines,$line);
};


print(join("\n",@$olines));
print("\n");

1;

__DATA__
__C__

#include "fractizer.h"
#include "fractizer.c"

fparams_t *new_fparams() {
    fparams_t *p = malloc(sizeof(fparams_t));
    if (p) set_default_params(p);
    return p;
}

void free_fparams(fparams_t *p) {
    if (p) free(p);
}

void fr_set_max_iters(fparams_t *p, uint16_t i) { p->max_iters = i; };
void fr_set_escape_val(fparams_t *p, double d)  { p->escape_val = d; };
void fr_set_x_min(fparams_t *p, double d)       { p->x_min = d; };
void fr_set_x_max(fparams_t *p, double d)       { p->x_max = d; };
void fr_set_y_min(fparams_t *p, double d)       { p->y_min = d; };
void fr_set_y_max(fparams_t *p, double d)       { p->y_max = d; };
void fr_set_x_pels(fparams_t *p, uint16_t i)    { p->x_pels = i; };
void fr_set_y_pels(fparams_t *p, uint16_t i)    { p->y_pels = i; };
void fr_set_x_tile(fparams_t *p, uint16_t i)    { p->x_tile = i; };
void fr_set_y_tile(fparams_t *p, uint16_t i)    { p->y_tile = i; };
void fr_set_type(fparams_t *p, uint8_t i)       { p->type = i; };
void fr_set_do_julia(fparams_t *p, uint8_t i)   { p->do_julia = i; };
void fr_set_jx(fparams_t *p, double d)          { p->jx = d; };
void fr_set_jy(fparams_t *p, double d)          { p->jy = d; };

uint16_t *fr_makeOutputBuffer(fparams_t *p) {
    uint16_t *o = malloc(sizeof(uint16_t) * p->x_pels * p->y_pels);
    return o;
}

void fr_freeOutputBuffer(uint16_t *o) {
    if (o) free(o);
};

void fr_show(fparams_t *p) {
    showParams(p);
};

SV *fr_calc(fparams_t *p) {
    size_t len = p->x_pels * p->y_pels;
    uint16_t *buf = malloc(sizeof(uint16_t) * len);
    generate_fractal(p,buf);

    AV* array = newAV();
    for (size_t i=0; i<len; i++) {
        av_push(array, newSVuv(buf[i]));
    };
    free(buf);
    return newRV_noinc((SV*)array);
}

