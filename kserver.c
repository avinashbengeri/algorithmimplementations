// Submitter name: AVINASH BENGERI
/* Program to find optimal strategy to solve k-server problem using dynamic programming */
/* Compiles with command line  gcc avior.c -lX11 -lm -L/usr/X11R6/lib -o avior */
/* Runs with command line  ./avior.exe input.txt */
/********************************************************************************/
/*																				*/
/*		------------------- Instructions -------------------            		*/
/*																				*/
/*		1. Use Left Mouse Click on the screen, inside the boundary box  		*/
/*		to indicate server requests. 											*/
/*		2. The requests of left mouse clicks are served by an online strategy	*/
/*		- nearest request served. So the Yellow, Red & Blue servers serve 		*/
/*		requests accordingly.													*/
/*		done drawing triangles(obstacles).										*/
/*		3. If done with server requests, click right mouse click.				*/
/*		4. Right mouse click indicates end of server requests and computes		*/
/*		optimal strategy for minimal cost for servers. The offline strategy is	*/
/*		to find the optimal solution with minimum cost by dynamic programming.	*/
/*		5. Finally the optimal solution to serve requests is highlighted by		*/
/*		thick paths, and the statistics of both strategies displayed.			*/
/*		Note: To repeat the execution, close window and restart the program.	*/
/*																				*/
/********************************************************************************/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;
static Point yellow, red, blue;
Point seq[100];
Point servers[100];
static int cnt = 0;
static int yellowDist = 0, redDist = 0, blueDist = 0;
static int onlineDist = 0, offlineDist = 0;
double compRatio = 0.0;
int currCost[50][50][50][50], cost[50][50][50][50];
Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;
Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;
XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Example Window";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey, gc_blue, gc_yellow_thick, gc_blue_thick, gc_red_thick;
Drawable dr;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values, gc_blue_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

int find_distance(Point a, Point b) {
    return (int) sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
}

