//
//  main.cpp
//  ECS 175
//
//  Created by Nan Chen on 10/10/19.
//  Copyright © 2019 Nan Chen. All rights reserved.
//

#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
//other includes

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <map>

/****set in main()****/
//the number of pixels in the grid
int grid_width;
int grid_height;

//the size of pixels sets the inital window height and width
//don't make the pixels too large or the screen size will be larger than
//your display size
float pixel_size;

/*Window information*/
int win_height;
int win_width;
double x;   // x-axis
double y;   // y-axis
int numOfPoints;
int numOfPoly;
std::map<int, std::vector<std::pair<int, int>>> toReturn;
std::vector<std::pair<int, int>> pix;

//GL display functions
void init();
void idle();
void displayDDA();
void displayBresenham();
void displayDDAR();//resterize
void displayBresenhamR();//resterize
void draw_pix(int x, int y);
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();

void linePixX(int pi, std::pair<double, double> p_0, std::pair<double, double> p, double delX, double delY, std::vector<std::pair<int, int>> &toReturn);
void linePixY_plus(int pi, std::pair<double, double> p_0, std::pair<double, double> p, double delX, double delY, std::vector<std::pair<int, int>> &toReturn);
void linePixY_minus(int pi, std::pair<double, double> p_0, std::pair<double, double> p, double delX, double delY, std::vector<std::pair<int, int>> &toReturn);
void linePixX_minus(int pi, std::pair<double, double> p_0, std::pair<double, double> p, double delX, double delY, std::vector<std::pair<int, int>> &toReturn);
void DDA(std::map<int, std::vector<std::pair<int, int>>> &toReturn);
void BresenHam(std::map<int, std::vector<std::pair<int, int>>> &toReturn);
void drawShape(std::vector<std::pair<int, int>> toReturn);
bool compara(std::pair<int, int> &p0, std::pair<int, int> &p);
bool comparator(std::pair<int, int> &p0, std::pair<int, int> &p);
void resterization(std::map<int, std::vector<std::pair<int, int>>> &toReturn);


//class defination
class Points {
public:
    Points(int num) {numOfPoints = num;}
    std::pair<double, double> getPoint(int index);
    void setCoordinates(std::pair<double, double> pix);
//    void updateCoordinates(int index, double newx,double newy);
    int getNumOfPoints() {return numOfPoints;}
private:
    int numOfPoints;
    std::vector<std::pair<double, double>> pixels;
};

class Poly {
public:
    Poly(int num) {numOfPolys = num;}
    Points getPoly(int index);
    void setList(Points id);
//    void updateList(int index,std::vector<std::pair<double, double>> newPoly);
    int getNumOfPolys() {return numOfPolys;}
//    Poly operator = (Poly const &obj){
private:
    int numOfPolys;
    std::vector<Points> polyList;
};

Poly Buffer(numOfPoly);
Poly newBuffer(numOfPoly);

std::pair<double, double> Points::getPoint(int index) {
    return pixels[index];
}

void Points::setCoordinates(std::pair<double, double> pix) {
    pixels.push_back(pix);
}

Points Poly::getPoly(int index) {
    return polyList[index];
}
void Poly::setList(Points id) {
    polyList.push_back(id);
}
//class defination ends

void translate(int index, double tx,double ty) {
    int count = Buffer.getPoly(index).getNumOfPoints();
    std::pair<double, double> newPix;
    int m = Buffer.getPoly(index).getNumOfPoints();
    Points newPoly(m);
    for (int i = 0; i < count; i++) {
        std::pair<double,double> target = Buffer.getPoly(index).getPoint(i);
        double x = target.first;
        double y = target.second;
        double newx = x + tx;
        double newy = y + ty;
        newPix = std::make_pair(newx, newy);
//        std::cout << x << " and " << y << std::endl;
        newPoly.setCoordinates(newPix);
//        std::cout << newx << " and " << newy << std::endl;
    }
    std::ifstream infile;
    infile.open("test_scene");
    infile >> numOfPoly;
    for (int i = 0; i < numOfPoly; i++) {
        infile >> numOfPoints;
        Points P(numOfPoints);
        for (int j = 0; j < numOfPoints; j++) {
            infile >> x >> y;
            P.setCoordinates(std::make_pair(x, y));
        }
        if (i == index) {
            newBuffer.setList(newPoly);
        } else {
            newBuffer.setList(P);
        }
    }
    infile.close();
    std::cout << "translate done" << std::endl;
    return;
}

