/* walk.c -- walk around in a room. */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
// include glm header
#include "glm.h"
double locx=150.0, locy=170.0, locz=9.0;		//location of viewer
double orient=4;				//orientation of viewer
double upDown=0.0;
double leftRight=0.0;
int winwidth=1100, winheight=700;		//initial window size
const double PI=3.14159;
double MAXROT;
const double MAXSTEP=2;	//maximum step, turn
const double FOV=75.0;
const int skyBoxSize = 600;
// declare a pointer for the model
GLMmodel* pmodel = NULL;
GLMmodel* tmodel = NULL;
static GLubyte fireTexture[96][96][4];
static GLubyte frontSkyBox[600][600][4];
static GLubyte topSkyBox[600][600][4];
static GLubyte rightSkyBox[600][600][4];
static GLubyte leftSkyBox[600][600][4];
static GLubyte backSkyBox[600][600][4];
static GLuint fire;
static GLuint front;
static GLuint back;
static GLuint top;
static GLuint left;
static GLuint right;
int numSpheres = 15;
int spheres[15][5];
GLUquadricObj *qsphere;
// declare an index for display list
GLint objDisplay;
GLfloat diffuseMaterialRed[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuseMaterialBlue[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuseLight[4] = { 1.0, 1.0, 1.0, 1.0};
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat sphere_mat[] = {1.f, .5f, 0.f, .5f};


void readFire(){ 
    gdImagePtr im; FILE *in; int result; int i, j, x, y, maxX, maxY, c;
    in = fopen("fire.png", "rb");
    im = gdImageCreateFromPng(in);
    if (!im) { printf("Cannot open file\n"); exit(1);}
    maxX = gdImageSX(im); maxY = gdImageSY(im);
    for (y=0; y< maxY; y++){
        for (x=0; x < maxX; x++){
            c = gdImageGetPixel(im, x, y);
            fireTexture[maxY-y-1][x][0] = gdImageRed(im, c);
            fireTexture[maxY-y-1][x][1] = gdImageGreen(im, c);
            fireTexture[maxY-y-1][x][2] = gdImageBlue(im, c);
            fireTexture[maxY-y-1][x][3] = 255;
        }
    }
}
void readFront(){ 
    gdImagePtr im; FILE *in; int result; int i, j, x, y, maxX, maxY, c;
    in = fopen("front55.png", "rb");
    im = gdImageCreateFromPng(in);
    if (!im) { printf("Cannot open file\n"); exit(1);}
    maxX = gdImageSX(im); maxY = gdImageSY(im);
    for (y=0; y< maxY; y++){
        for (x=0; x < maxX; x++){
            c = gdImageGetPixel(im, x, y);
            frontSkyBox[maxY-y-1][x][0] = gdImageRed(im, c);
            frontSkyBox[maxY-y-1][x][1] = gdImageGreen(im, c);
            frontSkyBox[maxY-y-1][x][2] = gdImageBlue(im, c);
            frontSkyBox[maxY-y-1][x][3] = 255;
        }
    }
}
void readTop(){ 
    gdImagePtr im; FILE *in; int result; int i, j, x, y, maxX, maxY, c;
    in = fopen("top552.png", "rb");
    im = gdImageCreateFromPng(in);
    if (!im) { printf("Cannot open file\n"); exit(1);}
    maxX = gdImageSX(im); maxY = gdImageSY(im);
    for (y=0; y< maxY; y++){
        for (x=0; x < maxX; x++){
            c = gdImageGetPixel(im, x, y);
            topSkyBox[maxY-y-1][x][0] = gdImageRed(im, c);
            topSkyBox[maxY-y-1][x][1] = gdImageGreen(im, c);
            topSkyBox[maxY-y-1][x][2] = gdImageBlue(im, c);
            topSkyBox[maxY-y-1][x][3] = 255;
        }
    }
}
void readBack(){ 
    gdImagePtr im; FILE *in; int result; int i, j, x, y, maxX, maxY, c;
    in = fopen("back55.png", "rb");
    im = gdImageCreateFromPng(in);
    if (!im) { printf("Cannot open file\n"); exit(1);}
    maxX = gdImageSX(im); maxY = gdImageSY(im);
    for (y=0; y< maxY; y++){
        for (x=0; x < maxX; x++){
            c = gdImageGetPixel(im, x, y);
            backSkyBox[maxY-y-1][x][0] = gdImageRed(im, c);
            backSkyBox[maxY-y-1][x][1] = gdImageGreen(im, c);
            backSkyBox[maxY-y-1][x][2] = gdImageBlue(im, c);
            backSkyBox[maxY-y-1][x][3] = 255;
        }
    }
}
void readRight(){ 
    gdImagePtr im; FILE *in; int result; int i, j, x, y, maxX, maxY, c;
    in = fopen("right55.png", "rb");
    im = gdImageCreateFromPng(in);
    if (!im) { printf("Cannot open file\n"); exit(1);}
    maxX = gdImageSX(im); maxY = gdImageSY(im);
    for (y=0; y< maxY; y++){
        for (x=0; x < maxX; x++){
            c = gdImageGetPixel(im, x, y);
            rightSkyBox[maxY-y-1][x][0] = gdImageRed(im, c);
            rightSkyBox[maxY-y-1][x][1] = gdImageGreen(im, c);
            rightSkyBox[maxY-y-1][x][2] = gdImageBlue(im, c);
            rightSkyBox[maxY-y-1][x][3] = 255;
        }
    }
}
void readLeft(){ 
    gdImagePtr im; FILE *in; int result; int i, j, x, y, maxX, maxY, c;
    in = fopen("left55.png", "rb");
    im = gdImageCreateFromPng(in);
    if (!im) { printf("Cannot open file\n"); exit(1);}
    maxX = gdImageSX(im); maxY = gdImageSY(im);
    for (y=0; y< maxY; y++){
        for (x=0; x < maxX; x++){
            c = gdImageGetPixel(im, x, y);
            leftSkyBox[maxY-y-1][x][0] = gdImageRed(im, c);
            leftSkyBox[maxY-y-1][x][1] = gdImageGreen(im, c);
            leftSkyBox[maxY-y-1][x][2] = gdImageBlue(im, c);
            leftSkyBox[maxY-y-1][x][3] = 255;
        }
    }
}

void init(void) { 
	int i;
	for(i=0;i<numSpheres;i++){
		spheres[i][0] = (rand()%600)-300;//XPOS
		spheres[i][1] = (rand()%600)-300;//YPOS
		spheres[i][2] = rand()%400;//ZPOS
		spheres[i][3] = rand()%20;//SIZE
		spheres[i][4] = (rand()%20);//SPEED
	}
   
	MAXROT = 1.0/6.0*PI;
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	pmodel = glmReadOBJ("city.obj");
	if (!pmodel) fprintf(stderr,"Cannot parse vase.obj");
	//glmUnitize(pmodel); // make model to fit in a unit cube
	glmFacetNormals(pmodel); // generate normals - is this needed?
	glmVertexNormals(pmodel, 90.0); // average joining normals - allow for hard edges.
	tmodel = glmReadOBJ("tower.obj");
	glmFacetNormals(tmodel); // generate normals - is this needed?
	glmVertexNormals(tmodel, 90.0); // average joining normals - allow for hard edges.
	readTop();
	readFront();
	readBack();
	readRight();
	readLeft();
	readFire();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &front);
	glGenTextures(1, &top);
	glGenTextures(1, &left);
	glGenTextures(1, &right);
	glGenTextures(1, &back);
	glGenTextures(1, &fire);
	qsphere = gluNewQuadric();
}

void initTexture(void){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//this might need to be moved.
}


void displaySkybox(void){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, top);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skyBoxSize, skyBoxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, topSkyBox);
	initTexture();
	//TOP
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);	
	glVertex3f(-300,-300,320); 
	glTexCoord2f(0.0, 1.0); 
	glVertex3f(-300,300,320);
	glTexCoord2f(1.0, 1.0);	
	glVertex3f(300,300,320); 
	glTexCoord2f(1.0, 0.0);	
	glVertex3f(300,-300,320); 
	glEnd();	

	glBindTexture(GL_TEXTURE_2D, front);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skyBoxSize, skyBoxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, frontSkyBox);
	initTexture();
	//FRONT
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);	glVertex3f(-300,300,-280); 
	glTexCoord2f(0.0, 1.0); glVertex3f(-300,300,320);
	glTexCoord2f(1.0, 1.0);	glVertex3f(300,300,320); 
	glTexCoord2f(1.0, 0.0);	glVertex3f(300,300,-280); 
	glEnd();	


	//BACK
	glBindTexture(GL_TEXTURE_2D, back);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skyBoxSize, skyBoxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, backSkyBox);
	initTexture();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);	glVertex3f(300,-300,-280); 
	glTexCoord2f(0.0, 1.0);	glVertex3f(300,-300,320);
	glTexCoord2f(1.0, 1.0);	glVertex3f(-300,-300,320); 
	glTexCoord2f(1.0, 0.0);	glVertex3f(-300,-300,-280); 
	glEnd();	
	
	//LEFT
	glBindTexture(GL_TEXTURE_2D, left);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skyBoxSize, skyBoxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, leftSkyBox);
	initTexture();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);	glVertex3f(-300,-300,-280); 
	glTexCoord2f(0.0, 1.0); glVertex3f(-300,-300,320);
	glTexCoord2f(1.0, 1.0);	glVertex3f(-300,300,320); 
	glTexCoord2f(1.0, 0.0);	glVertex3f(-300,300,-280); 
	glEnd();	
	
	//RIGHT
	glBindTexture(GL_TEXTURE_2D, right);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skyBoxSize, skyBoxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, rightSkyBox);
	initTexture();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);	glVertex3f(300,300,-280); 
	glTexCoord2f(0.0, 1.0); glVertex3f(300,300,320);
	glTexCoord2f(1.0, 1.0);	glVertex3f(300,-300,320); 
	glTexCoord2f(1.0, 0.0);	glVertex3f(300,-300,-280); 
	glEnd();	
	glDisable(GL_TEXTURE_2D);
	
}
void displayFloor(void){
	//FLOOR	
	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(300,300,1); 
	glVertex3f(300,-300,1);
	glVertex3f(-300,-300,1); 
	glVertex3f(-300,300,1); 
	glEnd();	
}

