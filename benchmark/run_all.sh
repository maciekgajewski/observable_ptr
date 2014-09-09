#!/bin/bash

thisfile=`readlink -f $0`
thisdir=`dirname $thisfile`

MAKE_PLOT=$thisdir/make_plot.sh

$MAKE_PLOT 'create.gnuplot' 'create_shared create_observable'
$MAKE_PLOT 'read.gnuplot' 'read_shared read_observable read_weak read_observer'
