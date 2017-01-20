// Submitter name: AVINASH BENGERI
/* Program to solve Traveling Salesman Problem using Held_Karp algorithm */
/* Compiles with command line  g++ tsp_final.cpp -lX11 -lm -L/usr/X11R6/lib -o tsp */
/* Runs with command line  ./tsp.exe  tspinput.txt */
/********************************************************************************/
/*																				*/									
/*		------------------- Instructions -------------------            		*/
/*																				*/									
/*		1. Use Left Mouse Click on the screen to add destinations for TSP. 		*/
/*		2. Right Mouse click to see Held_Karp applied.						 	*/									
/*		   vertical strip.														*/									
/*		3. Another right mouse click will to repeat the process again.			*/									
/*																				*/									
/********************************************************************************/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <limits>
#include <fstream>
using namespace std;

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
char *win_name_string = (char*)"TSP using Held Karp";
char *icon_name_string = (char*)"Icon for TSP Window";
XEvent report;
GC gc, gc_yellow, gc_red, gc_grey, gc_blue;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values, gc_blue_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;
struct Point{
    int x;
    int y;
	bool link;
    Point* next;
};
struct Node{
    int Value;
    bool Selected;
    std::vector<Node> Nodes;
    int CountNodes;
};
unsigned int **Distance;
int **p;
int **g;
void cost(int numOfVertices);
void emptyCost(int numOfVertices);
void subset(int n, int numOfVertices);
int compute(int start, int set, int n, int numOfVertices);
void getpath(int start, int set, int n);
void addDistances(int numOfVertices, int n);
void computeTravelCosts(int n, int numOfVertices);
int getCost(int x1, int y1, int x2, int y2);
Point* pointArr;
Point* Points;
int pointCount;
bool fileArg = false;
std::vector<int> finalVector;
std::vector<Point*> finalVectorSet[10];
int CountSolutions=0;

void cost(unsigned int numOfVertices){
	Distance = new unsigned int*[numOfVertices];
	for (unsigned int i = 0; i < numOfVertices; i++)
		Distance[i] = new unsigned int[numOfVertices];
}
void emptyCost(int numOfVertices){
	for (unsigned int i = 0; i < numOfVertices; i++)
		delete[] Distance[i];
	delete Distance;
}
void subset(int n, unsigned int numOfVertices){
	g = new int *[n];
	p = new int *[n];
	for (unsigned int i = 0; i < numOfVertices; i++){
		g[i] = new int[n];
		p[i] = new int[n];
	}
}
int compute(int start, int set, int n, unsigned int numOfVertices){
	int masked, mask, result = numeric_limits<int>::max(), temp;

	if (g[start][set] != -1)
		return g[start][set];
	for (unsigned int i = 0; i < numOfVertices; i++){
		mask = (n - 1) - (1 << i);
		masked = set & mask;
		if (masked != set){
			temp = Distance[start][i]
			        + compute(i, masked, n, numOfVertices);
			if (temp < result)
				result = temp, p[start][set] = i;
		}
	}
	return g[start][set] = result;
}

void getpath(int start, int set, int n) {
	if (p[start][set] == -1)
		return;
	int x = p[start][set];
	int mask = (n - 1) - (1 << x); 
	int masked = set & mask;
	finalVector.push_back(x);
	getpath(x, masked, n);
}

void addDistances(const unsigned int numOfVertices, int n)
{
	cost(numOfVertices);
	Point* starting = pointArr;
	int j=0;
	while(starting != NULL && j<n){
		starting = starting->next;
		j++;
	}
	Point *tempOuter = starting;
	int i=0;
	while(tempOuter != NULL && i<numOfVertices){
		XFillArc(display_ptr, win, gc, tempOuter->x-2, tempOuter->y-2,4, 4, 0, 360 * 64);
		Points[i].x = tempOuter->x;
		Points[i].y = tempOuter->y;
		Point *tempInner = starting;
		int j=0;
		while(tempInner!=NULL && j<numOfVertices){
			if(i!=j){
				Distance[i][j]=getCost(tempOuter->x,tempOuter->y, tempInner->x, tempInner->y);
			}else{
				Distance[i][j]=0;
			}
			j++;
			tempInner = tempInner->next;
		}
		tempOuter = tempOuter->next;
		i++;
	}
}

