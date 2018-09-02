set terminal png size 1200,800

set lmargin 11
set tmargin 1
set rmargin 2
set bmargin 4.5

set xlabel "Loss (%)" font "Times-Roman:bold, 16" offset 0,-0.5
set ylabel "Throughput (Kbps)" font "Times-Roman:bold, 16" offset -1

set xrange [1:16]
set yrange [1:1000]
set xtics font "Times-Roman:bold, 15"
set ytics font "Times-Roman:bold, 15"


set key box lw -1
set key spacing 2.5
set key font "Times-Roman,15"
set pointsize 0.8
set key width 0.5 box width -1
set key 


#set style line 1 lt 1 pt 2 lw 2 linecolor rgb "red"
#set style line 2 lt 6 pt 1 lw 2 linecolor rgb "blue"
#set style line 3 lt 3 pt 5 lw 2 linecolor rgb "#006400" #darkgreen
#set style line 4 lt 1 pt 4 lw 2 linecolor rgb "blue" #"magenta"
#set style line 5 lt 5 pt 6 lw 2 linecolor rgb "#9400D3" #darkviolet
#set style line 6 lt 4 pt 8 lw 2 linecolor rgb "brown"
#set style line 7 lt 1 pt 3 lw 2 linecolor rgb "black"



set style line 1 lc rgb '#006400' lt 1 lw 2 pt 7 ps 1.5   # --- darkgreen

set output 'part3_udp4096.png' 
set size 1,1
set key bottom right
set key vertical

plot "part3_udp4096.txt" using 1:2 index 0 with linespoints ls 1 title "Throughput vs Loss for UDP at 4096 Kbps"
# plot "part1.txt" using 1:2 index 0 with errorlines ls 2 title "UDP throughput vs bandwidth"