void displayCity(void){

	glColor3f(0.4, 0.4, 0.4);
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterialRed);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); 
    //glMaterialf(GL_FRONT, GL_SHININESS, 10.0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_COLOR_MATERIAL);      
    //glColorMaterial(GL_FRONT, GL_DIFFUSE);

    //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterialBlue);
    glPushMatrix();
      //glTranslatef(8.0, 5.0, 5.0);
    glScalef(2, 2, 2);
    glmDraw(pmodel, GLM_SMOOTH);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(20, 20, 0);
    glRotatef(90, 1, 1, 0);
    glScalef(20, 20, 20);
    glRotatef(30, 0, 0, 100);
    glmDraw(tmodel, GLM_SMOOTH);
    //glmDraw(tmodel, GLM_SMOOTH|GLM_MATERIAL);
    glPopMatrix();
    	glDisable(GL_COLOR_MATERIAL);      
}
void displayBall(void){
	int i;	

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fire);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, fireTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	for(i=0;i<numSpheres;i++){
		glPushMatrix();
		glTranslatef(spheres[i][0], spheres[i][1], spheres[i][2]);
		glScalef(10.0, 10.0, 10.0);
		glutSolidDodecahedron();
		//glutSolidSphere(spheres[i][3], 50, 50);
		//glutSolidCube(spheres[i][3]);
		glPopMatrix();
	}	
	glDisable(GL_TEXTURE_2D);
	

	glEnable(GL_BLEND);
    	glEnable(GL_COLOR_MATERIAL);      
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 0.0, 0.0, 0.2);
	for(i=0;i<numSpheres;i++){
		glPushMatrix();
		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, sphere_mat);
		glTranslatef(spheres[i][0], spheres[i][1], spheres[i][2]+5);
		gluSphere(qsphere,20, 50, 50);
		glPopMatrix();
	}
    	glDisable(GL_COLOR_MATERIAL);      
	glDisable(GL_BLEND);
}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displaySkybox();
	glEnable(GL_LIGHTING); 
	glEnable(GL_LIGHT0);
	displayBall();
	displayCity();
	glDisable(GL_LIGHT0);
    	glDisable(GL_LIGHTING); 
	displayFloor();
	glutSwapBuffers();
}

