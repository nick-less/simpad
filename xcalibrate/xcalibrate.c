/*
 * h3600 X calibration Application
 * Adapted from the Itsy calibration app by C Flynn
 * 
 * Copyright 2000 Compaq Computer Corporation.
 *
 * Use consistent with the GNU GPL is permitted,
 * provided that this copyright notice is
 * preserved in its entirety in all copies and derived works.
 *
 * COMPAQ COMPUTER CORPORATION MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
 * AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
 * FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 * Author: Charles Flynn.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/ioctl.h>

#include <assert.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>

#define CALIB_FILE  "/etc/pointercal"
Bool rotated = False;

const int SHIFT = 10;

typedef struct {
  ushort x;
  ushort y;
} TS_POINT;

typedef struct {
  unsigned short pressure;
  unsigned short x;
  unsigned short y;
  unsigned short pad;
  struct timeval stamp;
} TS_EVENT;

typedef struct {
        int xscale;
        int xtrans;
        int yscale;
        int ytrans;
        int xyswap;
} TS_CAL;

/* X globals */
Display *dpy;
Window w;
GC gc;
int backgroundColor, forgroundColor;
Cursor vis_cursor, invis_cursor;
XColor cursorcolor;

#define NIL (0)

static int calFlag = 0;
static int rawFlag = 0;
static int viewFlag = 0;
static int testFlag = 0;
static int setCalFlag = 0;
static int flashCalFlag = 0;
static int newCalFlag = 0;
       int rvFlag = 0;
static int readCalFlag = 0;
static int writeCalFlag = 0;
static TS_CAL new_cal;
TS_CAL raw_cal={256, 0, 256, 0 };
static int rate = 50;

/* Note: ITSY geometry screenW=320, screenH=200; H3600 Width=320 Height=240*/
#define SCREEN_WIDTH		800
#define SCREEN_HEIGHT		600
#define SZ_XHAIR                50      /* Num of pixels across xhair */


/* make 2 cursors, an X crosshair, and an invisible one for when the pen is
  up. */

static char * crosshair[] = {
"16 16 2 1",
"       c None",
".      c #000000",
"                ",
".             . ",
" .           .  ",
"  .         .   ",
"   .       .    ",
"    .     .     ",
"                ",
"                ",
"       .        ",
"                ",
"                ",
"    .     .     ",
"   .       .    ",
"  .         .   ",
" .           .  ",
".             . "};

static char * nothing[] = {
"1 1 1 1",
"       c None",
" "};    
Pixmap crosshair_pixmap, crosshair_mask, nothing_pixmap, nothing_mask;

static void 
make_cursors()
{
  int textpos;
  XpmAttributes Attrib;
  XTextItem testtext[3];

  Attrib.valuemask = 0L;

  XCreatePixmapFromData( dpy, 0, crosshair, &crosshair_pixmap,
			 &crosshair_mask, &Attrib);
  fflush(stderr);
  perror("\n\nin makecursor");
  fflush(stderr);
  XCreatePixmapFromData( dpy, 0, nothing, &nothing_pixmap,
			 &nothing_mask, &Attrib);

  fflush(stderr);
  perror("\nin makecursor");
  fflush(stderr);
  vis_cursor   = XCreatePixmapCursor( dpy, crosshair_pixmap, crosshair_mask, 
				      &cursorcolor, &cursorcolor, 7, 7);
  fflush(stderr);
  perror("in makecursor");
  fflush(stderr);
  invis_cursor = XCreatePixmapCursor( dpy, nothing_pixmap, nothing_mask, 
				      &cursorcolor, &cursorcolor, 0, 0);
  fflush(stderr);
  perror("in makecursor");
  fflush(stderr);
  XDefineCursor( dpy, w, vis_cursor);
  fflush(stderr);
  perror("in makecursor");
  fflush(stderr);
  XDefineCursor( dpy, w, None);
  XFlush(dpy);



  testtext[0].delta = 4;
  testtext[0].font = None;
  textpos = 12;

  testtext[0].chars = "To Calibrate Screen,";
  testtext[0].nchars = strlen(testtext[0].chars);
  XDrawText(dpy, w, gc, textpos, 70, testtext, 1);

  testtext[0].chars = "please poke the crosshairs";
  testtext[0].nchars = strlen(testtext[0].chars);
  XDrawText(dpy, w, gc, textpos, 85, testtext, 1);

  testtext[0].chars = "If the screen is badly mis-calibrated,";
  testtext[0].nchars = strlen(testtext[0].chars);
  XDrawText(dpy, w, gc, textpos, SCREEN_HEIGHT-85, testtext, 1);

  testtext[0].chars = "Press a button while poking crosshairs";
  testtext[0].nchars = strlen(testtext[0].chars);
  XDrawText(dpy, w, gc, textpos, SCREEN_HEIGHT-70, testtext, 1);

  XRecolorCursor(dpy, None, &cursorcolor, &cursorcolor);
};

