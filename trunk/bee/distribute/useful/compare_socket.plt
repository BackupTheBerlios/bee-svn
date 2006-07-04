#set terminal png enhanced
#set output 'distribution.png'
set style fill solid 0.5 border -1
set grid nopolar
set grid layerdefault
set   key right top Right enhanced box linetype 1 linewidth 2.000 samplen 4 \
      spacing 3 width 2 height 3 autotitles
#set size 1.5,1.5
set ylabel "%idle"
set xlabel "Time[s]"
set title "CPU System%"
plot 'data' using 2:1 title "Axigen-1.0.25"    with boxes
pause -1
