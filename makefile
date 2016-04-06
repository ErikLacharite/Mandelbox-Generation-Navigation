FLAGS=-acc -fast -ta=tesla,cc35 -Minfo=accel -Minform=inform -O3
CFLAGS  = $(FLAGS)
CXXFLAGS= $(FLAGS)
CC=pgCC
CXX=pgCC


PROGRAM_NAME=mandelbox

$(PROGRAM_NAME): main.o print.o timing.o savebmp.o getparams.o 3d.o getcolor.o distance_est.o \
	mandelboxde.o raymarching.o renderer.o init3D.o
	make clean
	$(CC) -o $@ $? $(CFLAGS) $(LDFLAGS)

omp:
	make clean
	make -f makefile_omp
ompserv:
	make clean
	make -f makefile_ompserv
ompservmpi:
	make clean
	make -f makefile_ompservmpi
serial:
	make clean
	make -f makefile_serial



test:
	./mandelbox params2.dat
testmpi:
	# --map-by socket:PE=1:none use for openACC
	mpirun --map-by socket:PE=4:none -hostfile host_file ./mandelbox para


bench:
	python -m timeit -n 3 -r 1 "__import__('os').system('./mandelbox params2.dat')"
benchmpi:
	python -m timeit -n 3 -r 1 "__import__('os').system('mpirun --map-by socket:PE=4 -hostfile host_file ./mandelbox para')"


video:
	# make test
	ffmpeg -r 20 -i images/image%010d.bmp  -c:v libx264 -preset slow -tune animation -crf 18 -c:a copy images/output.mp4
	open images/output.mp4


clean:
	rm -f *.o $(PROGRAM_NAME) $(EXEEXT) *~
