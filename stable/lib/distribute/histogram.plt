set boxwidth 0.1 relative
set style fill pattern border
set yrange[0:70]
set xrange[0.5:10]
plot 'hist.dat' using 1:2 w boxes
pause -1
