import Gnuplot
import sys
def plotting(band,delay,filename,flag,no):
	h = Gnuplot.Gnuplot()
	h('set terminal png size 1200,800 font ",16"')
	h('set key off')
	if(flag==1):
		h.title('UDP throughtput vs Bandwidth with S1-S2 bandwidth = '+band+' Mbps,delay = '+delay)
	else:
		h.title('UDP throughtput vs Bandwidth with S1-S2 loss rate =' + delay)
	h.xlabel('Bandwidth(in Kbps)')
	h.ylabel('Throughput(in Kbps)')
	h('set term png')
	h('set out "output'+no+'.png"')
	h('set xrange [0:4200]')
	h('set yrange [0:1100]')
	h('set xtics 0,100,4100')
	h('set ytics 0,50,2400')
	h('set xtic rotate by -45 scale 1 font ",10"')
	databuff = Gnuplot.File(filename,with_='linespoints')
	h.plot(databuff)

if __name__ == '__main__':
	if(len(sys.argv)==4):
		plotting(sys.argv[1],sys.argv[1],sys.argv[2],0,sys.argv[3])
	else:
		plotting(sys.argv[1],sys.argv[2],sys.argv[3],1,sys.argv[4])
