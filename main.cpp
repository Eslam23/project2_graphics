#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE

#endif

#include <string>
#include <sstream>
#include <cmath>
#include <math.h>
#include <tchar.h>
#include <windows.h>
#include <stack>
#include <bits/stdc++.h>
//#define FILE_MENU_Line 20
using namespace std;
#define ADDPOPUPMENU(hmenu, string) \
	HMENU hSubMenu = CreatePopupMenu(); \
	AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, string);

// Add a menu item
#define ADDMENUITEM(hmenu, ID, string) \
	AppendMenu(hSubMenu, MF_STRING, ID, string);

/*  Declare Windows procedure  */
enum
{
	FILE_MENU_EXIT,
	DDA,Midpoint,Parametric,
	Direct,Polar,Mid,bezier,
	hermite,splines,clipping_line,clipping_polygon,Save,Load
};


LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void AddMenus (HWND);
HMENU hMenu;
/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

      AddMenus(hwnd);
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}
void swap(int& x,int& y)
{
int tmp=x;
x=y;
y=tmp;
}
int round(float x)
{
return (int)(x+0.5);
}
//clipping
struct Vertex2{
    double x,y;
    Vertex2(int x1=0,int y1=0)
    {
    x=x1;
    y=y1;
    }
};
typedef vector<Vertex2> VertexList;
typedef bool (*IsInFunc)(Vertex2& v,int edge);
typedef Vertex2 (*IntersectFunc)(Vertex2& v1,Vertex2& v2,int edge);
VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect){
    VertexList OutList;
    Vertex2 v1=p[p.size()-1];
    bool v1_in=In(v1,edge);
    for(int i=0;i<(int)p.size();i++){
        Vertex2 v2=p[i];
        bool v2_in=In(v2,edge);
        if(!v1_in && v2_in){
            OutList.push_back(Intersect(v1,v2,edge));
            OutList.push_back(v2);
        }
        else if(v1_in && v2_in) OutList.push_back(v2);
        else if(v1_in) OutList.push_back(Intersect(v1,v2,edge));
        v1=v2;
        v1_in=v2_in;
    }
    return OutList;
}
bool InLeft(Vertex2& v,int edge)
{
return v.x>=edge;
}
bool InRight(Vertex2& v,int edge)
{
return v.x<=edge;
}
bool InTop(Vertex2& v,int edge)
{
return v.y>=edge;
}
bool InBottom(Vertex2& v,int edge)
{
return v.y<=edge;
}
Vertex2 VIntersect(Vertex2& v1,Vertex2& v2,int xedge){
    Vertex2 res;
    res.x=xedge;
    res.y=v1.y+(xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}
Vertex2 HIntersect(Vertex2& v1,Vertex2& v2,int yedge){
    Vertex2 res;
    res.y=yedge;
    res.x=v1.x+(yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);
    return res;
}

void PolygonClip(HDC hdc,POINT *p,int n,int xleft,int ytop,int xright,int ybottom){
    VertexList vlist;
    for(int i=0;i<n;i++)
        vlist.push_back(Vertex2(p[i].x,p[i].y));
    vlist=ClipWithEdge(vlist,xleft,InLeft,VIntersect);
    vlist=ClipWithEdge(vlist,ytop,InTop,HIntersect);
    vlist=ClipWithEdge(vlist,xright,InRight,VIntersect);
    vlist=ClipWithEdge(vlist,ybottom,InBottom,HIntersect);
    Vertex2 v1=vlist[vlist.size()-1];
    for(int i=0;i<(int)vlist.size();i++){
        Vertex2 v2=vlist[i];
        MoveToEx(hdc,round(v1.x),round(v1.y),NULL);
        LineTo(hdc,round(v2.x),round(v2.y));
        v1=v2;
    }
}

void SimpleDDA(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color)
{
   int dx=xe-xs;
   int dy=ye-ys;
    SetPixel(hdc,xs,ys,color);
   if(abs(dx)>=abs(dy))
 {
  int x=xs,xinc= dx>0?1:-1;
  double y=ys,yinc=(double)dy/dx*xinc;
  while(x!=xe)
 {
  x+=xinc;
  y+=yinc;
  SetPixel(hdc,x,round(y),color);
 }
 }
 else
{
  int y=ys,yinc= dy>0?1:-1;
  double x=xs,xinc=(double)dx/dy*yinc;
 while(y!=ye)
{
   x+=xinc;
   y+=yinc;
   SetPixel(hdc,round(x),y,color);
}
}
}
void midPoint(HDC hdc,int X1, int Y1, int X2, int Y2,COLORREF color)
{
    int dx = X2 - X1;
    int dy = Y2 - Y1;
    int d = dy - (dx/2);
    int x = X1, y = Y1;
   SetPixel(hdc,x,y,color);


    while (x < X2)
    {
        x++;


        if (d < 0)
            d = d + dy;


        else
        {
            d += (dy - dx);
            y++;
        }

        SetPixel(hdc,x,y,color);

    }
}
void parametric_line(HDC hdc,int x1,int x2,int y1,int y2,COLORREF color ){
 for(double t=0;t<=1;t+=0.001){
    int x=round(x1+t*(x2-x1));
    int y=round(y1+t*(y2-y1));
    SetPixel(hdc,x,y,color);
 }

}
void draw_polar_elipse(HDC hdc,int xc,int yc,int A,int B,COLORREF color){
for(double theta=0;theta<6.28;theta+=0.001)
{

    int x=round(xc+A*cos(theta));
    int y=round(yc+B*sin(theta));
    SetPixel(hdc,x,y,color);
}
}
void ellipse_midpoint(HDC hdc,int xc,int yc,int A,int B,COLORREF color)
 {
   int x, y, d;
   x=0;
   y=B;
   d=(B*B)-(A*A*B)+((A*A)/4);
   while((2*x*B*B)<(2*y*A*A))
   {
        SetPixel(hdc,xc+x,yc-y,color);
        SetPixel(hdc,xc-x,yc+y,color);
        SetPixel(hdc,xc+x,yc+y,color);
        SetPixel(hdc,xc-x,yc-y,color);

        if(d<0)
        {
     x=x+1;
     d=d+(2*B*B*x)+(B*B);
        }
        else
        {
     x=x+1;
     y=y-1;
     d=d+(2*B*B*x+B*B)-(2*A*A*y);
        }
   }
   d=((float)x+0.5)*((float)x+0.5)*B*B+(y-1)*(y-1)*A*A-A*A*B*B;

         while(y>=0)
   {
        SetPixel(hdc,xc+x,yc-y,color);
        SetPixel(hdc,xc-x,yc+y,color);
        SetPixel(hdc,xc+x,yc+y,color);
        SetPixel(hdc,xc-x,yc-y,color);


        if(d>0)
        {
         y=y-1;
        d=d-(2*A*A*y)+(A*A);

        }
        else
        {
         y=y-1;
         x=x+1;
         d=d+(2*B*B*x)-(2*A*A*y)-(A*A);
        }
   }
 }
 void Draw8Points(HDC hdc,int xc,int yc,int a,int b,COLORREF color)
{
    SetPixel(hdc,xc+a,yc+b,color);
    SetPixel(hdc,xc+a,yc-b,color);
    SetPixel(hdc,xc-a,yc+b,color);
    SetPixel(hdc,xc-a,yc-b,color);
    SetPixel(hdc,xc+b,yc+a,color);
    SetPixel(hdc,xc+b,yc-a,color);
    SetPixel(hdc,xc-b,yc+a,color);
    SetPixel(hdc,xc-b,yc-a,color);
}

struct Vector2{
    double x,y;
    Vector2(double a=0,double b=0){
        x=a; y=b;
    }
};

class Vector4{
    double v[4];
    public:
    Vector4(double a=0,double b=0,double c=0,double d=0){
        v[0]=a; v[1]=b; v[2]=c; v[3]=d;
    }
    Vector4(double a[]){
        memcpy(v,a,4*sizeof(double));
    }
    double& operator[](int i){
        return v[i];
    }
};

class Matrix4{
    Vector4 M[4];
    public:
    Matrix4(double A[]){
        memcpy(M,A,16*sizeof(double));
    }
    Vector4& operator[](int i){
        return M[i];
    }
};

Vector4 operator*(Matrix4 M,Vector4& b){
    Vector4 res;
    for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
    res[i]+=M[i][j]*b[j];
    return res;
}

double DotProduct(Vector4& a,Vector4& b){
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
}

Vector4 GetHermiteCoeff(double x0,double s0,double x1,double s1){
    static double H[16]={2,1,-2,1,-3,-2,3,-1,0,1,0,0,1,0,0,0};
    static Matrix4 basis(H);
    Vector4 v(x0,s0,x1,s1);
    return basis*v;
}

void DrawCurveHermite(HDC hdc,Vector2& P0,Vector2& T0,Vector2& P1,Vector2& T1 ,int numpoints, COLORREF color){
    Vector4 xcoeff=GetHermiteCoeff(P0.x,T0.x,P1.x,T1.x);
    Vector4 ycoeff=GetHermiteCoeff(P0.y,T0.y,P1.y,T1.y);
    if(numpoints<2)return;
    double dt=1.0/(numpoints-1);
    for(double t=0;t<=1;t+=dt)
    {
    Vector4 vt;
    vt[3]=1;
    for(int i=2;i>=0;i--)vt[i]=vt[i+1]*t;
    int x=round(DotProduct(xcoeff,vt));
    int y=round(DotProduct(xcoeff,vt));
    SetPixel(hdc,x,y,color);
    }
}

void CurveHermite(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,COLORREF color)
{
    int a1,b1,c1,d1;
    a1= 2 * x1 + x2 - 2 * x3 + x4;
    b1= -3 * x1 -2* x2 + 3 * x3 - x4;
    c1= x2;
    d1= x1 ;
    int a2, b2, c2, d2;
    a2 = 2 * y1 + y2 - 2 * y3 + y4;
    b2 = -3 * y1 - 2 * y2 + 3 * y3 - y4;
    c2 = y2;
    d2 = y1;
    double x,y;
    for (double t = 0; t <= 1; t += 0.001)
    {
        x=a1*(t*t*t)+b1*(t*t)+c1*(t)+d1;
        y=a2*(t*t*t)+b2*(t*t)+c2*(t)+d2;
        SetPixel(hdc,round(x),round(y),color);
    }
}




typedef double vect4[4];
typedef double mat4[4][4];

void multi(mat4 a, vect4 b, vect4 c)
{
	for (int i = 0; i < 4; i++)
	{
		c[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			c[i] += a[i][j] * b[j];
		}
	}
}
double dot(vect4 a, vect4 b)
{
	double sum = 0;
	for (int i = 0; i < 4; i++)
	{
		sum += a[i] * b[i];

	}
	return sum;
}
void DrawCurveBezier(HDC hdc, int xs, int ys, int xe, int ye, int x1, int x2, int y1, int y2, COLORREF color){
	static mat4 H = {{2,1,-2,1},{-3,-2,3,-1},{0,1,0,0},{1,0,0,0}};/*{ { -1, 3, -3, 1 }, { 3, -6, 3, 0 }, { -3, 3, 0, 0 }, { 1, 0, 0, 0 } };*/
	vect4 vx = { xs, x1, xe, x2 };
	vect4 vy = { ys, y1, ye, y2 };
	vect4 Gx, Gy;
	multi(H, vx, Gx);
	multi(H, vy, Gy);
	int n = max(abs(xe - xs), abs(ye - ys));
	double dt = 1.0 / n;
	double t = 0;
	for (int i = 0; i < n; /*i++*/i++)
	{
		double t2 = t*t, t3 = t2*t;
		vect4 vt = { t3, t2, t, 1 };
		double x = dot(Gx, vt);
		double y = dot(Gy, vt);
		SetPixel(hdc, round(x), round(y), color);
		t += dt;
	}
}
void Save_drawing(HDC hdc, const char* filename,RECT rect)
{

    fstream file;
    COLORREF pixel;
    file.open (filename,ios::out | ios::trunc);
    for(int i=0;i<=rect.right;i++){
        for(int j=0;j<=rect.bottom;j++){
            pixel=GetPixel(hdc,i,j);
            file<<i<<" "<<j<<" "<<pixel<<" ";
        }
    }
    file.close();
    MessageBox(NULL,("Saved done"), ("save"), MB_OK);
}

void Load_drawing(HDC hdc, const char* filename,RECT rect)
{

    COLORREF pixel;
    int i,j;
    fstream file;
    file.open(filename,ios::in);
    while(!file.eof()){
        file>>i>>j>>pixel;
        SetPixel(hdc,i,j,pixel);
    }
    MessageBox(NULL,("Loaded done"), ("load"), MB_OK);
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

{
    HDC  hdc;
    static int x,y,xs,ys,xe,ye,A,B,r,x1,y1,x2,y2,x3,y3,x4,y4;
    static int temp=0;
    static bool line=false,first=true;
    static string option="";
     static int temp2=0;
    POINT p2[100];
    Vector2 v[100];
    PAINTSTRUCT ps;
    RECT rect;
    hdc = GetDC(hwnd);
    GetClientRect(hwnd,&rect);
    //static int xs,ys,xe,ye, A,B;
    switch (message)
	{

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
			return 0;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case FILE_MENU_EXIT:
					PostQuitMessage(0);
					break;
//FILE_MENU_EXIT,
	//DDA,Midpoint,Parametric,
	//Direct,Polar,Mid,bezier,
	//hermite,splines
                case Save:
                    Save_drawing(hdc,"consol.txt",rect);
                    break;
                case Load:
                    Load_drawing(hdc,"consol.txt",rect);
                    break;
				case  DDA:
                    option="lineD";
					break;


                case Midpoint:
                    option="lineM";
                    break;



                case Parametric:
                    option="lineP";
                    break;


                case Direct:
                    option="EllipseD";
                    break;




                case Polar:
                    option="EllipseP";
                    break;

                case Mid:
                    option="EllipseM";
                    break;

                case hermite:
                    option="curveH";
                    break;


                case bezier:
                    option="curveB";
                    break;

                case clipping_polygon:
                    option="clippingP";
                    SimpleDDA(hdc,rect.left+100,rect.top+100,rect.left+100,rect.bottom-100,RGB(255,0,0));
                    SimpleDDA(hdc,rect.right-100,rect.top+100,rect.right-100,rect.bottom-100,RGB(255,0,0));
                    SimpleDDA(hdc,rect.left+100,rect.top+100,rect.right-100,rect.top+100,RGB(255,0,0));
                    SimpleDDA(hdc,rect.left+100,rect.bottom-100,rect.right-100,rect.bottom-100,RGB(255,0,0));
                    break;

			}

		} break;
		/*
				case WM_ERASEBKGND:
              HPEN pen;
              HBRUSH brush;

              pen=CreatePen(PS_SOLID, 1, bkgndcolor);
              brush=CreateSolidBrush(bkgndcolor);
              SelectObject((HDC)wParam, pen);
              SelectObject((HDC)wParam, brush);

              GetClientRect(hwnd, &rect);

              Rectangle((HDC)wParam, rect.left, rect.top, rect.right, rect.bottom);
		break;*/
/*
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
		break;
*/case WM_ERASEBKGND:
            //HPEN pen;
            //HBRUSH brush;

           //n=CreatePen(PS_SOLID, 1,RGB(255,0,0) );
            //brush=CreateSolidBrush(RGB(255,0,0));
            //SelectObject((HDC)wParam, pen);
            //SelectObject((HDC)wParam, brush);

            //GetClientRect(hwnd, &rect);

              Rectangle((HDC)wParam, rect.left, rect.top, rect.right, rect.bottom);
		break;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
		break;

        case WM_LBUTTONDOWN:
            if(option=="lineD"){
                if(first){
                    xs = LOWORD(lParam);
                    ys = HIWORD(lParam);
                    first=false;
                }else{
                    xe = LOWORD(lParam);
                    ye = HIWORD(lParam);
                    first=true;
                 SimpleDDA(hdc,xs,ys,xe,ye,RGB(255,0,0));
                }
            }
               else if(option=="lineM"){
                if(first){
                    xs = LOWORD(lParam);
                    ys = HIWORD(lParam);
                    first=false;
                }else{
                    xe = LOWORD(lParam);
                    ye = HIWORD(lParam);
                    first=true;
                  midPoint(hdc,xs,ys,xe,ye,RGB(255,0,0));
                }
            }else if(option=="lineP"){
                if(first){
                    xs = LOWORD(lParam);
                    ys = HIWORD(lParam);
                    first=false;
                }else{
                    xe = LOWORD(lParam);
                    ye = HIWORD(lParam);
                    first=true;
                    //r=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
                    parametric_line(hdc,xs,ys,xe,ye,RGB(255,0,0));
                }
            }else if(option=="EllipseM"){
                if(first){
                    xs = LOWORD(lParam);
                    ys = HIWORD(lParam);
                    first=false;
                }else{
                    xe = LOWORD(lParam);
                    ye = HIWORD(lParam);
                    first=true;
                    A=50;
                    B=100;
                    //r=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
                    ellipse_midpoint(hdc,xs,ys,A,B,RGB(255,0,0));
                }
            }
          else if(option=="EllipseP"){
                if(first){
                    xs = LOWORD(lParam);
                    ys = HIWORD(lParam);
                    first=false;
                }else{
                    xe = LOWORD(lParam);
                    ye = HIWORD(lParam);
                    first=true;
                    A=50;
                    B=100;
                    //r=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
                    draw_polar_elipse(hdc,xs,ys,A,B,RGB(255,0,0));
                }
            }



            else if(option=="curveH"){
                if(temp==0){
                    x1 = LOWORD(lParam);
                    y1 = HIWORD(lParam);
                    temp=1;
                }else if (temp==1){
                    x2 = LOWORD(lParam);
                    y2 = HIWORD(lParam);
                    temp=2;
                }else if (temp==2){
                    x3 = LOWORD(lParam);
                    y3 = HIWORD(lParam);
                    temp=3;
                }else if (temp==3){
                    x4 = LOWORD(lParam);
                    y4 = HIWORD(lParam);
                    CurveHermite(hdc,x1,y1,x2,y2,x3,y3,x4,y4,RGB(255,0,0));
                    temp=0;
                }


            }else if(option=="curveB"){
                if(temp==0){
                    x1 = LOWORD(lParam);
                    y1 = HIWORD(lParam);
                    temp=1;
                }else if (temp==1){
                    x2 = LOWORD(lParam);
                    y2 = HIWORD(lParam);
                    temp=2;
                }else if (temp==2){
                    x3 = LOWORD(lParam);
                    y3 = HIWORD(lParam);
                    temp=3;
                }else if (temp==3){
                    x4 = LOWORD(lParam);
                    y4 = HIWORD(lParam);
                    DrawCurveBezier(hdc,x1,y1,x2,y2,x3,y3,x4,y4,RGB(255,0,0));
                    temp=0;
                }
            }
            case WM_LBUTTONDBLCLK:
                if(option=="clippingP"){

                x = LOWORD(lParam);
                y = HIWORD(lParam);
                p2[temp2].x=x;
                p2[temp2].y=y;
                temp2++;
                PolygonClip(hdc,p2,temp2,rect.left+100,rect.top+100,rect.right-100,rect.bottom-100);
                temp2=0;
            }

            break;


            /*

            else if(algo == "flood"){
                x = LOWORD(lParam);
                y = HIWORD(lParam);
                NRFloodFill(hdc,x,y,Dcolor,Fcolor);
            }else if(algo=="clipping"){
                x = LOWORD(lParam);
                y = HIWORD(lParam);
                p2[flag2].x=x;
                p2[flag2].y=y;
                flag2++;
            }
            */
            break;
            /*

            case WM_LBUTTONDBLCLK:
             if(algo=="clippingL"){

                x = LOWORD(lParam);
                y = HIWORD(lParam);
                p2[flag2].x=x;
                p2[flag2].y=y;
                flag2++;
                CohenSuth(hdc,p2,flag2,rect.left+100,rect.top+100,rect.right-100,rect.bottom-100);
                CohenSuth(hdc,xs,ys,xe,ye,xc,yc,x,y);

                flag2=0;
            }
            */
            /*
             if(algo=="clippingP"){

                x = LOWORD(lParam);
                y = HIWORD(lParam);
                p2[flag2].x=x;
                p2[flag2].y=y;
                flag2++;
                PolygonClip(hdc,p2,flag2,rect.left+100,rect.top+100,rect.right-100,rect.bottom-100);
                flag2=0;
            }
            */

            break;

		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}

    return 0;
}


