// Submitter name: AVINASH BENGERI
/* Program to find shortest path with obstacles */
/* Compiles with command line  gcc avior.c -lX11 -lm -L/usr/X11R6/lib -o avior */
/* Runs with command line  ./avior.exe input.txt */
/********************************************************************************/
/*																				*/									
/*		------------------- Instructions -------------------            		*/
/*																				*/									
/*		1. Use Left Mouse Click on the screen at three different points to 		*/
/*		draw triangles(obstacles). 												*/
/*		2. If an input filecontaining triangle vertices is passed as 			*/									
/*		argument, then those triangles are already drawn on screen.				*/									
/*		3. The traingles are drawn in BLUE. Click Right Mouse click once		*/									
/*		done drawing triangles(obstacles).										*/									
/*		4. Now use left mouse click to select start point and target point.		*/									
/*		5. The program computes all possible paths between start point and		*/									
/*		target point. The shortest path is represented in GREEN. The program	*/									
/*		computes the shortest path considering the all the obstacles.			*/									
/*																				*/									
/********************************************************************************/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define MAX_VERTICES 2997
#define MAX_TRIANGLES 999
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )

typedef struct{
	int x;
	int y;
}Point;
typedef struct{
	Point v1;
	Point v2;
	Point v3;
}Triangle;
typedef enum { false, true } bool;

/* Defining color properties */
char white[] = "#FFFFFF";
char green[] = "#00FF00";
char red[] = "#FF0000";
char black[] = "#000000";
char blue[] = "#0000FF";
GC gc_red, gc_green, gc_blue, gc_white, gc_black;
Colormap colormap;
XColor color_red, color_green, color_blue, color_black, color_white;

/* Definig variables for computations */
static int pointCount=0;
static int clickcount=1;
static int close_flag=0;
Triangle triangles[MAX_TRIANGLES];
Point point[MAX_TRIANGLES];
int obs_count;
int click_count=1;
int num_point;
int graph[MAX_VERTICES][MAX_VERTICES];
int parent[MAX_VERTICES];
Point poi[MAX_VERTICES][MAX_VERTICES][2];
int vertex_count;
Point result_vertices[MAX_VERTICES][2];
int distance[MAX_VERTICES];
bool pathExist;

/* Defining Window properties */
Display *display_ptr;
Screen *screen;
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
char *win_name_string = "Shortest path computation with Obstacles";
char *icon_name_string = "Icon for Sortest path computation";
XEvent report;