void rotate(int index, double px, double py, double angle) {
    int count = Buffer.getPoly(index).getNumOfPoints();
    std::pair<double, double> newPix;
    int m = Buffer.getPoly(index).getNumOfPoints();
    Points newPoly(m);
    for (int i = 0; i < count; i++) {
        std::pair<double,double> target = Buffer.getPoly(index).getPoint(i);
        double x = target.first;
        double y = target.second;
        double newx = px + (x-px)*cos(angle) - (y-py)*sin(angle);
        double newy = py + (x-px)*sin(angle) + (y-py)*cos(angle);
        newPix = std::make_pair(newx, newy);
        newPoly.setCoordinates(newPix);
    }
    for (int j = 0; j < Buffer.getNumOfPolys(); j++) {
        if (j != index) {
            newBuffer.setList(Buffer.getPoly(j));
        } else {
            newBuffer.setList(newPoly);
        }
    }
    std::ifstream infile;
    infile.open("test_scene");
    infile >> numOfPoly;
    for (int i = 0; i < numOfPoly; i++) {
        infile >> numOfPoints;
        Points P(numOfPoints);
        for (int j = 0; j < numOfPoints; j++) {
            infile >> x >> y;
            P.setCoordinates(std::make_pair(x, y));
        }
        if (i == index) {
            newBuffer.setList(newPoly);
        } else {
            newBuffer.setList(P);
        }
    }
    infile.close();
    std::cout << "rotation done" << std::endl;
    return;
}

void scale(int index, double fx, double fy, double sx, double sy) {
    int count = Buffer.getPoly(index).getNumOfPoints();
    std::pair<double, double> newPix;
    int m = Buffer.getPoly(index).getNumOfPoints();
    Points newPoly(m);
    for (int i = 0; i < count; i++) {
        std::pair<double,double> target = Buffer.getPoly(index).getPoint(i);
        double x = target.first;
        double y = target.second;
        double newx = x*sx + fx*(1-sx);
        double newy = y*sy + fy*(1-sy);
        newPix = std::make_pair(newx, newy);
        newPoly.setCoordinates(newPix);
    }
    for (int j = 0; j < Buffer.getNumOfPolys(); j++) {
        if (j != index) {
            newBuffer.setList(Buffer.getPoly(j));
        } else {
            newBuffer.setList(newPoly);
        }
    }
    std::ifstream infile;
    infile.open("test_scene");
    infile >> numOfPoly;
    for (int i = 0; i < numOfPoly; i++) {
        infile >> numOfPoints;
        Points P(numOfPoints);
        for (int j = 0; j < numOfPoints; j++) {
            infile >> x >> y;
            P.setCoordinates(std::make_pair(x, y));
        }
        if (i == index) {
            newBuffer.setList(newPoly);
        } else {
            newBuffer.setList(P);
        }
    }
    infile.close();
    std::cout << "scaling done" << std::endl;
    return;
}

void writeFile(Poly newBuffer){
    std::fstream out;
    out.open("new_data",std::ios::out);
    out << newBuffer.Poly::getNumOfPolys() << std::endl;
    out << std::endl;
    for (int i = 0; i < newBuffer.Poly::getNumOfPolys(); i++) {
        out << newBuffer.Poly::getPoly(i).getNumOfPoints() << std::endl;
        for (int j = 0; j < newBuffer.Poly::getPoly(i).getNumOfPoints(); j++) {
            out << newBuffer.Poly::getPoly(i).getPoint(j).first << " " << newBuffer.Poly::getPoly(i).getPoint(j).second << std::endl;
        }
        out << std::endl;
    }
    out.close();
}

