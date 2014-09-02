#!/bin/bash -e

# run in build directory

SIZES='32 64 128 256 512 1024 2048'
#TESTS='create_shared create_observable'
#GNUPLOTOUT=plot.gnuplot
GNUPLOTOUT=$1
TESTS=$2
REPETITIONS=10

echo "Generating $GNUPLOTOUT..."
rm -f $GNUPLOTOUT

echo "set logscale x" >> $GNUPLOTOUT
echo "set xlabel 'number of pointers'" >> $GNUPLOTOUT
echo "set ylabel 'time'" >> $GNUPLOTOUT
echo "set format y ''" >> $GNUPLOTOUT
echo "set xtics 16,2,2048" >> $GNUPLOTOUT

echo "plot\\" >> $GNUPLOTOUT

for test in $TESTS
do
	OUTFILE=$test.dat
	rm -f $OUTFILE
	echo "Creating $OUTFILE..."
	for size in $SIZES
	do
		for ((r = 0; r < $REPETITIONS; r++))
		do
			./benchmark/benchmark $test $size >> $OUTFILE
		done
	done
	echo "	'$OUTFILE' using 1:3 title '$test' smooth unique,\\" >> $GNUPLOTOUT
done
echo "Done"