void computeTravelCosts(int a, unsigned int numOfVertices){
	for (unsigned int i = 0; i < numOfVertices; i++)
		for (int j = 0; j < a; j++)
			g[i][j] = p[i][j] = -1;
	for (unsigned int i = 0; i < numOfVertices; i++)
		g[i][0] = Distance[i][0];
	int result = compute(0, a-2, a, numOfVertices);
	printf("Dist travelled - %d\n", result);
	finalVector.clear();
	finalVector.push_back(0);
	getpath(0, a-2, a);
	std::vector<Point*> solPoints;
	for(int ia = 0; ia<finalVector.size(); ia++){
		Point* toStore = new Point();
		toStore->x=Points[finalVector.at(ia)].x;
		toStore->y = Points[finalVector.at(ia)].y;
		toStore->link = false;
		solPoints.push_back(toStore);
	}
	finalVectorSet[CountSolutions++] = solPoints;
}

int getCost(int x1, int y1, int x2, int y2){
	int diffx = x1 - x2;
	int diffy = y1 - y2;
	int diffx_sqr = pow(diffx,2);
	int diffy_sqr = pow(diffy, 2);
	int distance = sqrt(diffx_sqr + diffy_sqr);

return distance;
}
void sortPoints(){
	Point *outer = pointArr;
	Point *inner;
	int x,y;
	while(outer!=NULL){
		inner = outer->next;
		while(inner != NULL){
			if(outer->x > inner->x){
				x = outer->x;
				y = outer->y;
				outer->x = inner->x;
				outer->y = inner->y;
				inner->x=x;
				inner->y=y;
			}
			inner=inner->next;
		}
		outer = outer->next;
	}
}

void integratePath(){
	int minPos1=0;
	int minPos2=0;
	int minPos1_S=0;
	int minPos2_S=0;
	for(int x=0; x<CountSolutions-1; x++){
		minPos1=-1;
		minPos2=-1;
		minPos1_S=-1;
		minPos2_S=-1;
		int min=numeric_limits<int>::max();
		for(int i=0; i<finalVectorSet[x].size(); i++){
			int x1 = finalVectorSet[x].at(i)->x;
			int y1 = finalVectorSet[x].at(i)->y;
			for(int j=0; j<finalVectorSet[x+1].size(); j++){
				int x2 = finalVectorSet[x+1].at(j)->x;
				int y2 = finalVectorSet[x+1].at(j)->y;
				int d = getCost(x1, y1, x2, y2);
				if(d < min){
					min = d;
					minPos1=i;
					minPos2=j;
				}
			}
		}
		finalVectorSet[x].at(minPos1)->link=true;
		finalVectorSet[x+1].at(minPos2)->link=true;
		XDrawLine(display_ptr, win, gc_blue, finalVectorSet[x].at(minPos1)->x, finalVectorSet[x].at(minPos1)->y, finalVectorSet[x+1].at(minPos2)->x, finalVectorSet[x+1].at(minPos2)->y);
		min=numeric_limits<int>::max();
		for(int i=0; i<finalVectorSet[x].size(); i++){
			if(i!=minPos1){
				int x1 = finalVectorSet[x].at(i)->x;
				int y1 = finalVectorSet[x].at(i)->y;
				for(int j=0; j<finalVectorSet[x+1].size(); j++){
					if(j!=minPos2){
						int x2 = finalVectorSet[x+1].at(j)->x;
						int y2 = finalVectorSet[x+1].at(j)->y;
						int d = getCost(x1, y1, x2, y2);
						if(d < min){
							if(((i+1)==minPos1 || (i-1)==minPos1)&& ((j+1)==minPos2 || (j-1)==minPos2)){
								min = d;
								minPos1_S=i;
								minPos2_S=j;
							}
						}
					}
				}
			}
		}
		finalVectorSet[x].at(minPos1_S)->link=true;
		finalVectorSet[x+1].at(minPos2_S)->link=true;
		XDrawLine(display_ptr, win, gc_blue, finalVectorSet[x].at(minPos1_S)->x, finalVectorSet[x].at(minPos1_S)->y, finalVectorSet[x+1].at(minPos2_S)->x, finalVectorSet[x+1].at(minPos2_S)->y);
	}
	for(int x=0; x<CountSolutions; x++){
		for(int i=0; i<finalVectorSet[x].size()-1; i++){
			if(!(finalVectorSet[x].at(i)->link && finalVectorSet[x].at(i+1)->link))
				XDrawLine(display_ptr, win, gc_blue, finalVectorSet[x].at(i)->x, finalVectorSet[x].at(i)->y, finalVectorSet[x].at(i+1)->x, finalVectorSet[x].at(i+1)->y);
			else
			{
				XDrawLine(display_ptr, win, gc_red, finalVectorSet[x].at(i)->x, finalVectorSet[x].at(i)->y, finalVectorSet[x].at(i+1)->x, finalVectorSet[x].at(i+1)->y);
				finalVectorSet[x].at(i)->link = false;
				finalVectorSet[x].at(i+1)->link = false;
			}
		}
		if(!(finalVectorSet[x].front()->link && finalVectorSet[x].back()->link)){
			XDrawLine(display_ptr, win, gc_blue, finalVectorSet[x].front()->x, finalVectorSet[x].front()->y, finalVectorSet[x].back()->x, finalVectorSet[x].back()->y);
		}
	}
}