/* Draw the x-hair to stated coordinates */

static void
drawPlus (int xcoord, int ycoord, int delete)
{
        int hx_coord1,hy_coord1,hx_coord2,hy_coord2;
        int vx_coord1,vy_coord1,vx_coord2,vy_coord2;

	if (rotated) {
	  int temp = xcoord;
	  xcoord = ycoord;
	  ycoord = SCREEN_WIDTH - temp;
	}
        /* work out the horizontal and vertical line coords */

        /* horizontal line */
        hx_coord1= xcoord - (SZ_XHAIR/2);
        hx_coord2= xcoord + (SZ_XHAIR/2);
        hy_coord1=hy_coord2=ycoord;
        /* vertical line */
        vx_coord1=vx_coord2=xcoord;
        vy_coord1= ycoord - (SZ_XHAIR/2);
        vy_coord2= ycoord + (SZ_XHAIR/2);

        if (!delete )
                XSetForeground(dpy,gc,forgroundColor);
        else
                XSetForeground(dpy,gc,backgroundColor);

        /* draw horizontal line */
        XDrawLine (dpy,w,gc,hx_coord1,hy_coord1,hx_coord2,hy_coord2);

        /* draw vertical line */
        XDrawLine (dpy,w,gc,vx_coord1,vy_coord1,vx_coord2,vy_coord2);
        XFlush(dpy);
}

/* Check that the response is in the crosshairs */

static int
nearPlus (int xcoord, int ycoord, TS_EVENT point)
{
  int hx_coord1,hy_coord1,hx_coord2,hy_coord2;
  int vx_coord1,vy_coord1,vx_coord2,vy_coord2;
  int x, y;
  char dummychar;



  /* here I'm using the same code used in drawPlus, since I want those
     points within the smallest box containing the crosshairs. If you
     wish a different definition of what it means to be near the plus,
     change it here. */
  
#if 0
    printf("Old calibration:\n");
    printf("  xscale:%5d, xtrans:%5d\n", raw_cal.xscale,
	   raw_cal.xtrans);
    printf("  yscale:%5d, xtrans:%5d\n", raw_cal.yscale,
	   raw_cal.ytrans);
    printf("xyswap:%s\n", (raw_cal.xyswap == 0 ? "N" : "Y"));
#endif

  if (rotated) {
    int temp = xcoord;
    xcoord = ycoord;
    ycoord = SCREEN_WIDTH - temp;
  }
  /* work out the horizontal and vertical line coords */
  
  /* horizontal borders */
  hx_coord1= xcoord - (SZ_XHAIR/2);
  hx_coord2= xcoord + (SZ_XHAIR/2);

  /* vertical borders */

  vy_coord1= ycoord - (SZ_XHAIR/2);
  vy_coord2= ycoord + (SZ_XHAIR/2);
  
  x = ( ( raw_cal.xscale * point.x ) >> SHIFT ) + raw_cal.xtrans;
  y = ( ( raw_cal.yscale * point.y ) >> SHIFT ) + raw_cal.ytrans;
#if 0
  printf("Should be (%d,%d): point.x=%d point.y=%d\t actually is (%d,%d)\n", 
	 xcoord, ycoord, point.x, point.y, x, y);
  printf("X: %d - %d - %d      Y: %d - %d - %d\n",
	 hx_coord1, x, hx_coord2, vy_coord1, y, vy_coord2);
#endif
  return ((hx_coord1 < x) && (x < hx_coord2) &&
	  (vy_coord1 < y) && (y < vy_coord2));
}

