// Muhammad Hashmi

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h> // for cos and sins
#define PI 3.14159265358979323846 // for angles 
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 // 1 degree to radians 


float px,py; //player starting position coordinates
float pdx,pdy,pa; // this is delta x,delta y and the angle of the player

void drawPlayer(){
	glColor3f(1,1,0); //yellow color
	glPointSize(10); // player size
	glBegin(GL_POINTS);
	glVertex2i(px,py);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px,py);
	glVertex2i(px+pdx*5,py+pdy*5);
	glEnd();
}

int mapX=8,mapY=8,mapS=64; // the map dimensions;
int map[]=  // here zeros will be free spaces for my player and 1 will be wall or boundary 
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,1,0,1,1,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,1,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D(){
	int x,y,xo,yo;
	for(y=0;y<mapY;y++){
		for(x=0;x<mapX;x++)
		{
		 if(map[y*mapX+x]==1)
		 { 
		 glColor3f(1,1,1); // whie color for walls
		 } 
		 else
		 {
		  glColor3f(0,0,0);// black color for movement
		 }
   				xo=x*mapS; yo=y*mapS;
   				glBegin(GL_QUADS); 
   				glVertex2i( 0+xo+1, 0+yo+1); //these plus 1 and minus 1 are adding boundary line to each tile
  				glVertex2i( 0+xo+1, mapS+yo-1); 
    			glVertex2i( mapS+xo-1, mapS+yo-1);  
    			glVertex2i( mapS+xo-1, 0+yo+1); 
   				 glEnd();
	    }
	}
}

float dist(float ax,float ay, float bx, float by, float ang){
	return ( sqrt ( (bx-ax)*(bx-ax)+ (by-ay)*(by-ay)));
}
void drawRay3D(){
    int r, mx, my, mp, dof; // dof is depth of field
    float rx, ry, ra, xo, yo,disT;
    ra = pa - DR * 30;  // Start 30 rays to the left of player angle
    if(ra < 0){
        ra = ra + 2*PI;
    }
    if (ra > 2*PI){
        ra = ra - 2*PI;
    }
    
    for (r = 0; r < 20; r++){
        // Check horizontal lines of interference 
        dof = 0;
        float disH = 10000, hx = px, hy = py;
        float aTan = -1/tan(ra);
        
        if(ra > PI){ // looking up
            ry = (((int)py >> 6) << 6) - 0.0001;
            rx = (py - ry) * aTan + px;
            yo = -64;
            xo = -yo * aTan;
        }
        if(ra < PI){ // looking down
            ry = (((int)py >> 6) << 6) + 64;
            rx = (py - ry) * aTan + px;
            yo = 64;
            xo = -yo * aTan;
        }
        if(ra == 0 || ra == PI){ // looking straight left or right
            rx = px;
            ry = py;
            dof = 8;	
        }
        
        while(dof < 8){
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            
            if(mp >= 0 && mp < mapX * mapY && map[mp] == 1){ // hitting wall? 
                hx = rx;
                hy = ry;
                disH = dist(px, py, hx, hy, ra);
                dof = 8;
            } else {
                rx = rx + xo;
                ry = ry + yo;
                dof = dof + 1;
            }
        }
        
        // Check for vertical line interference
        dof = 0;
        float disV = 10000, vx = px, vy = py;
        float nTan = -tan(ra);
        
        if(ra > P2 && ra < P3){ // looking left
            rx = (((int)px >> 6) << 6) - 0.0001;
            ry = (px - rx) * nTan + py;
            xo = -64;
            yo = -xo * nTan;
        }
        if(ra < P2 || ra > P3){ // looking right
            rx = (((int)px >> 6) << 6) + 64;
            ry = (px - rx) * nTan + py;
            xo = 64;
            yo = -xo * nTan;
        }
        if(ra == P2 || ra == P3){ // looking straight up or down
            rx = px;
            ry = py;
            dof = 8;	
        }
        
        while(dof < 8){
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            
            if(mp >= 0 && mp < mapX * mapY && map[mp] == 1){ // hitting wall? 
                vx = rx;
                vy = ry;
                disV = dist(px, py, vx, vy, ra);
                dof = 8;
            } else {
                rx = rx + xo;
                ry = ry + yo;
                dof = dof + 1;
            }
        }
        
        // Choose the closest intersection
        if(disV < disH){
            rx = vx;
            ry = vy;
            disT=disV;
        }
        if(disH < disV){
            rx = hx;
            ry = hy;
            disT=disH;
        }
        
        // Draw each ray inside the loop
        glColor3f(1, 0, 0);
        glLineWidth(3);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();
        
        // First Person FOV - FIXED PART
        float ca=pa-ra;
        if(ca < 0){
            ca = ca + 2*PI;
        }
        if (ca > 2*PI){
            ca = ca - 2*PI;
        }
        disT=disT*cos(ca);// to fix the warping effect 
        float lineH=(mapS*320)/disT;    // wall height in first person
        if (lineH>320){
            lineH=320;
        }
        float lineO=160-lineH/2; // bringing walls to center of screen , setting the offcenter
        
        // Set wall color (red for walls)
        glColor3f(1,0,0);
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2i(r*8 + 530, lineO);  // Start from calculated offset
        glVertex2i(r*8 + 530, lineO + lineH);  // End at offset + height
        glEnd();
        
        // Increment angle for next ray
        ra = ra + 2*DR;
        if(ra < 0){
            ra = ra + 2*PI;
        }
        if (ra > 2*PI){
            ra = ra - 2*PI;
        }
    }
}
void display()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 drawMap2D();
 drawPlayer();
 drawRay3D();
 glutSwapBuffers();
 
}
void buttons(unsigned char key, int x, int y){
	// all these keys are recording my movements to make my player move 
	if(key=='a'){
		pa=pa-0.1; // we are now moving with respect to certain angles no fixed pixels 
		// pa represents our angle
		if(pa<0){
			pa=pa+2*PI;
		}
		pdx=cos(pa)*5;
		pdy=sin(pa)*5;
	}
		if(key=='d'){
		pa=pa+0.1; // we are now moving with respect to certain angles no fixed pixels
		if(pa>2*PI){
			pa=pa-2*PI;
		}
		pdx=cos(pa)*5;
		pdy=sin(pa)*5;
	}
		if(key=='s'){
		px=px-pdx;py=py-pdy;
	}
		if(key=='w'){
		px=px+pdx;py=py+pdy;
	}
	glutPostRedisplay();
}
void init(){
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,1024,512,0);
	px=300,py=300;
	pdx=cos(pa)*5;
	pdy=sin(pa)*5;
}
int main(int argc, char** argv)
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 glutInitWindowSize(1024,512);
 glutCreateWindow("My Raycaster");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(buttons); // calling the button function here in glut keyboard
 glutMainLoop();
}