void drawGraph(){
	XClearWindow(display_ptr, win);
	Point *tempInner = pointArr;
	int j=0;
	while(tempInner!=NULL && j<pointCount){
		XDrawPoint(display_ptr, win, gc, tempInner->x, tempInner->y);
		j++;
		tempInner = tempInner->next;
	}
	sortPoints();
	int limit=20;
	if(pointCount<limit){
		Points = new Point[pointCount];
		unsigned int numOfVertices = pointCount;
		int nPow = pow(2, numOfVertices);
		subset(nPow, numOfVertices);
		addDistances(numOfVertices, 0);
		finalVector.clear();
		computeTravelCosts(nPow, numOfVertices);
		emptyCost(numOfVertices);
		for(int x=0; x<CountSolutions; x++){
			for(int i=0; i<finalVectorSet[x].size()-1; i++){
				XDrawLine(display_ptr, win, gc_blue, finalVectorSet[x].at(i)->x, finalVectorSet[x].at(i)->y, finalVectorSet[x].at(i+1)->x, finalVectorSet[x].at(i+1)->y);
			}
			XDrawLine(display_ptr, win, gc_blue, finalVectorSet[x].front()->x, finalVectorSet[x].front()->y, finalVectorSet[x].back()->x, finalVectorSet[x].back()->y);
		}
	}else{
		int division = pointCount / limit+1;
		int offset=limit;
		offset = pointCount/division;
		for(int i=0; i<division; i++){
			unsigned int numOfVertices = offset;
			if(i==division-1){
				if(pointCount%division!=0){
					numOfVertices+=pointCount%division;
				}
			}
			Points = new Point[numOfVertices];
			int nPow = pow(2, numOfVertices);
			subset(nPow, numOfVertices);
			addDistances(numOfVertices, i*offset);
			computeTravelCosts(nPow, numOfVertices);
			emptyCost(numOfVertices);
		}
		integratePath();
	}
}

bool fexists(const char *filename){
  ifstream ifile(filename);
  return ifile;
}