void walkView(double orient, double locx, double locy, double locz) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, 1.0*winwidth/winheight, 0.5, 1000.0);
    gluLookAt(locx, locy, locz, locx+sin(orient), locy+cos(orient), locz+.3, .1, .1, .9);
    light_position[0] = locx;
    light_position[1] = locy;
    light_position[2] = locz+2.0;
    glutPostRedisplay();
}
void viewMove(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glutPostRedisplay();
}
void viewLook(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glutPostRedisplay();

}
void motion(int x, int y){
        double dx = x;
        double dw = winwidth;
        orient = ((dx/dw)*10);
        walkView(orient, locx, locy, locz);
}
void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
        case 'w'://go forward
       	locx += 2 * sin(orient); 
	locy += 2 *  cos(orient);
        walkView(orient, locx, locy, locz);
        break;

        case 'a'://go left
        //TEMP
        //locx += 2 *  cos(orient);
        //locy += 2 *  sin(orient);
        walkView(orient, locx, locy, locz);
        break;

        case 's'://go back
        locx -= 2 *  sin(orient);
        locy -= 2 *  cos(orient);
        walkView(orient, locx, locy, locz);
        break;

        case 'd'://go right
        //TEMP
        //locx -= 2 *  cos(orient);
        //locy -= 2 *  sin(orient);
        walkView(orient, locx, locy, locz);
        break;

      case 27:
         exit(0);
        break;
      default:
         break;
   }
}



