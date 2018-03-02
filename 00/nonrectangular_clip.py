# student : Ming-Wei , Niu
# HW : nonrectangular clip
# output result :  http://imgur.com/a/vItpQ
# Time complexity : O(M*N) 

import sys
from OpenGL.GLUT import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

#data for nonrectangular window
dot_data = np.array([
    [50,200],
    [250,100],
    [400,250],
    [350,400],
    [250,400],
    [150,350],
    ])
edge_NUM = dot_NUM = 6

#data for line
line_data = np.array([
    [[55,234],[422,33]],
    [[123,234],[22,33]],
    [[444,32],[333,11]],
    [[200,200],[350,300]],
    [[333,222],[222,33]],
    [[111,222],[299,399]],
    [[341,256],[777,11]],
    [[76,299],[179,66]],
    [[111,22],[111,267]],
    [[100,450],[400,200]],
    ])
line_NUM = 10

def get_u_v(x1, y1, x2, y2, a1, b1, a2, b2):
    # two line [(x1,y1) -> (x2,y2)] and [(a1,b1) -> (a2,b2)] 
    # cross at (x1+u[x2-x1] , y1+u[y2-y1]) = (a1+v[a2-a1] , b1+v[b2-b1])
    # solve ( x2 - x1 ) * u + ( a1 - a2 ) * v  = a1 - x1
    # with ( y2 - y1 ) * u + ( b1 - b2 ) * v  = b1 - y1
    A1 = x2 - x1
    B1 = a1 - a2
    C1 = a1 - x1 
    A2 = y2 - y1
    B2 = b1 - b2
    C2 = b1 - y1 
    if B2*A1 == B1*A2 :
        #parallel give max
        return (999999,999999)
    u = (B2*C1 - B1*C2)/(B2*A1 - B1*A2)
    v = (A2*C1 - A1*C2)/(A2*B1 - A1*B2)
    return (u, v)

def drawOneLine(x1, y1, x2, y2):
  glBegin(GL_LINES)
  glVertex2f(x1, y1)
  glVertex2f(x2, y2)
  glEnd()

def display():
    glClear(GL_COLOR_BUFFER_BIT)

    glColor3f(0.0, 1.0, 1.0)
    #draw clip window boundary
    for i in range(0, dot_NUM):
        drawOneLine (dot_data[i][0], dot_data[i][1], dot_data[(i+1)%dot_NUM][0], dot_data[(i+1)%dot_NUM][1])
    glColor3f(1.0, 1.0, 0.0)
    #show all line
    for i in range(0, line_NUM):
        drawOneLine (line_data[i][0][0], line_data[i][0][1], line_data[i][1][0], line_data[i][1][1])

    #draw clipped line
    glColor3f(1.0, 0.0, 0.0)
    u = np.arange(line_NUM*dot_NUM,dtype=float).reshape(line_NUM,dot_NUM)
    v = np.arange(line_NUM*dot_NUM,dtype=float).reshape(line_NUM,dot_NUM)
    for i in range(0, line_NUM):
        x1 = line_data[i][0][0]
        y1 = line_data[i][0][1]
        x2 = line_data[i][1][0]
        y2 = line_data[i][1][1]
        u_max = 999999.9999
        u_min = -999999.9999
        front_will_touch = False
        # if front_inside True means prolong will touch edge
        back_will_touch = False
        # if back_inside True means antidirection prolong will touch edge
        for j in range(0, edge_NUM):
            a1 = dot_data[j][0]
            b1 = dot_data[j][1]
            a2 = dot_data[(j+1)%dot_NUM][0]
            b2 = dot_data[(j+1)%dot_NUM][1]
            (u[i][j],v[i][j]) = get_u_v(x1, y1, x2, y2, a1,b1,a2,b2)
            if 0.0<= v[i][j] and v[i][j]<=1.0:
                if 1.0<=u[i][j] : front_will_touch = True
                if 0.0>=u[i][j] : back_will_touch = True
        #get new x and y
        get_bit = False             
        for j in range(0, edge_NUM):
            if 0.0<= v[i][j] and v[i][j]<=1.0:
                    if front_will_touch :
                        u_max = 1.0
                        if back_will_touch : u_min = 0.0
                        else : 
                            if 0.0<= u[i][j] and u[i][j]<=1.0:
                                u_min = u[i][j]
                    else:
                        if back_will_touch : 
                            u_min = 0.0
                            if 0.0<= u[i][j] and u[i][j]<=1.0:
                                u_max = u[i][j]
                        else :
                            if 0.0<= u[i][j] and u[i][j]<=1.0: 
                                if get_bit : u_max = u[i][j]
                                else : 
                                    u_min = u[i][j]
                                    get_bit =True
        #draw it
        if u_max<=1.0 and u_min >=0.0 :
            new_x1 = x1 + u_min *(x2-x1)
            new_y1 = y1 + u_min *(y2-y1)
            new_x2 = x1 + u_max *(x2-x1)
            new_y2 = y1 + u_max *(y2-y1)
            drawOneLine (new_x1  ,new_y1, new_x2 ,new_y2)

    glDisable (GL_LINE_STIPPLE)
    glFlush ()

def init():
    glutInit(sys.argv)
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
    glutInitWindowSize(500, 500)
    glutInitWindowPosition(50, 50)
    glutCreateWindow(b'clip')
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glShadeModel(GL_FLAT)

def reshape(w, h):
  glViewport(0, 0, w, h)
  glMatrixMode(GL_PROJECTION)
  glLoadIdentity()
  gluOrtho2D(0.0, w, 0.0, h)

def keyboard(key, x, y):
  if key == chr(27):
    sys.exit(0)

def main():
    init()
    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(keyboard)
    glutMainLoop()

#run
main()
