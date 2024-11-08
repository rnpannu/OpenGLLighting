#include <Angel.h>

#include <iostream>
using namespace std;

//----------------------------------------------------------------------------

GLfloat theta_x = 0.0, theta_y = 0.0, theta_z = 0.0;

vec4 pos(0.0, 0.0, 0.0, 1.0);
vec3 scale(1.0, 1.0, 1.0);
GLint faceColourLoc, modelLoc, viewLoc, projLoc, lightPosLoc, ambientLoc, diffuseLoc, specularLoc,
      lusterLoc, normalVecLoc;
GLuint vao[8], buffer[8]; // 2 + (2 * 3) = 8 triangles, 2 triangles/face
bool PP = false;
GLfloat angle = M_PI / 2.0;
GLfloat r = 1.0;

const vec4 point[6] = {
  vec4(-0.5, -0.5, -1.0, 1),
  vec4(0.5, -0.5, -1.0, 1),
  vec4(-0.5, 0.5, -1.0, 1),
  vec4(-0.5, -0.5, 1.0, 1),
  vec4(0.5, -0.5, 1.0, 1),
  vec4(-0.5, 0.5, 1.0, 1)
};

const vec4 colour[5] = {
  vec4(1,0,0,1), // red
  vec4(1,1,0,1), // yellow
  vec4(0,1,0,1), // green
  vec4(0,0,1,1),  // blue
  vec4(0,1,1,1) // cyan
};

const int face[8][3] = {
  {0,1,2},   // bot 
  {3,4,5},   // top face
  {0,1,3},   // face 1 t1
  {1,3,4},   // face 1 t2
  {1,2,5},   // face 2 t1
  {1,4,5},   // ...
  {0,2,3},  
  {2,3,5},
};

vec3 getNormalVec(const const vec4 v1, const vec4 v2, const vec4 v3){
  vec3 diff1 = (v2.x, v2.y, v2.z) - (v1.x, v1.y, v1.z);
  vec3 diff2 = (v3.x, v3.y, v3.z)- (v1.x, v1.y, v1.z);

  vec3 normal = cross(diff1, diff2);
  return normalize(normal);
}

void init() {
  GLuint program = InitShader("vshader.glsl", "fshader.glsl");
  glUseProgram(program);
  
  // Get uniform locations
  faceColourLoc = glGetUniformLocation(program, "uFaceColour");
  modelLoc = glGetUniformLocation(program, "uModel");
  viewLoc = glGetUniformLocation(program, "uView");
  projLoc = glGetUniformLocation(program, "uProjection");
  lightPosLoc = glGetUniformLocation(program, "uLightPos");
  ambientLoc = glGetUniformLocation(program, "uAmbient");
  diffuseLoc = glGetUniformLocation(program, "uDiffuse");
  specularLoc = glGetUniformLocation(program, "uSpecular");
  shininessLoc = glGetUniformLocation(program, "uShininess");
  normalMatrixLoc = glGetUniformLocation(program, "uNormalMatrix");

  // Set lighting parameters
  glUniform3f(lightPosLoc, 1.0, 2.0, 3.0);
  glUniform3f(ambientLoc, 0.3, 0.3, 0.3);
  glUniform3f(diffuseLoc, 0.8, 0.8, 0.8);
  glUniform3f(specularLoc, 1.0, 1.0, 1.0);
  glUniform1f(shininessLoc, 25.0);

  // Get attribute locations
  GLuint loc = glGetAttribLocation(program, "vPosition");
  GLuint normalLoc = glGetAttribLocation(program, "vNormal");

  // Create vertex arrays and buffers
  glGenVertexArrays(8, vao);
  glGenBuffers(8, buffer);
  GLuint normalBuffer[8];
  glGenBuffers(8, normalBuffer);

  // Process each triangle
  for (int i = 0; i < 8; i++) {
    glBindVertexArray(vao[i]);
    
    // Set up vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, buffer[i]);
    vec4 triangleVertices[3];
    for (int j = 0; j < 3; j++) {
      triangleVertices[j] = point[face[i][j]];
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // Calculate and set up normal vector
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
    vec3 normal = getNormalVec(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
    vec3 triangleNormals[3] = {normal, normal, normal}; // Same normal for all vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleNormals), triangleNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  }

  // Set up OpenGL state
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
}


//----------------------------------------------------------------------------

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model and view matrices
    mat4 rotate = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
    mat4 model = Translate(pos) * rotate * Scale(scale);
    vec4 eye(-1, -1, 2, 1);
    vec4 at(0, 0, 0, 0);
    vec4 up(0, 0, 1, 0);
    mat4 view = LookAt(eye, at, up);

    // Set model and view matrices
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);

    // Calculate and set normal matrix for lighting
    mat3 normalMatrix = Normal(view * model);
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_TRUE, normalMatrix);

    // Set projection matrix
    mat4 proj;
    if (PP) {
        proj = Perspective(45.0, 1.0, 0.1, 100);
    } else {
        proj = Ortho(-r, r, -r, r, -1, 100);
    }
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, proj);

    // Draw all triangles
    for (int i = 0; i < 8; i++) {
        // Determine which color to use based on face
        int colorIndex;
        if (i < 2) {
            // First two triangles (bottom and top faces)
            colorIndex = i;
        } else {
            // Side faces (grouped in pairs)
            colorIndex = 2 + ((i - 2) / 2);
        }
        
        glUniform4fv(faceColourLoc, 1, colour[colorIndex]);
        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y )
{
  switch ( key ) {
  case 033:
    exit( EXIT_SUCCESS );
    break;
  case 'p':
    PP = !PP;
    break;
  }
}

void arrow(int key, int x, int y)
{
  switch (key) { // self explanatory key handling according to assignment instructions
  case GLUT_KEY_LEFT:
    pos -= vec4(0.5, 0, 0, 0);
    break;
  case GLUT_KEY_RIGHT:
    pos += vec4(0.5, 0, 0, 0);
    break;
  case GLUT_KEY_UP:
    pos += vec4(0, 0, 0.5, 0);
    break;
  case GLUT_KEY_DOWN:
    pos -= vec4(0, 0, 0.5, 0);
    break;
  case GLUT_KEY_PAGE_UP:
    scale = scale * vec3(1.05, 1.05, 1.05);
    break;
  case GLUT_KEY_PAGE_DOWN:
    scale = scale * vec3(1/1.05, 1/1.05, 1/1.05);
    break;
  case GLUT_KEY_HOME:
    theta_x += 0.5;
    break;
  case GLUT_KEY_END:
    theta_y += 0.5;
    break;
  case GLUT_KEY_INSERT:
    theta_z += 0.5;
    break;
  }
  
  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
	  if (button == GLUT_LEFT_BUTTON) {
	    angle -= 0.2;
	  } else if (button == GLUT_RIGHT_BUTTON) {
	      angle += 0.2; 
	  }
	  glutPostRedisplay();
  }
}


void mouseWheel(int wheel, int direction, int x, int y) {
	if (direction > 0) {
		r -= 0.1; 
		r = (r < 1.0) ? 1.0 : r; 
	} else {
		r += 0.1; 
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(512, 512);

  // If you are using freeglut, the next two lines will check if 
  // the code is truly 3.2. Otherwise, comment them out
    
  glutInitContextVersion(3, 2);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutCreateWindow("Triangular Prism");

  glewInit();

  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(arrow);
  
  glutMainLoop();
  return 0;
}
