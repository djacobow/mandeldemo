#!/usr/bin/perl -w
use lib '.';

use fractizer;
use Data::Dumper;

my $params = fractizer::fparams_t->new();

fractizer::set_default_params($params);
my $width  = 200;
my $height = 100;

$params->swig_x_pels_set($width);
$params->swig_y_pels_set($height);

fractizer::showParams($params);

my $obuf = fractizer::make_obuf($params->swig_x_pels_get() * $params->swig_y_pels_get());

fractizer::generate_fractal($params,$obuf);

my $output = fractizer::bufToArray($obuf,$params->swig_x_pels_get() * $params->swig_y_pels_get());

fractizer::free_obuf($obuf);

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

