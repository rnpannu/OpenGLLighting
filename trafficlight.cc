#include <Angel.h>
#include <iostream>

#include "camera.h"
#include "light.h"

using namespace std;

//----------------------------------------------------------------------------

Light *light[2];
Camera *camera;

void init()
{
  GLint faceColourLoc, modelLoc, viewLoc, projLoc;

  // Load shaders and use the resulting shader program
  GLuint program = InitShader("vshader.glsl", "fshader.glsl");
  glUseProgram(program);
  faceColourLoc = glGetUniformLocation(program, "uFaceColour");
  modelLoc = glGetUniformLocation(program, "uModel");
  viewLoc = glGetUniformLocation(program, "uView");
  projLoc = glGetUniformLocation(program, "uProjection");

  // Initialize the vertex position attribute from the vertex shader
  GLuint loc = glGetAttribLocation( program, "vPosition" );

  light[0] = new Light(loc, faceColourLoc, modelLoc, vec4(0, 0, 0, 0));
  light[1] = new Light(loc, faceColourLoc, modelLoc, vec4(3, 0, 0, 0),
		       0, 0, 45, 1, 1, 2);

  vec4 at(1.5,0,1.5,1);
  vec4 up(0,0,1,0);
  vec4 eye(1.5, -10, 6, 1);
  camera = new Camera(viewLoc, projLoc, eye, at, up, -5, 5, -5, 5, -1, 100);

  
  glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background

  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
}

//----------------------------------------------------------------------------

void display( void )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  camera->sendToShader();
  for (int i = 0; i < 2; i++) {
    light[i]->draw();
  }

  glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
  switch ( key ) {
  case 033:
    exit( EXIT_SUCCESS );
    break;
  case 'a':
    camera->moveEye(-0.2, 0, 0);
    break;
  case 'd':
    camera->moveEye(0.2, 0, 0);
    break;
  case 'w':
    camera->moveEye(0, 0.2, 0.2);
    break;
  case 's':
    camera->moveEye(0, -0.2, -0.2);
    break;
  }
  glutPostRedisplay();
}

void timer(int val)
{
  for (int i = 0; i < 2; i++) {
    light[i]->next_colour();
  }
  glutPostRedisplay();
  glutTimerFunc(3000, timer, 0);
}

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 800);

  // If you are using freeglut, the next two lines will check if 
  // the code is truly 3.2. Otherwise, comment them out
    
  glutInitContextVersion(3, 2);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutCreateWindow("Traffic Light");

  glewInit();

  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutTimerFunc(3000, timer, 0);
  
  glutMainLoop();

  for (int i = 0; i < 2; i++) {
    delete light[i];
  }
  delete camera;
  return 0;
}