static int 
do_calibration(int fd )
{
  int k, xsum, ysum, cnt, rv;
  TS_EVENT ts_ev;
  TS_POINT screenCalPos[5];
  int x,y;
  unsigned short xraw,yraw;
  int xa[5], ya[5];
  char dummychar;


/* Screen Offsets  for test calibration */
#define OFF1 30
#define OFF2 30

  screenCalPos[0].x = OFF1;
  screenCalPos[0].y = OFF1;
  screenCalPos[1].x = SCREEN_WIDTH - OFF2 - 1;
  screenCalPos[1].y = OFF2;
  screenCalPos[2].x = OFF2;
  screenCalPos[2].y = SCREEN_HEIGHT - OFF2 - 1;
  screenCalPos[3].x = SCREEN_WIDTH - OFF1 - 1;
  screenCalPos[3].y = SCREEN_HEIGHT - OFF1 - 1;
  screenCalPos[4].x = SCREEN_WIDTH / 2;
  screenCalPos[4].y = SCREEN_HEIGHT / 2;

/*
 *   Enter here with ts events pertaining to screen coords screenCalPos[k]
 */

#define MAX_CAL_POINTS	5

  do {
    for (k = 0; k < MAX_CAL_POINTS ; k++) 
      {
	/*	
	 * for each screen position take the average of 5 points.
	 */
	
	printf("\nTouch screen at x=%d y=%d\n",screenCalPos[k].x,screenCalPos[k].y);
	fflush(stdout);
	drawPlus(screenCalPos[k].x, screenCalPos[k].y,0);
	
	cnt = xsum = ysum = 0;
        xa[k] = ya[k] = 0;
	
	/* now loop until we have 5 samples - */
	while (1) {
	  /*	
	   * This read() call will block until the user taps the screen
	   * upon which it will proceed to gather samples until the pen
	   * is lifted. It takes the average of the 'cnt' samples.
	   * It will not accept any less than 5 samples.
	   */
	  if ((rv = read(fd, &ts_ev, sizeof(TS_EVENT))) == -1) {
	    usleep(100);
	    drawPlus(screenCalPos[k].x, screenCalPos[k].y,0);
	    continue;
	  }
	  else if (rv != sizeof(TS_EVENT)) {
	    usleep(100);
	    drawPlus(screenCalPos[k].x, screenCalPos[k].y,0);
	    continue;
	  }
	  
	  // printf("%d, %d, %d\n", ts_ev.x, ts_ev.y, ts_ev.pressure);
	  XUndefineCursor( dpy, w);
	  XDefineCursor( dpy, w, 
			 (ts_ev.pressure == 0)?invis_cursor:vis_cursor);
	    
	  /*
	    Dont exit the while loop until we have 5 events
	    AND the PEN is UP denoted by pressure=0
	  */
	  
	  if (ts_ev.pressure < 200  &&  cnt > 20)
	    {
	      break;
	    }
	  else
	    {
	      /* accumulate the x coords */
	      xsum += ts_ev.x;
	      ysum += ts_ev.y;
	      /* increment the event counter */
	      cnt++;
	    }
	} /* endwhile */
	
	/* delete the plus */
	drawPlus(screenCalPos[k].x, screenCalPos[k].y,1);

	/*
	 *  Enter here with - agregate of the x,y coords stored in xsum & ysum.
	 */
	
	
	/* take the average of the x & y points */
	xa[k] = xsum/cnt;
	ya[k] = ysum/cnt;
	printf(" k=%d  AveX=%3d  AveY=%3d  (cnt=%3d)\n", k, xa[k], ya[k], cnt);
      }
    
    /* Enter here with the average x,y coords of the MAX_CAL_POINTS */
    
    /* get calibration parameters */
    {
      int *xp, *yp, x, y, dx, dy, flag;
      int xscale0, xscale1, xtrans0, xtrans1;
      int yscale0, yscale1, ytrans0, ytrans1;
      int xyswap;
      
      flag = 0;
      
      /* Calculate ABS(x1-x0) */
      dx = ((x = xa[1] - xa[0]) < 0 ? -x : x);
      dy = ((y = ya[1] - ya[0]) < 0 ? -y : y);
      
      /* CF

	(0,0) --------->Y
X	+-------+---------------+-
|	|	|		|
V	|	|xa0,ya0	|
	+-------O---------------+ --
	|	|		| dx or dy
	+-------+---------------O --
^	|	|		| xa1,ya1
|	|	|		|
Y	+-------+---------------+
	(0,0) -------->X

	Work out where the origin is, either at the TLH or BLH corners.
        Initialise xp such that it points to the array containing the X coords
        Initialise yp such that it points to the array containing the Y coords
      */
      if (dx < dy) {
	xyswap = 1;
	xp = ya, yp = xa;
      }
      else {
	xyswap = 0;
	xp = xa;
	yp = ya;
      }
      
      xraw=xp[4]; yraw=yp[4];	/* used for check later */

      /*

	We have MAX_CAL_POINTS sets of x,y coordinates.

	If we plot Xcal against Xraw we get two equations, each of a straight
		line. One for the Xcoord and the other for the Y coord.
		This line models the linear characteristics of the ts A/D
		converter.

	Xcal = m*Xraw + Cx
	Ycal = m*Yraw + Cy

	X/Ycal is the calibrated coord which is the pixel pos on the screen.
	X/Yraw is the uncalibrated X coord.
	m is the xscale ( gradient of line)
	Cx/y is the trans (constant)
	
	xscale
	  'm' can be got by calculating the gradient between two data points
	  Example Xscale0 = (Xcal1 - Xcal0 ) / (Xraw1 - Xraw0)

	trans = Xcal - mXraw
	  What is actualy done is to take the Ave of two measurements
	  Example  Xtrans0 = ( (Xcal0 - mXraw0) + (Xcal3 - mXraw3) ) / 2

	We repeat the above procedure to calculate 
	Yscale0 and Ytrans0 and repeat the whole lot again using two
	new data indexes 1 and 2 giving 4 new variables
	Xscale1, Xtrans1, Yscale1,Ytrans1, making a total of eight.

	The final calibration variables are the average of data ponts 
		0,3 and 1,2
	xscale = (Xscale0 + Xscale1) / 2
	yscale = (Yscale0 + Yscale1) / 2
	xtrans = (Xtrans0 + Xtrans1) /2
	ytrans = (Ytrans0 + Ytrans1) /2
	
      */
      xscale0 = ( (screenCalPos[0].x - screenCalPos[3].x) << SHIFT ) / 
	(((xp[0] - xp[3])));
      
      printf("Xc0=%d Xc3=%d Xr0=%d Xr3=%d xscale0=%d\n",
	     screenCalPos[0].x, screenCalPos[3].x, xp[0],xp[3],xscale0 );
      
      xtrans0 = ( (screenCalPos[0].x - ((xp[0]*xscale0) >> SHIFT)) + 
		  (screenCalPos[3].x - ((xp[3]*xscale0) >> SHIFT)) ) / 2;
      yscale0 = ( (screenCalPos[0].y - screenCalPos[3].y) << SHIFT ) / 
	(((yp[0] - yp[3])));
      ytrans0 = ( (screenCalPos[0].y - ((yp[0]*yscale0) >> SHIFT)) + 
		  (screenCalPos[3].y - ((yp[3]*yscale0) >> SHIFT)) ) / 2;
      
      xscale1 = ( (screenCalPos[1].x - screenCalPos[2].x) << SHIFT ) / 
	(((xp[1] - xp[2])));
      
      printf("Xc1=%d Xc2=%d Xr1=%d Xr2=%d xscale1=%d\n",
	     screenCalPos[1].x, screenCalPos[2].x, xp[1],xp[2],xscale1 );
      
      xtrans1 = ( (screenCalPos[1].x - ((xp[1]*xscale1) >> SHIFT)) +
		  (screenCalPos[2].x - ((xp[2]*xscale1) >> SHIFT)) ) / 2;
      yscale1 = ( (screenCalPos[1].y - screenCalPos[2].y) << SHIFT ) / 
	(((yp[1] - yp[2])));
      ytrans1 = ( (screenCalPos[1].y - ((yp[1]*yscale1) >> SHIFT)) 
		  +(screenCalPos[2].y - ((yp[2]*yscale1) >> SHIFT)) ) / 2;
      
      printf("xs0:%d, xs1:%d, xt0:%d, xt1:%d\n", xscale0, xscale1,
	     xtrans0, xtrans1);
      printf("ys0:%d, ys1:%d, yt0:%d, yt1:%d\n", yscale0, yscale1,
	     ytrans0, ytrans1);
      new_cal.xscale = (xscale0 + xscale1) / 2;
      printf("AveXscale=%d\n",new_cal.xscale);
      new_cal.xtrans = (xtrans0 + xtrans1) / 2;
      new_cal.yscale = (yscale0 + yscale1) / 2;
      new_cal.ytrans = (ytrans0 + ytrans1) / 2;
      new_cal.xyswap = xyswap;
    }
    
    printf("New calibration:\n");
    printf("  xscale:%5d, xtrans:%5d\n", new_cal.xscale,
	   new_cal.xtrans);
    printf("  yscale:%5d, xtrans:%5d\n", new_cal.yscale,
	   new_cal.ytrans);
    printf("xyswap:%s\n", (new_cal.xyswap == 0 ? "N" : "Y"));
    
    /* Now check it with center coords*/
    printf("CHECK with Center Coords (%d,%d): xraw=%d yraw=%d\n", 
	   SCREEN_WIDTH/2, SCREEN_HEIGHT/2, xraw,yraw);
    x = ( ( new_cal.xscale * xraw ) >> SHIFT ) + new_cal.xtrans;
    y = ( ( new_cal.yscale * yraw ) >> SHIFT ) + new_cal.ytrans;
    printf("CHECK: x(center)=%d y(center)=%d\n",x,y);
    x -= SCREEN_WIDTH/2;
    y -= SCREEN_HEIGHT/2;
    printf("off(%d,%d) Badness = %d\n", x, y, x*x+y*y);
  } while ((50<(x*x+y*y)));
  
  return 0;
}