void Reshape(int w, int h) {
    winwidth=w; winheight=h; 
    glViewport(0, 0, w, h);
    walkView(orient, locx, locy, locz);
}

void mouse(int button, int state, int x, int y) {
    float step;
    if (button == GLUT_LEFT_BUTTON && state==GLUT_UP) {
	orient += MAXROT * (2.0*x/winwidth - 1.0);
	if (orient < -2*PI) orient += 2 * PI;
	if (orient >  2*PI) orient -= 2 * PI;
	step = MAXSTEP * (2.0 * y / winheight - 1.0);
	locx -= step *  sin(orient);
	locy -= step *  cos(orient);
	walkView(orient, locx, locy, locz);
    }
}
void draw(){
	int i;
	int temp;
	int temp2;
	for(i=0;i<numSpheres;i++){
		temp = spheres[i][2];	
		if(spheres[i][2]<-10){
			spheres[i][2] = 400;
			spheres[i][0] = (rand()%600)-300;
			spheres[i][1] = (rand()%600)-300;
			spheres[i][4] = (rand()%20);//SPEED
		}else{
			double speed = spheres[i][4];
			spheres[i][4]=spheres[i][4]+.8;
			speed = speed / 10.0;
			spheres[i][2] = spheres[i][2]-speed;
		}
		temp2 = spheres[i][2];
		if(temp==temp2){//a hack
			spheres[i][2] = 400;
			spheres[i][0] = (rand()%600)-300;
			spheres[i][1] = (rand()%600)-300;
			spheres[i][4] = (rand()%20);//SPEED
		}	
	}
	glutPostRedisplay();

}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (winwidth, winheight); 
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init();
    glutReshapeFunc (Reshape);
    glutDisplayFunc(display);
//glutMouseFunc (mouse);
    glutIdleFunc(draw);
    glutPassiveMotionFunc(motion);
    glutKeyboardFunc(keyboard);


    glutMainLoop();
    return 0;            
}