void staySame(){
    std::ifstream infile;
    infile.open("test_scene");
    infile >> numOfPoly;
    for (int i = 0; i < numOfPoly; i++) {
        infile >> numOfPoints;
        Points P(numOfPoints);
        for (int j = 0; j < numOfPoints; j++) {
            infile >> x >> y;
            P.setCoordinates(std::make_pair(x, y));
        }
        newBuffer.setList(P);
    }
    infile.close();
    return;
}

int main(int argc, char **argv)
{
    //the number of pixels in the grid
    grid_width = 100;
    grid_height = 100;
    
    //the size of pixels sets the inital window height and width
    //don't make the pixels too large or the screen size will be larger than
    //your display size
    pixel_size = 5;
    
    /*Window information*/
    win_height = grid_height*pixel_size;
    win_width = grid_width*pixel_size;
    
    //read in file data and store in lists
//    std::string input;
//    std::cout << "Enter the name of the file you want to read(defaultly test_scene)" << std::endl;
//    std::cin >> input;
    std::ifstream infile;
    infile.open("test_scene");
    if (!infile) {
        std::cout << "file was not able to be opened" << std::endl;
        exit(1);
    }
    Poly Buffer(numOfPoly);
    infile >> numOfPoly;
    for (int i = 0; i < numOfPoly; i++) {
        infile >> numOfPoints;
        Points P(numOfPoints);
        for (int j = 0; j < numOfPoints; j++) {
            infile >> x >> y;
            P.setCoordinates(std::make_pair(x, y));
        }
        Buffer.setList(P);
    }
    infile.close();
//    std::cout << Buffer.Poly::getNumOfPolys() << std::endl;
    //operating all transformations
    char op;
    int index;
    double tx,ty,px,py,angle,fx,fy,sx,sy;
//    float xmin,xmax,ymin,ymax;
    while (true) {
        std::cout << "Enter the option you want to choose" << std::endl;
        std::cout << "1>translation" << std::endl;
        std::cout << "2>rotation" << std::endl;
        std::cout << "3>scaling" << std::endl;
    //  std::cout << "4>clipping" << std::endl;
        std::cout << "0>done transforming" << std::endl;
        std::cin >> op;
        if (op == '1') {
            std::cout << "Enter the polygon you want to translate (start from 0)" << std::endl;
            std::cin >> index;
            std::cout << "Enter your translation vector" << std::endl;
            std::cin >> tx >> ty;
            translate(index,tx,ty);
            std::cout << "success!" << std::endl;
            continue;
        } else if (op == '2'){
            std::cout << "Enter the polygon you want to rotate (start from 0)" << std::endl;
            std::cin >> index;
            std::cout << "Enter the pivot point coordinates" << std::endl;
            std::cin >> px >> py;
            std::cout << "Enter the angle of rotation" << std::endl;
            std::cin >> angle;
            rotate(index,px,py,angle);
            std::cout << "success!" << std::endl;
            continue;
        } else if (op == '3'){
            std::cout << "Enter the polygon you want to scale (start from 0)" << std::endl;
            std::cin >> index;
            std::cout << "Enter the fix point coordinates" << std::endl;
            std::cin >> fx >> fy;
            std::cout << "Enter your scaling factors along x and y axis respectively" << std::endl;
            std::cin >> sx >> sy;
            scale(index, fx, fy, sx, sy);
            std::cout << "success!" << std::endl;
            continue;
        }  else if (op == '0'){
            staySame();
            break;
        } else {
            std::cout << "Invalid input, try again" << std::endl;
            continue;
        }
    }
    writeFile (newBuffer);
    
    /*Set up glut functions*/
    /** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/
    
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /*initialize variables, allocate memory, create buffers, etc. */
    //create window of size (win_width x win_height
    glutInitWindowSize(win_width,win_height);
    //windown title is "Project 1"
    glutCreateWindow("Project 1");

    /*defined glut callback functions*/
    //glutDisplayFunc(display); rendering calls here
    while (true) {
        std::cout << "How would you like to draw shapes? Enter 1>dda 2>bresnham" << std::endl;
        std::cin >> op;
        if (op == '1') {
            while (true) {
                std::cout << "Would you like to resterize these shapes? Yes(y) or No(n)" << std::endl;
                std::cin >> op;
                if (op == 'y') {
                    glutDisplayFunc(displayDDAR);
                    break;
                } else if (op == 'n') {
                    glutDisplayFunc(displayDDA);
                    break;
                } else {
                    std::cout << "Invalid input, try again" << std::endl;
                    continue;
                }
            }
        } else if (op == '2') {
            while (true) {
                std::cout << "Would you like to resterize these shapes? Yes(y) or No(n)" << std::endl;
                std::cin >> op;
                if (op == 'y') {
                    glutDisplayFunc(displayBresenhamR);
                    break;
                } else if (op == 'n') {
                    glutDisplayFunc(displayBresenham);
                    break;
                } else {
                    std::cout << "Invalid input, try again" << std::endl;
                    continue;
                }
            }
        } else {
            std::cout << "Invalid input, try again" << std::endl;
            continue;
        }
        break;
    }
//    glutDisplayFunc(displayDDAR);
    glutReshapeFunc(reshape); //update GL on window size change
    glutMouseFunc(mouse);     //mouse button events
    glutMotionFunc(motion);   //mouse movement events
    glutKeyboardFunc(key);    //Keyboard events
    glutIdleFunc(idle);
    
    //initialize opengl variables
    init();
    //start glut event loop
    glutMainLoop();
    return 0;
}