int initX()
{
   XSetWindowAttributes attributes;

   /* call Xlib directly to init our display */
   dpy = XOpenDisplay(NULL);

   assert(dpy);

   if (DisplayWidth(dpy, 0) < DisplayHeight(dpy, 0))
     rotated = True;

   if (rvFlag) {
     forgroundColor  = BlackPixel(dpy,DefaultScreen(dpy) );
     backgroundColor = WhitePixel(dpy,DefaultScreen(dpy) );
     cursorcolor.red = 65535; 
     cursorcolor.blue = 65535; 
     cursorcolor.green = 65535; 
     cursorcolor.green = -1; 
   } else {
     backgroundColor = BlackPixel(dpy,DefaultScreen(dpy) );
     forgroundColor  = WhitePixel(dpy,DefaultScreen(dpy) );
     cursorcolor.red = 0;
     cursorcolor.blue = 0;
     cursorcolor.green = 0; 
     cursorcolor.green = -1; 
   }
   /* make this be a root window */
   attributes.override_redirect = True;
   attributes.background_pixel = backgroundColor;

   w = XCreateWindow(dpy, DefaultRootWindow(dpy), 0 , 0,
                     DisplayWidth(dpy, 0), DisplayHeight(dpy, 0),
                     0,
                     DefaultDepth(dpy, 0),
                     InputOutput,
                     DefaultVisual(dpy, 0),
                     CWOverrideRedirect|CWBackPixel,
                     &attributes );
   XSelectInput(dpy,w, StructureNotifyMask);
   XMapWindow(dpy,w);
   XGrabPointer(dpy,w,
                /* owner events */ True,
                /* event mask */Button1Mask,
                /* pointer mode */GrabModeAsync,
                /* keyboard mode */GrabModeAsync,
                /* confine to */ 0,
                /* cursor */ None,
                CurrentTime);

   gc = XCreateGC(dpy,w,0,NIL);
   XSetForeground(dpy,gc,forgroundColor);
   for(;;)
      {
         XEvent e;
         /* Read next event */
         XNextEvent (dpy, &e);   /* this is the blocking version */
         if( e.type == MapNotify)
            break;
      }

   return (dpy) ? 1 : -1;
}