/*
    switch (message)
    {
        case DDA:
                case WM_LBUTTONDOWN:
                     xs=LOWORD(lParam);
                     ys=HIWORD(lParam);
                     break;
                case WM_LBUTTONUP:
                     xe=LOWORD(lParam);
                     ye=HIWORD(lParam);
                     HDC hdc;
                     hdc =GetDC(hwnd);
                     SimpleDDA( hdc, xs, ys, xe, ye,RGB(255,0,0));
                     ReleaseDC(hwnd,hdc);
                     break;
            break;

    case Midpoint:
                     midPoint( hdc, xs, ys, xe, ye,RGB(255,0,0));

                     break;
       case Parametric:
                   parametric_line(hdc,xs,xe,ys,ye,RGB(255,0,0));
                   break;
       case Direct:
                 A=100;
                 B=50;
                 draw_polar_elipse(hdc,xe,ye,B,A,RGB(255,0,0));
                 break;

    case WM_COMMAND:

        switch (wParam)
        {


        case FILE_MENU_EXIT:
                 DestroyWindow(hwnd);
                 break;
        }



        break;
        case WM_CREATE:
            AddMenus(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
*/
void AddMenus(HWND hwnd)
{

HMENU hmenu = CreateMenu();
ADDPOPUPMENU(hmenu, "&Drawing_menu");
ADDMENUITEM(hmenu, Save,"save drawing");
ADDMENUITEM(hmenu, Load,"load drawing");
ADDMENUITEM(hmenu, DDA,"draw line using DDA");
	ADDMENUITEM(hmenu, Midpoint,"drawline using midpoint");
	ADDMENUITEM(hmenu, Parametric, "draw line using parametric");
	ADDMENUITEM(hmenu,Direct , "draw direct Ellipse");

	//ADDMENUITEM(hmenu, NULL, NULL);

	ADDMENUITEM(hmenu, Polar, "draw polar Ellipse");
	ADDMENUITEM(hmenu, Mid, "draw midpoint Ellipse");
	ADDMENUITEM(hmenu, hermite,"drawcurve using hermite");
	ADDMENUITEM(hmenu, bezier, "draw curve using bezier");
    ADDMENUITEM(hmenu,clipping_polygon,"clipping polygon");
	ADDMENUITEM(hmenu, FILE_MENU_EXIT, "Exit menue");
	//ADDMENUITEM(hmenu, ID_Line_Parametric, "&Line: Parametric");

	//ADDMENUITEM(hmenu, NULL, NULL);
	/*

	ADDMENUITEM(hmenu, ID_Circle_Cartesian, "&Circle: Cartesian");
	ADDMENUITEM(hmenu, ID_Circle_Polar, "&Circle: Polar");
	//ADDMENUITEM(hmenu, ID_Circle_IterativePolar, "&Circle: IterativePolar");
	ADDMENUITEM(hmenu, ID_Circle_MidPoint, "&Circle: MidPoint");

	ADDMENUITEM(hmenu, NULL, NULL);

	//ADDMENUITEM(hmenu, ID_Curve_FirstDegree, "&Curve: First Degree");
	//ADDMENUITEM(hmenu, ID_Curve_SecondDegree, "&Curve: Second Degree");
	ADDMENUITEM(hmenu, ID_Curve_ThirdHermit, "&Curve: Hermit");
	ADDMENUITEM(hmenu, ID_Curve_ThirdBezier, "&Curve: Bezier");
	//ADDMENUITEM(hmenu, ID_Curve_Splines, "&Curve: Splines");

	ADDMENUITEM(hmenu, NULL, NULL);

	//ADDMENUITEM(hmenu, ID_Filling, "&Filling: Convex & NonConvex");
	ADDMENUITEM(hmenu, ID_FloodFilling, "&Filling: Flood Fill");

	ADDMENUITEM(hmenu, NULL, NULL);

	ADDMENUITEM(hmenu, ID_ClippingP, "&Clipping: Polygon Clipping Polygon");
    ADDMENUITEM(hmenu, ID_ClippingL, "&Clipping: Polygon Clipping Line");
	ADDMENUITEM(hmenu, NULL, NULL);
	*/
	//ADDMENUITEM(hmenu, File_, "&Exit");

    SetMenu(hwnd, hmenu);
}