/*initialize gl stufff*/
void init()
{
    //set clear color (Default background to white)
    glClearColor(1.0,1.0,1.0,1.0);
    //checks for OpenGL errors
    check();
}

//called repeatedly when glut isn't doing anything else
void idle()
{
    //redraw the scene over and over again
    glutPostRedisplay();
}

//this is where we render the screen
void displayDDA() {
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();
    //draws every other pixel on the screen
    DDA(toReturn);
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
}

void displayDDAR() {
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();
    //draws every other pixel on the screen
    DDA(toReturn);
    resterization(toReturn);
    glutSwapBuffers();
    check();
}

void displayBresenham() {
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();
    //draws every other pixel on the screen
    BresenHam(toReturn);
    glutSwapBuffers();
    check();
    //blits the current opengl framebuffer on the screen
    //checks for opengl errors
}

void displayBresenhamR()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();
    //draws every other pixel on the screen
    BresenHam(toReturn);
    resterization(toReturn);
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
}

//Draws a single "pixel" given the current grid size
//don't change anything in this for project 1
void draw_pix(int x, int y){
    glBegin(GL_POINTS);
    glColor3f(.2,.2,1.0);
    glVertex3f(x+.5,y+.5,0);
    glEnd();
}

/*Gets called when display size changes, including initial craetion of the display*/
void reshape(int width, int height)
{
    /*set up projection matrix to define the view port*/
    //update the ne window width and height
    win_width = width;
    win_height = height;
    
    //creates a rendering area across the window
    glViewport(0,0,width,height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,grid_width,0,grid_height,-10,10);
    
    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    pixel_size = width/(float)grid_width;
    
    //set pixel size relative to the grid cell size
    glPointSize(pixel_size);
    //check for opengl errors
    check();
}

//gets called when a key is pressed on the keyboard
void key(unsigned char ch, int x, int y)
{
    switch(ch)
    {
        case 'a':
            break;
        case 'd':
            break;
        default:
            //prints out which key the user hit
            printf("User hit the \"%c\" key\n",ch);
            break;
    }
    //redraw the scene after keyboard input
    glutPostRedisplay();
}