/* Read calibration data from File */
void readCalibData()
{
    FILE * calFd;

    if ((calFd = fopen(CALIB_FILE, "r")) < 0) 
    {
	printf("ERROR: unable to open %s\n",CALIB_FILE);
	exit(1);
    }
    
    fscanf(calFd, "%d %d %d %d %d\n", new_cal.xscale, new_cal.xtrans,
	    new_cal.yscale, new_cal.ytrans, new_cal.xyswap);

    (void) fclose(calFd);
}

/* Save calibration data to File */
void writeCalibData()
{
    FILE * calFd;

    if ((calFd = fopen(CALIB_FILE, "w+")) < 0) 
    {
	printf("ERROR: unable to open %s\n",CALIB_FILE);
	exit(1);
    }
    
    fprintf(calFd, "%d %d %d %d %d\n", new_cal.xscale, new_cal.xtrans,
	    new_cal.yscale, new_cal.ytrans, new_cal.xyswap);

    (void) fclose(calFd);
}

int main(int argc, char **argv)
{
    int k, fd, cfd, err;
    char dummychar;
    static char * DEV_NODE = "/tmp/tsraw";
    static char * CAL = "/tmp/tscal"; 
    //FILE * cfd;

    for (k=1; k<argc; k++) {
      if (!strcmp(argv[k], "-raw"))
        rawFlag = 1, calFlag = 0;
      else if (!strcmp(argv[k], "-view"))
        viewFlag = 1, calFlag = 0;
      else if (!strcmp(argv[k], "-test"))
        testFlag = 1, calFlag = 0;
      else if (!strcmp(argv[k], "-rv"))
        rvFlag = 1;
      else if (!strcmp(argv[k], "-cal")) {
        sscanf(argv[++k], "%d", &new_cal.xscale);
        sscanf(argv[++k], "%d", &new_cal.xtrans);
        sscanf(argv[++k], "%d", &new_cal.yscale);
        sscanf(argv[++k], "%d", &new_cal.ytrans);
        sscanf(argv[++k], "%d", &new_cal.xyswap);
        setCalFlag = 1, calFlag = 0;
      }
      else if ((!strcmp(argv[k], "-w")) || (!strcmp(argv[k], "-write"))) {
	if(readCalFlag == 1){
	  printf("ERROR: You can't set read and wirte flag at once!");
	  exit(1);
	}
	else
	  writeCalFlag = 1;
      }
      else if ((!strcmp(argv[k], "-r")) || (!strcmp(argv[k], "-read"))) {
	if(writeCalFlag == 1){
	  printf("ERROR: You can't set read and wirte flag at once!");
	  exit(1);
	}
	else
	  readCalFlag = 1;
      }
      else if (!strncmp(argv[k], "-h", 2)) {
        printf("USAGE: xcal <optional arguments>\n");
        printf("  optional args:\n");
        printf("    -view            view ts calibrated data\n");
        printf("    -test            show + where screen is pressed\n");
        printf("    -cal <xs> <xt> <ys> <yt> <xyswap>\n");
        printf("                     set new session calibration\n");
        printf("    -rv              reverse video\n");
	printf("    -r -read         read calib data from file\n");
	printf("    -w -write        write calib data to file\n");
	
        exit(0);
      }
    }

    if( initX() < 0 )
    {
	printf("Unable to Xinit\n");
	exit(1);
    }

    /* Flush data from fifo */
    if ((fd = open(DEV_NODE, O_RDONLY|O_NONBLOCK)) < 0) 
    {
	printf("ERROR: unable to open %s\n",DEV_NODE);
	exit(1);
    }
    while((k = read(fd, &dummychar, 1)) >= 0);
    (void) close(fd);


    if ((fd = open(DEV_NODE, O_RDONLY)) < 0) 
    {
	printf("ERROR: unable to open %s\n",DEV_NODE);
	exit(1);
    }

    if( readCalFlag == 1)
    {
        readCalibData();
    }
    else
    {
        make_cursors();
	do_calibration (fd);
    }
    
    printf("Writing result\n");
    if ((cfd = open(CAL, O_WRONLY)) < 0) 
    {
	printf("ERROR: unable to open %s\n",CAL);
	exit(1);
    }
    k = write(cfd, (char*)&new_cal, sizeof(new_cal));

    (void) close(cfd);

    if( writeCalFlag == 1 )
        writeCalibData();

    /* close session devices */
    (void) close(fd);

    return 0;
}
