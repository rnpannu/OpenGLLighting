#include <Angel.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

//----------------------------------------------------------------------------

GLint normalMatLoc, normalVecLoc;

//----------------------------------------------------------------------------
GLfloat theta_x = 0.0, theta_y = 0.0, theta_z = 0.0;

vec4 pos(0.0, 0.0, 0.0, 0.0);
vec3 scale(1.0, 1.0, 1.0);
GLint faceColourLoc, modelLoc, viewLoc, projLoc;
GLuint vao[5], buffer[5];

bool use_perspective = false;
GLfloat camera_theta = M_PI/2, camera_radius = 1.0;

const vec4 point[6] = {
  vec4(-0.5, -0.5, -1.0, 1), vec4(0.5, -0.5, -1.0, 1),
  vec4(-0.5, 0.5, -1.0, 1),
  vec4(-0.5, -0.5, 1.0, 1), vec4(0.5, -0.5, 1.0, 1),
  vec4(-0.5, 0.5, 1.0, 1), 
};

// use this as the reflectance value for each face
const vec3 colour[5] = {
  vec3(1,0,0), // red
  vec3(1,1,0), // yellow
  vec3(0,1,0), // green
  vec3(0,1,1), // cyan
  vec3(0,0,1) // blue
};

const int face[5][4] = {
  {0,2,1,-1},  // bot         -1 means invalid index so only 3 vertices
  {3,4,5,-1},  // top
  {5,4,1,2},   // side 1
  {0,3,5,2},   // side 2
  {0,1,4,3}    // side 3
};


// replace the following coordinates
vec2 texCoord[5][4] =
  {
    { vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1) }, // bot
    { vec2(0,0), vec2(0,1), vec2(1,0), vec2(1,1) }, // top
    {vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1) }, // front
    { vec2(1,0), vec2(0,1), vec2(1,1), vec2(0,0)  }, // right
    { vec2(1,0), vec2(0,0), vec2(1,1), vec2(0,1) } // left
  };

vec3 normalvec[5];

vec3 compute_normal(int face_i)
{
  vec4 p[3];
  for (int i = 0; i < 3; i++) {
    p[i] = point[face[face_i][i]];
  }
  return normalize(cross(p[1]-p[0], p[2]-p[0]));
}

void setLighting(GLuint program)
{
  GLint lightPosLoc, lightAmbLoc, lightDiffLoc, lightSpecLoc, shininessLoc;
  
  const vec4 lightPos(1, 2, 3, 1);
  const vec3 lightAmbient(0.3, 0.3, 0.3);
  const vec3 lightDiffuse(0.8, 0.8, 0.8);
  const vec3 lightSpecular(1, 1, 1);
  const GLfloat shininess = 25;
  
  lightPosLoc =  glGetUniformLocation(program, "uLightPos");
  lightAmbLoc =  glGetUniformLocation(program, "uLightAmbient");
  lightDiffLoc =  glGetUniformLocation(program, "uLightDiffuse");
  lightSpecLoc =  glGetUniformLocation(program, "uLightSpecular");
  shininessLoc =  glGetUniformLocation(program, "uShininess");

  glUniform4fv(lightPosLoc, 1, lightPos);
  glUniform3fv(lightAmbLoc, 1, lightAmbient);
  glUniform3fv(lightDiffLoc, 1, lightDiffuse);
  glUniform3fv(lightSpecLoc, 1, lightSpecular);
  glUniform1f(shininessLoc, shininess);
}

void setTextures()
{
  // generate and bind texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // set up wrapping, minification and magnification parameters

  // Repeat wrapping 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Minification filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //Magnification filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load("texture.png", &width, &height, &nrChannels, 0);
  
  if (data) {
    // load image into texture, generate mipmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    cout << "Failed to load texture" << endl;
    exit(1);
  }
  stbi_image_free(data);
}