//gets called when a mouse button is pressed
void mouse(int button, int state, int x, int y)
{
    //print the pixel location, and the grid location
    printf ("MOUSE AT PIXEL: %d %d, GRID: %d %d\n",x,y,(int)(x/pixel_size),(int)((win_height-y)/pixel_size));
    switch(button)
    {
        case GLUT_LEFT_BUTTON: //left button
//            DDA(toReturn);
            printf("LEFT ");
            break;
        case GLUT_RIGHT_BUTTON: //right button
            printf("RIGHT ");
        default:
            printf("UNKNOWN "); //any other mouse button
            break;
    }
    if(state !=GLUT_DOWN)  //button released
        printf("BUTTON UP\n");
    else
        printf("BUTTON DOWN\n");  //button clicked
    
    //redraw the scene after mouse click
    glutPostRedisplay();
}

//gets called when the curser moves accross the scene
void motion(int x, int y)
{
    //redraw the scene after mouse movement
    glutPostRedisplay();
}

//checks for any opengl errors in the previous calls and
//outputs if they are present
void check()
{
    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
    {
        printf("GLERROR: There was an error %s\n",gluErrorString(err) );
        exit(1);
    }
}

//***********************************************************//
//GLUT function completed
//***********************************************************//

// 0 < m < 1
void linePixX(int pi, std::pair<double, double> p1, std::pair<double, double> p2, double delX, double delY, std::vector<std::pair<int, int>> &toReturn) {
    //base case
    if (round(p1.first) == round(p2.first)) {
        return;
    }
    toReturn.push_back(std::make_pair(round(p1.first), round(p1.second)));
    if (pi >= 0) {
        p1 = std::make_pair(p1.first + 1, p1.second + 1);
        pi += 2*delY - 2*delX;
        linePixX(pi, p1, p2, delX, delY, toReturn);
    } else {
        p1 = std::make_pair(p1.first + 1, p1.second);
        pi += 2*delY;
        linePixX(pi, p1, p2, delX, delY, toReturn);
    }
}

// -1 < m < 0
void linePixX_minus(int pi, std::pair<double, double> p1, std::pair<double, double> p2, double delX, double delY, std::vector<std::pair<int, int>> &toReturn) {
    //base case
    if (round(p1.first) == round(p2.first)) {
        return;
    }
    toReturn.push_back(std::make_pair(round(p1.first), round(p1.second)));
    if (pi >= 0) {
        p1 = std::make_pair(p1.first + 1, p1.second - 1);
        pi += 2*delY + 2*delX;
        linePixX(pi, p1, p2, delX, delY, toReturn);
    } else {
        p1 = std::make_pair(p1.first + 1, p1.second);
        pi += 2*delY;
        linePixX(pi, p1, p2, delX, delY, toReturn);
    }
}

void linePixY_plus(int pi, std::pair<double, double> p1, std::pair<double, double> p2, double delX, double delY, std::vector<std::pair<int, int>> &toReturn) {
    //base case
    if (round(p1.second) == round(p2.second)) {
        return;
    }
    toReturn.push_back(std::make_pair(round(p1.first), round(p1.second)));
    if (pi >= 0) {
        p1 = std::make_pair(p1.first + 1, p1.second + 1);
        pi += 2*delX - 2*delY;
        linePixY_plus(pi, p1, p2, delX, delY, toReturn);
    } else {
        p1 = std::make_pair(p1.first, p1.second + 1);
        pi += 2*delX;
        linePixY_plus(pi, p1, p2, delX, delY, toReturn);
    }
}

