CC = arm-linux-gcc
#CC = gcc
CFLAGS = -I/usr/X11R6/include -I../xpm-3.4k/lib
#LFLAGS = -L/usr/X11R6/lib -L../xpl-3.4k/lib
LFLAGS = -L/home/simpad/simpadtools/rd-0.4-2.4.18-walti/usr/X11R6/lib
libs = -lXpm -lX11
xcalibrate: xcalibrate.o
	$(CC) $(LFLAGS) -o $@ $@.o $(libs)
clean:
	rm -f *.o xcalibrate