int min(int a, int b, int c) {
    return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

void resetServers() {
    yellow.x = 39;
    yellow.y = 29;
    red.x = 659;
    red.y = 29;
    blue.x = 334;
    blue.y = 619;
}

void serveRequestOnline(Point p) {
    cnt++;
    if (find_distance(yellow, p) < find_distance(red, p)) {
        if (find_distance(yellow, p) < find_distance(blue, p)) {
            XDrawLine(display_ptr, win, gc_yellow, p.x, p.y, yellow.x, yellow.y);
            yellowDist += find_distance(yellow, p);
            yellow=p;
        } else {
            XDrawLine(display_ptr, win, gc_blue, p.x, p.y, blue.x, blue.y);
            blueDist += find_distance(blue, p);
            blue=p;
        }
    } else if (find_distance(red, p) < find_distance(blue, p)) {
        XDrawLine(display_ptr, win, gc_red, p.x, p.y, red.x, red.y);
        redDist += find_distance(red, p);
        red=p;
    } else {
        XDrawLine(display_ptr, win, gc_blue, p.x, p.y, blue.x, blue.y);
        blueDist += find_distance(blue, p);
        blue=p;
    }
}

void serveRequestOffline(Point* sequenceOfRequests) {
    int t, i, j, k;
	int m;
	int sa, sb, sc, sx, sy, sz;
    servers[0] = yellow;
    servers[1] = red;
    servers[2] = blue;
    for (i = 0, j = 3; i < cnt; i++, j++) {
        servers[j] = sequenceOfRequests[i];
    }
    cnt = cnt + 3;
    for (t = 0; t < cnt; t++) {
        for (i = 0; i < cnt; i++) {
            for (j = 0; j < cnt; j++) {
                for (k = 0; k < cnt; k++) {
                    currCost[t][i][j][k] = 999999; //infinity
                }
            }
        }
    }
    currCost[2][0][1][2] = 0;
    for (t = 3; t < cnt; t++) {
        for (i = 0; i < t + 1; i++) {
            for (j = 1; j < t + 1; j++) {
                for (k = 2; k < t + 1; k++) {
                    if (t != i && t != j && t != k)
                        currCost[t][i][j][k] = 999999; //infinity
                    else {
                        for (m = 0; m < t; m++) {
                            sa = currCost[t - 1][m][j][k] + find_distance(servers[i], servers[m]);
                            if (currCost[t][i][j][k] > sa) {
                                currCost[t][i][j][k] = sa;
                                sx = m;
                            }
                        }
                        sa = currCost[t][i][j][k];
                        for (m = 0; m < t; m++) {
                            sb = currCost[t - 1][i][m][k] + find_distance(servers[j], servers[m]);
                            if (currCost[t][i][j][k] > sb) {
                                currCost[t][i][j][k] = sb;
                                sy = m;
                            }
                        }
                        sb = currCost[t][i][j][k];
                        for (m = 0; m < t; m++) {
                            sc = currCost[t - 1][i][j][m] + find_distance(servers[k], servers[m]);
                            if (currCost[t][i][j][k] > sc) {
                                currCost[t][i][j][k] = sc;
                                sz = m;
                            }
                        }
                        sc = currCost[t][i][j][k];
                        currCost[t][i][j][k] = min(sa, sb, sc);
                        if (currCost[t][i][j][k] == sa)
                            cost[t][i][j][k] = sx;
                        else if (currCost[t][i][j][k] == sb)
                            cost[t][i][j][k] = sy;
                        else if (currCost[t][i][j][k] == sc)
                            cost[t][i][j][k] = sz;
                    }
                }
            }
        }
    }
    offlineDist = 999999; //infinity
    t = cnt;
    for (i = 0; i < t; i++) {
        for (j = 1; j < t; j++) {
            for (k = 2; k < t; k++) {
                if (currCost[t - 1][i][j][k] < offlineDist) {
                    offlineDist = currCost[t - 1][i][j][k];
                    sa = i;
                    sb = j;
                    sc = k;
                }
            }
        }
    }
    t = cnt;
    while (t >= 3) {
        if (sa == t) {
            XDrawLine(display_ptr, win, gc_yellow_thick, servers[t].x, servers[t].y, servers[cost[t][sa][sb][sc]].x, servers[cost[t][sa][sb][sc]].y);
            yellowDist += find_distance(servers[t],servers[cost[t][sa][sb][sc]]);
			sa = cost[t][sa][sb][sc];
        } else if (sb == t) {
            XDrawLine(display_ptr, win, gc_red_thick, servers[t].x, servers[t].y, servers[cost[t][sa][sb][sc]].x, servers[cost[t][sa][sb][sc]].y);
            redDist += find_distance(servers[t],servers[cost[t][sa][sb][sc]]);
			sb = cost[t][sa][sb][sc];
        } else if (sc == t) {
            XDrawLine(display_ptr, win, gc_blue_thick, servers[t].x, servers[t].y, servers[cost[t][sa][sb][sc]].x, servers[cost[t][sa][sb][sc]].y);
            blueDist += find_distance(servers[t],servers[cost[t][sa][sb][sc]]);
			sc = cost[t][sa][sb][sc];
        }
        t--;
    }
    offlineDist = yellowDist + redDist + blueDist;
}

int main(int argc, char **argv) {
    resetServers();
    if ((display_ptr = XOpenDisplay(display_name)) == NULL) {
        printf("Could not open display. \n");
    }
    printf("Connected to X server  %s\n", XDisplayName(display_name));
    screen_num = DefaultScreen(display_ptr);
    screen_ptr = DefaultScreenOfDisplay(display_ptr);
    color_map = XDefaultColormap(display_ptr, screen_num);
    display_width = DisplayWidth(display_ptr, screen_num);
    display_height = DisplayHeight(display_ptr, screen_num);
    printf("Width %d, Height %d, Screen Number %d\n",
            display_width, display_height, screen_num);
    border_width = 10;
    win_x = 0;
    win_y = 0;
    win_width = 700;
    win_height = 650;
    win = XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num),
            win_x, win_y, win_width, win_height, border_width,
            BlackPixel(display_ptr, screen_num),
            WhitePixel(display_ptr, screen_num));
    size_hints = XAllocSizeHints();
    wm_hints = XAllocWMHints();
    class_hints = XAllocClassHint();
    if (size_hints == NULL || wm_hints == NULL || class_hints == NULL) {
        printf("Error allocating memory for hints. \n");
    }

    size_hints -> flags = PPosition | PSize | PMinSize;
    size_hints -> min_width = 60;
    size_hints -> min_height = 60;

    XStringListToTextProperty(&win_name_string, 1, &win_name);
    XStringListToTextProperty(&icon_name_string, 1, &icon_name);

    wm_hints -> flags = StateHint | InputHint;
    wm_hints -> initial_state = NormalState;
    wm_hints -> input = False;

    class_hints -> res_name = "x_use_example";
    class_hints -> res_class = "examples";

    XSetWMProperties(display_ptr, win, &win_name, &icon_name, argv, argc,
            size_hints, wm_hints, class_hints);

    XSelectInput(display_ptr, win,
            ExposureMask | StructureNotifyMask | ButtonPressMask);

    XMapWindow(display_ptr, win);

    XFlush(display_ptr);

    gc = XCreateGC(display_ptr, win, valuemask, &gc_values);
    XSetForeground(display_ptr, gc, BlackPixel(display_ptr, screen_num));
    XSetLineAttributes(display_ptr, gc, 6, LineSolid, CapRound, JoinRound);
    gc_yellow = XCreateGC(display_ptr, win, valuemask, &gc_yellow_values);
    XSetLineAttributes(display_ptr, gc_yellow, 1, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "yellow",
            &tmp_color1, &tmp_color2) == 0) {
        printf("failed to get color yellow\n");
    }
    else
        XSetForeground(display_ptr, gc_yellow, tmp_color1.pixel);
    gc_yellow_thick = XCreateGC(display_ptr, win, valuemask, &gc_yellow_values);
    XSetLineAttributes(display_ptr, gc_yellow_thick, 5, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "yellow",
            &tmp_color1, &tmp_color2) == 0) {
        printf("failed to get color yellow\n");
    }
    else
        XSetForeground(display_ptr, gc_yellow_thick, tmp_color1.pixel);

    gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
    XSetLineAttributes(display_ptr, gc_red, 1, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "red",
            &tmp_color1, &tmp_color2) == 0) {
        printf("failed to get color red\n");
    }
    else
        XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
    gc_red_thick = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
    XSetLineAttributes(display_ptr, gc_red_thick, 5, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "red",
            &tmp_color1, &tmp_color2) == 0) {
        printf("failed to get color red\n");
    }
    else
        XSetForeground(display_ptr, gc_red_thick, tmp_color1.pixel);

    gc_blue = XCreateGC(display_ptr, win, valuemask, &gc_blue_values);
    XSetLineAttributes(display_ptr, gc_blue, 1, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "blue",
            &tmp_color1, &tmp_color2) == 0) {
        printf("failed to get color blue\n");
    }
    else
        XSetForeground(display_ptr, gc_blue, tmp_color1.pixel);
    gc_blue_thick = XCreateGC(display_ptr, win, valuemask, &gc_blue_values);
    XSetLineAttributes(display_ptr, gc_blue_thick, 5, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "blue",
            &tmp_color1, &tmp_color2) == 0) {
        printf("failed to get color blue\n");
    }
    else
        XSetForeground(display_ptr, gc_blue_thick, tmp_color1.pixel);
    gc_grey = XCreateGC(display_ptr, win, valuemask, &gc_grey_values);
    if (XAllocNamedColor(display_ptr, color_map, "light grey",
            &tmp_color1, &tmp_color2) == 0) {
        printf("failed to get color grey\n");
    }
    else
        XSetForeground(display_ptr, gc_grey, tmp_color1.pixel);

    while (1) {
        XNextEvent(display_ptr, &report);
        switch (report.type) {
            case Expose:
                XDrawLine(display_ptr, win, gc, 30, 20, 670, 20);
                XDrawLine(display_ptr, win, gc, 30, 20, 30, 630);
                XDrawLine(display_ptr, win, gc, 30, 630, 670, 630);
                XDrawLine(display_ptr, win, gc, 670, 20, 670, 630);
                XFillArc(display_ptr, win, gc_yellow,
                        35, 25,
                        10, 10, 0, 360 * 64);
                XFillArc(display_ptr, win, gc_red,
                        655, 25,
                        10, 10, 0, 360 * 64);
                XFillArc(display_ptr, win, gc_blue,
                        330, 615,
                        10, 10, 0, 360 * 64);
                break;
            case ConfigureNotify:
                win_width = report.xconfigure.width;
                win_height = report.xconfigure.height;
                break;
            case ButtonPress:
            {
                int x, y, i;
                x = report.xbutton.x;
                y = report.xbutton.y;
                if (report.xbutton.button == Button1) {
                    if (x <= 32 || x >= 668 || y <= 22 || y >= 628) {
                        printf("Click inside the boundary..!!\n");
                        break;
                    } else {
                        seq[cnt].x = x;
                        seq[cnt].y = y;
                        printf("Request %d at (%d,%d)\n", cnt + 1, x, y);
                        serveRequestOnline(seq[cnt]);
                    }
                } else {
				    printf("\n----------- Cost of Online Strategy ----------------------\n");
                    printf("|\tDistance travelled by Yellow - %d\n", yellowDist);
                    printf("|\tDistance travelled by Red - %d\n", redDist);
                    printf("|\tDistance travelled by Blue - %d\n", blueDist);
                    onlineDist = yellowDist + redDist + blueDist;
                    printf("|\tTotal cost =  %d\n", onlineDist);
					printf("----------------------------------------------------------\n");
                    yellowDist = 0;
                    redDist = 0;
                    blueDist = 0;
					resetServers();
                    serveRequestOffline(seq);
                    printf("\n----------- Cost of Offline Strategy ---------------------\n");
                    printf("|\tDistance travelled by Yellow - %d\n", yellowDist);
                    printf("|\tDistance travelled by Red - %d\n", redDist);
                    printf("|\tDistance travelled by Blue - %d\n", blueDist);
                    printf("|\tTotal cost =  %d\n", offlineDist);
					printf("----------------------------------------------------------\n");
                    compRatio = (double)onlineDist / offlineDist;
					printf("----------------------------------------------------------\n");
                    printf("|\tCompetency ratio is %.2f:1                       |\n", compRatio);
					printf("----------------------------------------------------------\n");
                }
            }
                break;
            default:
                break;
        }

    }

}