/************************************************************************************************************************/
// Function to reset the window everytime
void startOver(){
	int i, j;
	for(i=0; i<num_point;i++)
		for(j=0; j<num_point; j++)
			if (graph[i][j]> 0)
				XDrawLine(display_ptr, win, gc_red,poi[i][j][0].x, poi[i][j][0].y, poi[i][j][1].x, poi[i][j][1].y);
	for(i=0; i<num_point; i++){
		point[i].x = 0;
		point[i].y = 0;
	}
	for(i=0;i<num_point; i++){
		for(j=0; j<num_point; j++){
			graph[i][j] = 0;
			poi[i][j][0].x = 0; poi[i][j][0].y = 0; poi[i][j][1].x = 0; poi[i][j][1].y = 0;
		}
	}
	for(i=0; i <vertex_count; i++)
		XDrawLine(display_ptr, win, gc_green,result_vertices[i][0].x, result_vertices[i][0].y, result_vertices[i][1].x, result_vertices[i][1].y);
	for (i=0; i<obs_count ; i++){
		XDrawLine(display_ptr, win, gc_blue, triangles[i].v1.x, triangles[i].v1.y, triangles[i].v2.x, triangles[i].v2.y);
		XDrawLine(display_ptr, win, gc_blue, triangles[i].v2.x, triangles[i].v2.y, triangles[i].v3.x, triangles[i].v3.y);
		XDrawLine(display_ptr, win, gc_blue, triangles[i].v3.x, triangles[i].v3.y, triangles[i].v1.x, triangles[i].v1.y);
	}
	num_point = 0;
	vertex_count = 0;
	pathExist = false;
	return;
}
int point_orientation(Point a, Point b, Point c){
	return (a.x*b.y) + (b.x*c.y) + (c.x*a.y) - (a.y*b.x) - (b.y*c.x) - (c.y*a.x);
}
bool direction(int orientation){
	if (orientation == 0){
		return 0;
	}
	return (orientation > 0) ? 1: 2;
}
int find_distance(Point a, Point b){
	return (int) sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
}
bool isSamePoint (Point p, Point q, Point r, Point s) {
	if (p.x == r.x && p.y == r.y || q.x == s.x && q.y == s.y ||
		p.x == s.x && p.y == s.y || q.x == r.x && q.y == r.y){
		return true;
	}
    return false;
}
bool onSegment(Point p, Point q, Point r){
	if (r.x <= max(p.x, q.x) && r.x >= min(p.x, q.x) &&
		r.y <= max(p.y, q.y) && r.y >= min(p.y, q.y)){
		return true;
	}
	return false;
}
bool isIntriangle(int obs_count, Point p1){
	int i;
	bool check1, check2, check3;
	for (i = 0; i < obs_count; i++){
		check1 = direction(point_orientation(p1, triangles[i].v1, triangles[i].v2)) == direction(point_orientation(triangles[i].v1, triangles[i].v2, triangles[i].v3));
		check2 = direction(point_orientation(triangles[i].v2, triangles[i].v3, p1)) == direction(point_orientation(triangles[i].v2, triangles[i].v3, triangles[i].v1));
		check3 = direction(point_orientation(triangles[i].v1, triangles[i].v3, p1)) == direction(point_orientation(triangles[i].v1, triangles[i].v3, triangles[i].v2));
		if (check1 && check2 && check3){
			return true;
		}
	}
	return false;
}
bool isIntersect(Point p, Point q, Point r, Point s){
	int o1 = point_orientation(p,q,r);
	int o2 = point_orientation(p,q,s);
	int o3 = point_orientation(r,s,p);
	int o4 = point_orientation(r,s,q);
	if(isSamePoint(p, q, r, s)){
		return false;
	}
	if (direction(o1) != direction (o2) && direction(o3) != direction(o4)){
		return true;
	}
	if ((direction(o1) == 0 && onSegment(p,q,r)) ||
		(direction(o2) == 0 && onSegment(p,q,s)) ||
		(direction(o3) == 0 && onSegment(r,s,p)) ||
		(direction(o4) == 0 && onSegment(r,s,q))){
		return true;
	}
	return false;
}
bool intersectTriangles(Point p, Point q){
	int k;
	for (k=0; k<obs_count; k++){
		if (isIntersect(p, q, triangles[k].v1, triangles[k].v2) ||
			isIntersect(p, q, triangles[k].v2, triangles[k].v3) ||
			isIntersect(p, q, triangles[k].v3, triangles[k].v1)){
			return true;
		}
	}
	return false;
}
void start_graph(Point start, Point target){
	int i,j, k;
	Point p, q;
	for(i=0; i<num_point; i++){
		for(j=0; j<num_point; j++){
			p = point[i];
			q = point[j];
			if (!isIntriangle(obs_count, p) && !isIntriangle(obs_count, q)){
				if(!intersectTriangles(p, q)){
					graph[i][j] = find_distance(p,q);
					poi[i][j][0] = p;
					poi[i][j][1] = q;
				}
			}
		}
	}
	return;
}
int minimum_distance(int distance[], int processed[]){
	int min = INT_MAX;
	int min_index=0;
	int i;
	for(i=0; i<num_point; i++){
		if(processed[i] == 0 && distance[i]< min){
			min = distance[i];
			min_index = i;
		}
	}
	return min_index;
}
void shortestPathDijkstrasAlgo(int graph[][MAX_VERTICES], int src){
	int i, j, v;
	int processed[num_point], distance[num_point];
	int temp_v;
	for(i=0;i<MAX_VERTICES; i++){
		parent[i]= INT_MAX;
	}
	for(i=0; i<num_point; i++){
		processed[i]=0;
		distance[i]= INT_MAX;
	}
	distance[src] = 0;
	for(i=0; i<num_point; i++){
		int u = minimum_distance(distance, processed);
		processed[u] = 1;
		for (v=0; v< num_point; v++){
			if (!processed[v] && graph[u][v] && distance[u] != INT_MAX && distance[u]+graph[u][v] < distance[v]){
				distance[v] = distance[u] + graph[u][v];
				parent[v] = u;
			}
		}
	}
	return;
}
/*******************************************************************************************************************/
int main(int argc, char **argv)
{
	int allpoints[100][2];
	FILE *fp;
	int i, j;
	int click_count = -1;
	num_point =0;
	obs_count = 0;
	vertex_count=0;
	Point start, target, p1;
	fp = fopen(argv[1], "r");
	if (fp == NULL){
		printf("There's no input file.\n");
	}
	if( (display_ptr = XOpenDisplay(display_name)) == NULL ){
		printf("Could not open display. \n");
		exit(-1);
	}
	printf("Connected to X server  %s\n", XDisplayName(display_name) );
	screen_num = DefaultScreen(display_ptr);
	screen = DefaultScreenOfDisplay(display_ptr);
	colormap = XDefaultColormap(display_ptr, screen_num);
	display_width = DisplayWidth(display_ptr, screen_num);
	display_height = DisplayHeight(display_ptr, screen_num);
	int x1, y1, x2,y2, x3, y3;
	char t;
	if(fp!=NULL){
		fscanf(fp, "%s (%d,%d) (%d,%d) (%d,%d)", &t, &x1, &y1, &x2, &y2, &x3, &y3);
		while(!feof(fp)){
			if (t == 84){
				triangles[obs_count].v1.x = x1; triangles[obs_count].v1.y = y1;
				triangles[obs_count].v2.x = x2; triangles[obs_count].v2.y = y2;
				triangles[obs_count].v3.x = x3; triangles[obs_count++].v3.y = y3;
			}
			fscanf(fp, "%s (%d,%d) (%d,%d) (%d,%d)", &t, &x1, &y1, &x2, &y2, &x3, &y3);
		}
		rewind(fp);
	}
	for (i=0; i<obs_count; i++){
		printf("(%d , %d) (%d, %d) (%d, %d)\n", triangles[i].v1.x, triangles[i].v1.y, triangles[i].v2.x, triangles[i].v2.y, triangles[i].v3.x, triangles[i].v3.y);
	}
	border_width = 20;
	win_x = 0; win_y = 0;
	win_width = display_width/2;
	win_height = display_height * 0.8;
	printf("Window width - %d\nWindow height - %d\n", display_width, display_height);
	win = XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num),
		win_x, win_y, win_width, win_height, border_width,
		BlackPixel(display_ptr, screen_num), WhitePixel(display_ptr, screen_num));
	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();
	if (size_hints == NULL || wm_hints == NULL || class_hints == NULL){
		printf("Error allocating memory for hints\n");
		exit(-1);
	}
	size_hints -> flags = PPosition | PSize | PMinSize;
	size_hints -> min_width = 60;
	size_hints -> min_height = 60;
	XStringListToTextProperty(&win_name_string, 1, &win_name);
	XStringListToTextProperty(&icon_name_string, 1, &icon_name);
	wm_hints -> flags = StateHint | InputHint;
	wm_hints -> initial_state = NormalState;
	wm_hints -> input = False;
	class_hints -> res_name = "x_use";
	class_hints -> res_class = "assignment1";
	XSetWMProperties(display_ptr, win, &win_name, &icon_name, argv, argc, size_hints, wm_hints, class_hints );
	XSelectInput(display_ptr, win, ExposureMask | KeyPressMask | ButtonPressMask);
	// put on screen
	XMapWindow(display_ptr, win);
	XFlush(display_ptr);
	//graphics setup
	gc_green = XCreateGC(display_ptr, win, 0, 0);
	XParseColor(display_ptr, colormap, green, &color_green);
	if (XAllocColor(display_ptr, colormap, &color_green) == 0){
		printf("Failed to get color green\n");
		exit(-1);
	}
	else{
		XSetForeground(display_ptr, gc_green, color_green.pixel);
	}
	gc_red = XCreateGC(display_ptr, win, 0, 0);
	XParseColor(display_ptr, colormap, red, &color_red);
	if (XAllocColor(display_ptr, colormap, &color_red) == 0){
		printf("Failed to get color red\n");
		exit(-1);
	}
	else{
		XSetForeground(display_ptr, gc_red, color_red.pixel);
	}
	gc_black = XCreateGC(display_ptr, win, 0, 0);
	XParseColor(display_ptr, colormap, black, &color_black);
	XSetLineAttributes(display_ptr, gc_black, 2, LineSolid, CapRound, JoinRound);
	if (XAllocColor(display_ptr, colormap, &color_black) == 0){
		printf("Failed to get color black\n");
		exit(-1);
	}
	else{
		XSetForeground(display_ptr, gc_black, color_black.pixel);
	}
	
	gc_blue = XCreateGC(display_ptr, win, 0, 0);
	XParseColor(display_ptr, colormap, blue, &color_blue);
	if (XAllocColor(display_ptr, colormap, &color_blue) == 0){
		printf("Failed to get color blue\n");
		exit(-1);
	}
	else{
		XSetForeground(display_ptr, gc_blue, color_blue.pixel);
	}
	gc_white = XCreateGC(display_ptr, win, 0, 0);
	XParseColor(display_ptr, colormap, white, &color_white);
	XSetLineAttributes(display_ptr, gc_white, 2, LineSolid, CapRound, JoinRound);
	if (XAllocColor(display_ptr, colormap, &color_white) == 0){
		printf("Failed to get color white\n");
		exit(-1);
	}
	else{
		XSetForeground(display_ptr, gc_white, color_white.pixel);
	}
	printf("Total triangles - %d\n", obs_count);
	while(1){
		XNextEvent(display_ptr, &report);
		switch(report.type){
			case Expose:
			{
				for (i=0; i< obs_count ; i++){
					//Draw the triangles
					XDrawLine(display_ptr, win, gc_blue, triangles[i].v1.x, triangles[i].v1.y, triangles[i].v2.x, triangles[i].v2.y);
					XDrawLine(display_ptr, win, gc_blue, triangles[i].v2.x, triangles[i].v2.y, triangles[i].v3.x, triangles[i].v3.y);
					XDrawLine(display_ptr, win, gc_blue, triangles[i].v3.x, triangles[i].v3.y, triangles[i].v1.x, triangles[i].v1.y);
				}
				if(pathExist){
					for(i=0; i<num_point;i++){
						for(j=0; j<num_point; j++){
							if (graph[i][j]> 0){
								XDrawLine(display_ptr, win, gc_red,
									poi[i][j][0].x, poi[i][j][0].y, poi[i][j][1].x, poi[i][j][1].y);
							}
						}
					}
					for (i=0; i<vertex_count; i++){
						XDrawLine(display_ptr, win, gc_blue, result_vertices[i][0].x, result_vertices[i][0].y, result_vertices[i][1].x, result_vertices[i][1].y);
					}
				}
				XFlush(display_ptr);
				break;
			}
			case ButtonPress:
			{
				p1.x = report.xbutton.x;
				p1.y = report.xbutton.y;
				int vx[3],vy[3];
				if (report.xbutton.button == Button1){
					XFillArc( display_ptr, win, gc_blue, p1.x ,p1.y, 3, 3, 0, 360*64);
					if(close_flag==0){
						allpoints[pointCount][0]=p1.x;
						allpoints[pointCount][1]=p1.y;
						printf("Point clicked - %d %d\n",allpoints[pointCount][0],allpoints[pointCount][1]);
						pointCount++;
						vx[clickcount%3]=p1.x;
						vy[clickcount%3]=p1.y;
						if(clickcount%3 == 0){
							triangles[obs_count].v1.x = vx[0]; triangles[obs_count].v1.y = vy[0];
							triangles[obs_count].v2.x = vx[1]; triangles[obs_count].v2.y = vy[1];
							triangles[obs_count].v3.x = vx[2]; triangles[obs_count++].v3.y = vy[2];
						}clickcount++;
					}else{
						click_count++;
						if(isIntriangle(obs_count, p1)){
							printf("You are clicking inside triangle. Please click outside. \n");
							click_count = -1;
						}
						else{
						}
						if (click_count == 0){
							startOver();
							start = p1;
							XFillArc( display_ptr, win, gc_green,
							start.x-win_height/90 , start.y-win_height/90,
							win_height/70, win_height/70, 0, 360*64);
						}
						else if (click_count == 1){
							target = p1;
							XFillArc( display_ptr, win, gc_green,
							target.x-win_height/90 , target.y-win_height/90,
							win_height/70, win_height/70, 0, 360*64);
							for(i=0; i<obs_count; i++){
								point[num_point++] = triangles[i].v1;
								point[num_point++] = triangles[i].v2;
								point[num_point++] = triangles[i].v3;
							}
							point[num_point++] = start;
							point[num_point++] = target;
							printf("Start Point - (%d, %d)\nTarget point - (%d, %d)\n", start.x, start.y, target.x, target.y);
							start_graph(start, target);
							for(i=0; i<num_point;i++){
								for(j=0; j<num_point; j++){
									if (graph[i][j]> 0){
										XDrawLine(display_ptr, win, gc_red,
											poi[i][j][0].x, poi[i][j][0].y, poi[i][j][1].x, poi[i][j][1].y);
									}
								}
							}
							shortestPathDijkstrasAlgo(graph, num_point-2);
							int index = num_point-1;
							while(index != num_point-2){
								if(parent[index] == INT_MAX){
									pathExist = false;
									break;
								}
								pathExist = true;
								if (vertex_count == 0){
									result_vertices[vertex_count][0] = point[index];
									result_vertices[vertex_count][1] = point[index];
									distance[vertex_count++] = 0;
								}
								else {
									result_vertices[vertex_count][0] = result_vertices[vertex_count-1][1];
									result_vertices[vertex_count][1] = point[index];
									distance[vertex_count++] = find_distance(result_vertices[vertex_count][0], result_vertices[vertex_count][1]);
								}
								index = parent[index];
							}
							result_vertices[vertex_count][0] = result_vertices[vertex_count-1][1];
							result_vertices[vertex_count][1] = point[num_point-2];
							distance[vertex_count++] = find_distance(result_vertices[vertex_count-1][1], point[num_point-2]);
							if (pathExist){
								for (i=vertex_count-1; i>=0; i--){
									XDrawLine(display_ptr, win, gc_black, result_vertices[i][0].x, result_vertices[i][0].y, result_vertices[i][1].x, result_vertices[i][1].y);
								}
							}
							printf("Shortest Path computed\n");
							click_count = -1;
						}else{}
					}
				}
				else{
					close_flag++;
					if(close_flag==2){
						printf("Closing Window.\n");
						XDestroyWindow(display_ptr, win);
						XCloseDisplay(display_ptr);
						exit(1);
					}
				}	
				startOver();
				break;
			}
			default:
				break;
		}
	}
	fclose(fp);
	return 0;
}	
//end of main
// Submitter name: AVINASH BENGERI