void linePixY_minus(int pi, std::pair<double, double> p1, std::pair<double, double> p2, double delX, double delY, std::vector<std::pair<int, int>> &toReturn) {
    //base case
    if (round(p1.second) == round(p2.second)) {
        return;
    }
    toReturn.push_back(std::make_pair(round(p1.first), round(p1.second)));
    if (pi >= 0) {
        p1 = std::make_pair(p1.first + 1, p1.second - 1);
        pi += 2*delX + 2*delY;
        linePixY_minus(pi, p1, p2, delX, delY, toReturn);
    } else {
        p1 = std::make_pair(p1.first, p1.second - 1);
        pi += 2*delX;
        linePixY_minus(pi, p1, p2, delX, delY, toReturn);
    }
}

void DDA(std::map<int, std::vector<std::pair<int, int>>> &toReturn) {
    for (int i = 0; i < numOfPoly; i++) {
        std::vector<std::pair<int, int>> list;
        for (int j = 0; j < newBuffer.getPoly(i).getNumOfPoints(); j++) {
            std::pair<double, double> p;
            if (j == newBuffer.getPoly(i).getNumOfPoints() - 1) {
                p = newBuffer.getPoly(i).getPoint(0);
            } else {
                p = newBuffer.getPoly(i).getPoint(j+1);
            }
            std::pair<double, double> p_new = newBuffer.getPoly(i).getPoint(j);
            double delX = p.first - p_new.first;
            if (delX < 0) {
                delX = -1 * delX;
                std::pair<double, double> temp;
                temp = p;
                p = p_new;
                p_new = temp;
            }
            if (round(p.first) == round(p_new.first)) {
                int num0 = 0, num = 0;
                if (round(p.second) > round(p_new.second)) {
                    num0 = round(p_new.second);
                    num = round(p.second);
                } else if (round(p.second) < round(p_new.second)) {
                    num = round(p_new.second);
                    num0 = round(p.second);
                }
                int point = num0;
                while (point <= num) {
                    list.push_back(std::make_pair(round(p_new.first), point));
                    point++;
                }
                continue;
            }
            double slope = (p.second - p_new.second) / (p.first - p_new.first);
            if (slope <= 1 && slope >= -1) {
                for (int k = round(p_new.first); k <= round(p.first); k++) {
                    double y = p_new.second + slope * (k - p_new.first);
                    list.push_back(std::make_pair(k, round(y)));
                }
            } else {
                if (round(p.second) > round(p_new.second)) {
                    for (int k = round(p_new.second); k <= round(p.second); k++) {
                        double x = p_new.first + 1.0 / slope * (k - p_new.second);
                        list.push_back(std::make_pair(round(x), k));
                    }
                } else if (round(p.second) < round(p_new.second)) {
                    for (int k = round(p_new.second); k >= round(p.second); k--) {
                        double x = p_new.first + 1.0 / slope * (k - p_new.second);
                        list.push_back(std::make_pair(round(x), k));
                    }
                }
            }
        }
        toReturn[i] = list;
        drawShape(list);
    }
}