/*
//    hMenu =CreateMenu();
    HMENU hFileMenu =CreateMenu();
    HMENU hFileMenu2 =CreateMenu();
    HMENU hFileMenu3 =CreateMenu();
    HMENU hSubMenu =CreateMenu();
    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hFileMenu,"Draw Line");
    AppendMenu(hFileMenu,MF_STRING,DDA,"DDA");
    AppendMenu(hFileMenu,MF_STRING,Midpoint,"Midpoint");
    AppendMenu(hFileMenu,MF_STRING,Parametric,"Parametric");
    AppendMenu(hFileMenu,MF_STRING,FILE_MENU_EXIT,"Exit");

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hFileMenu2,"Draw Ellipse");
    AppendMenu(hFileMenu2,MF_STRING,Direct," Direct");
    AppendMenu(hFileMenu2,MF_STRING,Polar,"  Polar");
    AppendMenu(hFileMenu2,MF_STRING,Mid,"  Midpoint");
    AppendMenu(hFileMenu2,MF_STRING,FILE_MENU_EXIT,"Exit");

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hFileMenu3,"Draw Curve");
    AppendMenu(hFileMenu3,MF_STRING,bezier," bezier");
    AppendMenu(hFileMenu3,MF_STRING,hermite,"  hermite");
    AppendMenu(hFileMenu3,MF_STRING,splines,"  splines");
    AppendMenu(hFileMenu3,MF_STRING,FILE_MENU_EXIT,"Exit");


//#defin
    //AppendMenu(hFileMenu,MF_POPUP, (UINT_PTR)hSubMenu,"Open SubMenu");
    //AppendMenu(hFileMenu,MF_SEPARATOR,NULL,NULL);
    //AppendMenu(hFileMenu,MF_STRING,FILE_MENU_EXIT,"Exit");
    //AppendMenu(hSubMenu,MF_STRING,NULL,"SubMenu Item");

      //  AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hFileMenu,"Curve");
    AppendMenu(hMenu,MF_STRING,NULL,"Help");
    SetMenu(hwnd,hMenu);
}

*/