void init()
{
  // Load shaders and use the resulting shader program
  GLuint program = InitShader("vshader.glsl", "fshader.glsl");
  glUseProgram(program);
  faceColourLoc = glGetUniformLocation(program, "uFaceColour");
  modelLoc = glGetUniformLocation(program, "uModel");
  viewLoc = glGetUniformLocation(program, "uView");
  projLoc = glGetUniformLocation(program, "uProjection");

  // Initialize the vertex position attribute from the vertex shader
  GLuint vloc = glGetAttribLocation( program, "vPosition" );
  GLuint tloc = glGetAttribLocation( program, "aTexCoord" );
  normalMatLoc = glGetUniformLocation(program, "uNormalMatrix");
  normalVecLoc = glGetUniformLocation(program, "uNormalVec");

  setLighting(program);
  
  // Create a vertex array object for each face
  glGenVertexArrays(5, vao);
  glGenBuffers(5, buffer);
  
  for (int i = 0; i < 5; i++) {
    glBindVertexArray(vao[i]);
  
    // initialize a buffer object
    glBindBuffer(GL_ARRAY_BUFFER, buffer[i]);

    GLfloat A[4*(4+2)];
    int size = 0;
    for (int j = 0; j < 4; j++) {
      if (face[i][j] >= 0) {
	A[j*6] = point[face[i][j]].x;
	A[j*6+1] = point[face[i][j]].y;
	A[j*6+2] = point[face[i][j]].z;
	A[j*6+3] = point[face[i][j]].w;
	A[j*6+4] = texCoord[i][j].x;
	A[j*6+5] = texCoord[i][j].y;
	size++;
      }
    }
    glBufferData(GL_ARRAY_BUFFER, size * 6 * sizeof(GLfloat), A, GL_STATIC_DRAW);

    glEnableVertexAttribArray( vloc );
    glVertexAttribPointer( vloc, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
			   BUFFER_OFFSET(0) );

    // this loads the texture coordinates
    glEnableVertexAttribArray( tloc );
    glVertexAttribPointer( tloc, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
			   BUFFER_OFFSET(4 * sizeof(GLfloat)) );

    normalvec[i] = compute_normal(i);
  }

  setTextures();
  
  glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background

  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
}


//----------------------------------------------------------------------------

mat4 get_projection(mat4 modelview)
{
  GLfloat z1 = 1e10, z2 = -1e10;
  
  for (int i = 0; i < 6; i++) {
    auto p = modelview * point[i];
    z1 = min(z1, -p.z);
    z2 = max(z2, -p.z);
  }

  GLfloat znear, zfar;

  if (use_perspective) {
    znear = z1 - 0.01;
    zfar = z2 + 0.01;
    return Perspective(90, 1.0, znear, zfar);
  } else {
    znear = z1 - 0.5;
    zfar = z2 + 0.5;
    return Ortho(-1.0, 1.0, -1.0, 1.0, znear, zfar);
  }
}

void display( void )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  mat4 rotate = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
  mat4 model = Translate(pos) * rotate * Scale(scale);

  GLfloat eye_x = camera_radius * cos(camera_theta);
  GLfloat eye_y = camera_radius * sin(camera_theta);
  vec4 eye(eye_x, eye_y, 0, 1);
  vec4 at(0,0,0,1);
  vec4 up(0,0,1,0);
  mat4 view = LookAt(eye, at, up);
  mat4 modelview = view * model;
  mat3 normalMat = Normal(modelview);
  mat4 proj = get_projection(modelview);

  glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model);
  glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);
  glUniformMatrix3fv(normalMatLoc, 1, GL_TRUE, normalMat);
  glUniformMatrix4fv(projLoc, 1, GL_TRUE, proj);
  
  for (int i = 0; i < 5; i++) {
    glUniform3fv(faceColourLoc, 1, colour[i]);
    glUniform3fv(normalVecLoc, 1, normalvec[i]);
    glBindVertexArray(vao[i]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);    // draw the square
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
  case 'p':
    use_perspective = !use_perspective;
    glutPostRedisplay();
    break;
  }
}

void arrow(int key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_LEFT:
    pos -= vec4(0.05, 0, 0, 0);
    break;
  case GLUT_KEY_RIGHT:
    pos += vec4(0.05, 0, 0, 0);
    break;
  case GLUT_KEY_UP:
    pos += vec4(0, 0, 0.05, 0);
    break;
  case GLUT_KEY_DOWN:
    pos -= vec4(0, 0, 0.05, 0);
    break;
  case GLUT_KEY_PAGE_UP:
    scale *= 1.05;
    break;
  case GLUT_KEY_PAGE_DOWN:
    scale *= 1.0/1.05;
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

void mouse_button(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
      camera_theta -= 0.2;
      break;
    case GLUT_RIGHT_BUTTON:
      camera_theta += 0.2;
      break;
    case 3:
      camera_radius -= 0.1;
      if (camera_radius < 1.0) {
	camera_radius = 1.0;
      }
      break;
    case 4:
      camera_radius += 0.1;
      break;
    }
    glutPostRedisplay();
  }
}

// note: on some platform the following doesn't work
void mouse_wheel(int wheel, int dir, int x, int y)
{
  if (dir > 0) {
    camera_radius -= 0.1;
    if (camera_radius < 1.0)
      camera_radius = 1.0;
  } else {
    camera_radius += 0.1;
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
  glutMouseFunc(mouse_button);
  glutMouseWheelFunc(mouse_wheel);
  
  glutMainLoop();
  return 0;
}