void BresenHam(std::map<int, std::vector<std::pair<int, int>>> &toReturn) {
    for (int i = 0; i < numOfPoly; i++) {
        std::vector<std::pair<int, int>> list;
        for (int j = 0; j < newBuffer.getPoly(i).getNumOfPoints(); j++) {
            std::pair<double, double> p;
            if (j == newBuffer.getPoly(i).getNumOfPoints() - 1) {
                p = newBuffer.getPoly(i).getPoint(0);
            } else {
                p = newBuffer.getPoly(i).getPoint(j+1);
            }
            std::pair<double, double> p_new = newBuffer.getPoly(i).getPoint(j);
            double delX = p.first - p_new.first;
            double delY = p.second - p_new.second;
            if (delX < 0) {
                delX = -1 * delX;
                delY = -1 * delY;
                std::pair<double, double> temp;
                temp = p;
                p = p_new;
                p_new = temp;
            }
            if (delY == 0) {
                int point = round(p_new.first);
                while (point <= round(p.first)) {
                    list.push_back(std::make_pair(point, round(p_new.second)));
                    point++;
                }
            }
            if (delX == 0) {
                int num0 = 0, num = 0;
                if (round(p.second) > round(p_new.second)) {
                    num0 = round(p_new.second);
                    num = round(p.second);
                } else if (round(p.second) < round(p_new.second)) {
                    num = round(p_new.second);
                    num0 = round(p.second);
                }
                int point = num0;
                while (point <= num) {
                    list.push_back(std::make_pair(round(p_new.first), point));
                    point++;
                }
                continue;
            }
            double slope = delY / delX;
            if ((slope < 1 && slope > 0)) {
                double pi = 2 * delY - delX;
                linePixX(pi, p_new, p, delX, delY, list);
            } else if (slope < 0 && slope > -1) {
                double pi = 2 * delY - delX;
                linePixX_minus(pi, p_new, p, delX, delY, list);
            } else if (slope > 1) {
                if (delX == 0) {
                    for (int i = round(p_new.second); i <= round(p.second); i++) {
                        list.push_back(std::make_pair(round(p_new.first), i));
                    }
                    continue;
                }
                double pi = 2 * delX - delY;
                linePixY_plus(pi, p_new, p, delX, delY, list);
            } else if (slope < -1) {
                if (delX == 0) {
                    for (int i = round(p_new.second); i >= round(p.second); i--) {
                        list.push_back(std::make_pair(round(p_new.first), i));
                    }
                    continue;
                }
                double pi = 2 * delX - delY;
                linePixY_minus(pi, p_new, p, delX, delY, list);
            } else if (slope == 1) {
                for (int i = round(p_new.first), j = round(p_new.second); i <= round(p.first); i++, j++) {
                    list.push_back(std::make_pair(i, j));
                }
            } else if (slope == -1) {
                for (int i = round(p_new.first), j = round(p_new.second); i <= round(p.first); i++, j--) {
                    list.push_back(std::make_pair(i, j));
                }
            }
        }
        toReturn[i] = list;
        drawShape(toReturn[i]);
    }
}

void drawShape(std::vector<std::pair<int, int>> toReturn) {
    for (std::pair<int, int> t : toReturn) {
        if (t.first >= grid_width) {
            if (t.second >= grid_height) {
                draw_pix(grid_width, grid_height);
                continue;
            }
            if (t.second < 0) {
                draw_pix(grid_width, 0);
                continue;
            }
            draw_pix(grid_width, t.second);
            continue;
        }
        if (t.second >= grid_height) {
            if (t.first < 0) {
                draw_pix(0, grid_height);
                continue;
            }
            draw_pix(t.first, grid_height);
            continue;
        }
        if (t.first < 0) {
            if (t.second < 0) {
                draw_pix(0, 0);
                continue;
            }
            draw_pix(0, t.second);
            continue;
        }
        if (t.second < 0) {
            draw_pix(t.first, 0);
            continue;
        }
        draw_pix(t.first, t.second);
    }
}

bool comparator(std::pair<int, int> &p0, std::pair<int, int> &p) {
    return (p0.second > p.second);
}

bool compara(std::pair<int, int> &p0, std::pair<int, int> &p) {
    if (p0.second == p.second) {
        return p0.first < p.first;
    } else {
        return p0.second > p.second;
    }
}

void resterization(std::map<int, std::vector<std::pair<int, int>>> &toReturn) {
//    std::vector<std::pair<int, int>> pix;
    for (int i = 0; i < toReturn.size(); i++) {
        std::vector<std::pair<int, int>> rester = toReturn[i];
        std::sort(rester.begin(), rester.end(), comparator);
        std::sort(rester.begin(), rester.end(), compara);
        for (int t = 0; t <= rester.size() - 1; t++) {
            if (rester[t].second == rester[t+1].second) {
                for (int k = rester[t].first + 1; k <= rester[t+1].first - 1; k++) {
                    toReturn[i].push_back(std::make_pair(k, rester[t].second));
                }
            } else {
                continue;
            }
        }
        drawShape(toReturn[i]);
    }
}