int main(int argc, char* argv[])
{
	struct Point *firstPoint;
	char filename[255];
	if(argc > 1){
		strcpy(filename, argv[1]);
	}
	char line[80];
	FILE *fr;
	if(fexists(filename)){
		pointArr = NULL;
		pointCount=0;
		fr = fopen(filename,"rt");
		while(fgets(line, 80, fr) != NULL){
			firstPoint = new Point();

			sscanf (line, "(%d,%d)\n", &firstPoint->x, &firstPoint->y);
			if(pointArr == NULL){
				pointArr = firstPoint;
			}else{
				firstPoint->next = pointArr;
				pointArr = firstPoint;
			}
			pointCount++;
		}
		fileArg = true;
	}else{
		fileArg = false;
	}
	/* opening display: basic connection to X Server */
	if( (display_ptr = XOpenDisplay(display_name)) == NULL )
	{ printf("Could not open display. \n"); exit(-1);}
	screen_num = DefaultScreen( display_ptr );
	screen_ptr = DefaultScreenOfDisplay( display_ptr );
	color_map  = XDefaultColormap( display_ptr, screen_num );
	display_width  = DisplayWidth( display_ptr, screen_num );
	display_height = DisplayHeight( display_ptr, screen_num );
	/* creating the window */
	border_width = 10;
	win_x = 0; win_y = 0;
	/*bound window*/
	win_width = 800;
	win_height = 800;
	win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num), win_x, win_y, win_width, win_height, border_width,
							BlackPixel(display_ptr, screen_num), WhitePixel(display_ptr, screen_num) );
	/* now try to put it on screen, this needs cooperation of window manager */
	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();
	if( size_hints == NULL || wm_hints == NULL || class_hints == NULL )
	{ printf("Error allocating memory for hints. \n"); exit(-1);}
	size_hints -> flags = PPosition | PSize | PMinSize  ;
	size_hints -> min_width = 60;
	size_hints -> min_height = 60;
	XStringListToTextProperty( &win_name_string,1,&win_name);
	XStringListToTextProperty( &icon_name_string,1,&icon_name);
	wm_hints -> flags = StateHint | InputHint ;
	wm_hints -> initial_state = NormalState;
	wm_hints -> input = False;
	class_hints -> res_name = (char*)"x_use_example";
	class_hints -> res_class = (char*)"examples";
	XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,size_hints, wm_hints, class_hints );
	/* what events do we want to receive */
	XSelectInput( display_ptr, win, ExposureMask | StructureNotifyMask | ButtonPressMask );
	/* finally: put window on screen */
	XMapWindow( display_ptr, win );
	XFlush(display_ptr);
	/* create graphics context, so that we may draw in this window */
	gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
	XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
	XSetLineAttributes( display_ptr, gc, 1, LineSolid, CapNotLast, JoinMiter);
	/* and three other graphics contexts, to draw in yellow and red and grey*/
	gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
	XSetLineAttributes(display_ptr, gc_yellow, 2, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "yellow",
			&tmp_color1, &tmp_color2 ) == 0 )
	{printf("failed to get color yellow\n"); exit(-1);}
	else
	XSetForeground( display_ptr, gc_yellow, tmp_color1.pixel );

	gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
	XSetLineAttributes( display_ptr, gc_red, 2, LineSolid, CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "red",
			&tmp_color1, &tmp_color2 ) == 0 )
	{printf("failed to get color red\n"); exit(-1);}
	else
	XSetForeground( display_ptr, gc_red, tmp_color1.pixel );

	gc_blue = XCreateGC( display_ptr, win, valuemask, &gc_blue_values);
	XSetLineAttributes( display_ptr, gc_blue, 2, LineSolid, CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "blue",
			&tmp_color1, &tmp_color2 ) == 0 )
	{printf("failed to get color blue\n"); exit(-1);}
	else
	XSetForeground( display_ptr, gc_blue, tmp_color1.pixel );

	gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
	if( XAllocNamedColor( display_ptr, color_map, "light grey",
			&tmp_color1, &tmp_color2 ) == 0 )
	{printf("failed to get color grey\n"); exit(-1);}
	else
	XSetForeground( display_ptr, gc_grey, tmp_color1.pixel );

  /* and now it starts: the event loop */
	while(1){
		XNextEvent( display_ptr, &report );
		switch( report.type ){
			case Expose:
				if(fileArg){
					drawGraph();
					fileArg=false;
					pointCount=0;
					delete[] pointArr;
					pointArr = NULL;
					CountSolutions=0;
				}
				break;
			case ConfigureNotify:
				/* This event happens when the user changes the size of the window*/
				win_width = report.xconfigure.width;
				win_height = report.xconfigure.height;
				break;
			case ButtonPress:
			  {
				if(fileArg){
					drawGraph();
					fileArg=false;
					pointCount=0;
					delete[] pointArr;
					pointArr = NULL;
					CountSolutions=0;
				}else{
					int x, y;
					x = report.xbutton.x;
					y = report.xbutton.y;
					if (report.xbutton.button == Button1 ){
						firstPoint = new Point();
						firstPoint->x = x;
						firstPoint->y = y;
						if(pointArr == NULL){
							pointArr = firstPoint;
						}else{
							firstPoint->next = pointArr;
							pointArr = firstPoint;
						}
						pointCount++;
						XFillArc(display_ptr, win, gc, x-2, y-2,4, 4, 0, 360 * 64);
					}else{
						if(pointCount>0){
							drawGraph();
							pointCount=0;
							delete[] pointArr;
							pointArr = NULL;
							CountSolutions=0;
						}else{
							XClearWindow(display_ptr, win);
						}
					}
				}
			}
			break;
			default:
		  /* this is a catch-all for other events; it does not do anything.
				 One could look at the report type to see what the event was */
			  break;
		}
	}
    return 0;
}