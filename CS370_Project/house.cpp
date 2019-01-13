// CS370 - Fall 2018
// Final Project

#ifdef OSX
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lighting.h"
#include "materials.h"

// Shader file utility functions
#include "shaderutils.h"

#define DEG2RAD 0.01745
#define NO_TEXTURES 10
#define TAPESTRY 0
#define SKY 1
#define POT 2
#define WOOD 3
#define FUN_WOOD 4
#define SHAG 5
#define ENVIRONMENT 6
#define LABEL 7
#define ORANGE 8
#define DOOR 9
#define X 0
#define Y 1
#define Z 2

// Global spherical coord values
GLfloat azimuth = 45.0f;
GLfloat daz = 2.0f;
GLfloat elevation = 45.0f;
GLfloat del = 2.0f;
GLfloat radius = 10.0f;
GLfloat dr = 0.1f;
GLfloat min_radius = 1.0f;

// Global camera vectors
GLfloat eye[3] = {4.344f,3.152f,2.682f};
GLfloat at[3] = {0.0f,0.0f,0.0f};
GLfloat up[3] = {0.0f,1.0f,0.0f};

//global rotation values
GLfloat theta = 0.0f;
GLfloat dtheta = -0.5f;
GLfloat blinds_anim = 0.52f;
GLfloat accum = 0.3f;

//Animation bools
GLint light = false;
GLint fan_on = false; //done
GLint blinds_op = false;
GLint perspective = false;

// Light0 (broad spot) Parameters
GLfloat light0_pos[] = {0.0f,10.0f,0.0f,1.0f};

// Global screen dimensions
GLint ww, hh;

//quadrics
GLUquadricObj* pot_quad;
GLUquadricObj* fan_quad;
GLUquadricObj* soda_quad;
GLUquadricObj* glass_quad;
GLUquadricObj* orange_quad;

GLfloat cube[][3] = { { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, 1.0f }, { -1.0f, 1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f },
    { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f } };

GLfloat cube_tex[][2] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

GLfloat fan_v[12][2] = {{1.0f, 1.0f}, {1.0f, 4.0f}, {0.0f, 4.0f}, {0.0f, 1.0f}, {-3.0f, 1.0f}, {-3.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, -3.0f}, {1.0f, -3.0f}, {1.0f, 0.0f}, {4.0f, 0.0f}, {4.0f, 1.0f}};

// Shader files
GLchar* defaultVertexFile = "defaultvert.vs";
GLchar* defaultFragmentFile = "defaultfrag.fs";
GLchar* lightVertexFile = "lightvert.vs";
GLchar* lightFragmentFile = "lightfrag.fs";
GLchar* texVertexFile = "texturevert.vs";
GLchar* texFragmentFile = "texturefrag.fs";

// Shader objects
GLuint defaultShaderProg;
GLuint lightShaderProg;
GLuint textureShaderProg;
GLuint numLights_param;
GLint numLights = 1;
GLint texSampler;

//texture indices
GLuint tex_ids[NO_TEXTURES];

// Texture files
char texture_files[NO_TEXTURES][20] = {"tapestry.jpg", "sky.jpg", "terra.jpg", "wood.jpg", "lightwood.jpg", "shag.jpg", "blank.bmp", "soda.jpg", "orange.jpg", "door.png"};

void display();
void render_Scene();
void keyfunc(unsigned char key, int x, int y);
void idlefunc();
void reshape(int w, int h);
bool load_textures();
void create_Mirror();
void render_Mirror();
void texturecube();
void texquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat t1[], GLfloat t2[], GLfloat t3[], GLfloat t4[]);
void bed();
void fan();
void floor();
void room();
void tapestry();
void plant();
void window_sill();
void blinds();
void carpet();
void table();
void soda();
void glass();
void fruit_bowl();
void orange_pyramid();
void door();

int main(int argc, char *argv[])
{
    // Initialize GLUT
    glutInit(&argc,argv);
    
    // Initialize the window with double buffering and RGB colors
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Set the window size to image size
    glutInitWindowSize(512, 512);
    
    // Create window
    glutCreateWindow("House");
    
#ifndef OSX
    // Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
    glewInit();
#endif
    
    // Define callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyfunc);
    glutIdleFunc(idlefunc);
    glutReshapeFunc(reshape);
    
    // Set background color
    glClearColor(1.0f,0.4f,0.5f,0.7f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    // TODO: Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // TODO: Create quadrics
    pot_quad = gluNewQuadric();
    gluQuadricDrawStyle(pot_quad, GLU_FILL);
    gluQuadricNormals(pot_quad, GLU_SMOOTH);
    
    fan_quad = gluNewQuadric();
    gluQuadricDrawStyle(fan_quad, GLU_FILL);
    gluQuadricNormals(fan_quad, GLU_SMOOTH);
    
    soda_quad = gluNewQuadric();
    gluQuadricDrawStyle(soda_quad, GLU_FILL);
    gluQuadricTexture(soda_quad, GL_TRUE);
    
    glass_quad = gluNewQuadric();
    gluQuadricDrawStyle(glass_quad, GLU_FILL);
    gluQuadricNormals(glass_quad, GLU_SMOOTH);
    
    orange_quad = gluNewQuadric();
    gluQuadricDrawStyle(orange_quad, GLU_FILL);
    gluQuadricTexture(orange_quad, GL_TRUE);
    
    // Load textures
    if (!load_textures())
    {
        exit(0);
    }
    // Load shader programs
    defaultShaderProg = load_shaders(defaultVertexFile,defaultFragmentFile);
    lightShaderProg = load_shaders(lightVertexFile, lightFragmentFile);
    textureShaderProg = load_shaders(texVertexFile, texFragmentFile);
    numLights_param = glGetUniformLocation(lightShaderProg, "num_lights");
    texSampler = glGetUniformLocation(textureShaderProg, "texMap");
    glUseProgram(defaultShaderProg);
    
    // Begin event loop
    glutMainLoop();
    return 0;
}

// Display callback
void display()
{
    // TODO: PASS 1 - Create mirror texture
    create_Mirror();
    
    // PASS 2 - Render actual scene
    // Reset background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Set perspective projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 8.0f);
    // Alternatively
    // glPerspective(90.0f,1.0f,1.0f,8.0f);
    // glOrtho(-4.0f, 4.0f, -4.0f, 4.0f, 1.0f, 8.0f); // orthographic projection
    
    // Set modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[X], eye[Y], eye[Z], at[X], at[Y], at[Z], up[X], up[Y], up[Z]);
    
    // Render scene
    glScalef(2.0f, 2.0f, 2.0f);
    render_Scene();
    
    // TODO: Render mirror
    render_Mirror();
    
    // Flush buffer
    glFlush();
    
    // Swap buffers
    glutSwapBuffers();
}

// Scene render function
void render_Scene()
{
    //add floor
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD]);
    glTranslatef(0.2f, 0.2f, 0.3f);
    glScalef(0.7f, 0.7f, 0.7f);
    floor();
    glPopMatrix();
    
    //fan
    glPushMatrix();
    glTranslatef(0.2f, 0.84f, 0.3f);
    glScalef(0.05, 0.05, 0.05);
    glRotatef(theta, 0.0f, 1.0f, 0.0f);
    fan();
    glPopMatrix();
    
    //create space
    glPushMatrix();
    glTranslatef(0.2f, 0.2f, 0.3f);
    glScalef(0.7f, 0.7f, 0.7f);
    room();
    glPopMatrix();
    
    //door
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[DOOR]);
    glTranslatef(0.9f, -0.08f, 0.45f);
    glScalef(0.1f, 0.103f, 0.115f);
    door();
    glPopMatrix();
    
    //carpet
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[SHAG]);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glTranslatef(-0.12f, 1.0f, 0.5f);
    carpet();
    glPopMatrix();
    
    //table
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[FUN_WOOD]);
    glTranslatef(0.0f, -0.36f, 0.7f);
    glScalef(0.7, 0.7, 0.7);
    table();
    glPopMatrix();
    
    //soda
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[LABEL]);
    glTranslatef(0.0f, -0.27f, 0.7f);
    glScalef(0.3, 0.3, 0.3);
    soda();
    glPopMatrix();
    
    //glass
    glPushMatrix();
    glDepthMask(GL_FALSE);
    glColor4f(0.0f, 0.3f, 0.8f, 0.3f);
    glTranslatef(0.1f, -0.22f, 0.75f);
    glScalef(0.3, 0.37, 0.3);
    glass();
    glDepthMask(GL_TRUE);
    glPopMatrix();
    
    //fruit bowl
    glPushMatrix();
    glTranslatef(-0.1f, -0.33f, 0.7f);
    glScalef(0.3, 0.3, 0.3);
    fruit_bowl();
    glPopMatrix();
    
    //oranges
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[ORANGE]);
    glTranslatef(-0.2f, -0.225f, 0.7f);
    glScalef(0.017, 0.017, 0.017);
    glRotatef(45, 0.0f, 0.0f, 1.0f);
    orange_pyramid();
    glPopMatrix();
    
    //bed
    glPushMatrix();
    glTranslatef(0.012f, -0.39f, 0.1f);
    bed();
    glPopMatrix();

    //tapestry
    glPushMatrix();
    // TODO: Bind space texture
    glBindTexture(GL_TEXTURE_2D, tex_ids[TAPESTRY]);
    glTranslatef(-0.48f, 0.29f, 0.2f);
    tapestry();
    glPopMatrix();
    
    //plant
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[POT]);
    glTranslatef(0.8f, -0.5f, -0.25f);
    glScalef(0.53f, 0.82f, 0.7f);
    plant();
    glPopMatrix();
    
    //windowsill
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex_ids[SKY]);
    glTranslatef(0.2f, 0.8f, -0.4f);
    window_sill();
    glPopMatrix();
    
    //blinds overtop
    glPushMatrix();
    glTranslatef(-0.01f, 0.8f, -0.36f);
    glScalef(blinds_anim, 1.0f, 1.0f);
    blinds();
    glPopMatrix();
}

void create_Mirror()
{
    // PASS 1 - Render reflected scene
    // Reset background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // TODO: Set projection matrix for flat "mirror" camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-6.0, 2.0, -3.0, 5.0, 0.0, 15.0);
    
    // TODO: Set modelview matrix positioning "mirror" camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    // TODO: Render scene from mirror
    glPushMatrix();
        render_Scene();
    glPopMatrix();
    
    glFinish();
    
    // TODO: Copy scene to texture
    glBindTexture(GL_TEXTURE_2D, tex_ids[ENVIRONMENT]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 512, 512, 0);
    
}

// Mirror render function
void render_Mirror()
{
    glPushMatrix();
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler,0);
    // TODO: Draw mirror surface
    glTranslatef(0.2f, 0.0f, 0.49f);
    glBindTexture(GL_TEXTURE_2D, tex_ids[ENVIRONMENT]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5,-0.5,0.5);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0,-0.5,0.5);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.0,0.5,0.5);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-0.5,0.5,0.5);
    glEnd();
    
    glUseProgram(defaultShaderProg);
    glColor3f(0.0f,0.0f,0.0f);
    // Draw mirror frame
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.5,-0.5,0.5);
    glVertex3f(0.0,-0.5,0.5);
    glVertex3f(0.0,0.5,0.5);
    glVertex3f(-0.5,0.5,0.5);
    glEnd();
    
    glPopMatrix();
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
    if (key == 'p'|| key == 'P'){
        perspective = !perspective;
    }

    if (key == 'a' || key == 'A') //turn left
    {
        azimuth+=daz;
    }
    else if (key == 'd' || key == 'D') // turn right
    {
        azimuth -= daz;
    }
    
    // TODO: Adjust elevation angle
    if (key == 'x' || key == 'X') //look down
    {
        elevation += del;
    }
    else if (key == 'z' || key == 'Z') //look up
    {
        elevation -= del;
    }
    
    if (key == 's' || key == 'S') //move backward
    {
        radius += dr;
    }
    else if (key == 'w' || key == 'W') //move forward
    {
        if(radius < min_radius){
            radius = radius;
        }
        else{
            radius -= dr;
        }
    }
    
    if (key == 'O' || key == 'o'){
        blinds_op = !blinds_op;
    }
    //TODO: Compute cartesian camera position
    //variable/deg2rad = theta and phi
    eye[X] = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    eye[Y] = (GLfloat)(radius*cos(elevation*DEG2RAD));
    eye[Z] = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    
    // TODO: f toggles fan animation
    if (key == 'f' || key == 'F')
    {
        fan_on = !fan_on;
    }
    
    if (key =='l' || key == 'L'){
        light = !light;
    }
    // <esc> quits
    if (key == 27)
    {
        exit(0);
    }
    
    // Redraw screen
    glutPostRedisplay();
}

// Idle callback
void idlefunc()
{
    if(perspective){
            azimuth = 0.0f;
            elevation = 90.0f;
            radius = 2.1f;
    }
    
    if(blinds_op){
        blinds_anim = 0.1f;
        // Redraw screen (only if animating)
        glutPostRedisplay();
    }
    else if (!blinds_op){
        blinds_anim = 0.52f;
        // Redraw screen (only if animating)
        glutPostRedisplay();
    }
    // TODO: Animation code
    if (fan_on)
    {
        theta += dtheta;
        if (theta > 360.0f)
        {
            theta -= 360.0f;
        }
        
        // Redraw screen (only if animating)
        glutPostRedisplay();
    }
    if (light){
        // Redraw screen (only if animating)
        set_PointLight(GL_LIGHT0, &white_light, light0_pos);
        glutPostRedisplay();
    }
}

// Reshape callback
void reshape(int w, int h)
{
    // Set new screen extents
    glViewport(0, 0, w, h);
    
    // Store new extents
    ww = w;
    hh = h;
}

// Routine to load textures using SOIL
bool load_textures()
{
    // Load environment map texture (NO MIPMAPPING)
    tex_ids[ENVIRONMENT] = SOIL_load_OGL_texture(texture_files[5],SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
    
    // TODO: Set environment map properties if successfully loaded
    if (tex_ids[ENVIRONMENT] != 5)
    {
        // Set scaling filters (no mipmap)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        // Set wrapping modes (clamped)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
    }
    // Otherwise texture failed to load
    else
    {
        return false;
    }
    
    // Load object textures normally
    for (int i=0; i < NO_TEXTURES; i++)
    {
        // Load images
        tex_ids[i] = SOIL_load_OGL_texture(texture_files[i],SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        
        
        // Set texture properties if successfully loaded
        if (tex_ids[i] != 0)
        {
            // Set scaling filters
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
            
            // Set wrapping modes
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        }
        // Otherwise texture failed to load
        else
        {
            return false;
        }
    }
    return true;
}

void texturecube()
{
    // Top face
    texquad(cube[4], cube[7], cube[6], cube[5], cube_tex[0], cube_tex[1], cube_tex[2], cube_tex[3]);
    
    // Bottom face
    texquad(cube[0], cube[1], cube[2], cube[3], cube_tex[4], cube_tex[5], cube_tex[6], cube_tex[7]);
    
    // Left face
    texquad(cube[2], cube[6], cube[7], cube[3], cube_tex[8], cube_tex[9], cube_tex[10], cube_tex[11]);
    
    // Right face
    texquad(cube[0], cube[4], cube[5], cube[1], cube_tex[12], cube_tex[13], cube_tex[14], cube_tex[15]);
    
    // Front face
    texquad(cube[1], cube[5], cube[6], cube[2], cube_tex[16], cube_tex[17], cube_tex[18], cube_tex[19]);
    
    // Back face
    texquad(cube[0], cube[3], cube[7], cube[4], cube_tex[20], cube_tex[21], cube_tex[22], cube_tex[23]);
}

// Routine to draw quadrilateral face
void texquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat t1[], GLfloat t2[], GLfloat t3[], GLfloat t4[])
{
    // Draw face
    glBegin(GL_POLYGON);
    glTexCoord2fv(t1);
    glVertex3fv(v1);
    glTexCoord2fv(t2);
    glVertex3fv(v2);
    glTexCoord2fv(t3);
    glVertex3fv(v3);
    glTexCoord2fv(t4);
    glVertex3fv(v4);
    glEnd();
}

void floor(){
    // Activate shader program and set texture unit 0
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);

    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 1.0f, 0.01f);
    texturecube();
}

void room(){
    
    //create walls
    /* Ceiling */
    glUseProgram(defaultShaderProg);
    glColor3f(1.0f, 1.0f, 0.9f);
    glBegin(GL_QUADS);
        glVertex3f(-1,1,-1);
        glVertex3f(1,1,-1);
        glVertex3f(1,1,1);
        glVertex3f(-1,1,1);
     glEnd();
    /* Walls */
    
//    //wall with mirror
    glColor3f(1.0f, 1.0f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,1);
    glVertex3f(1,-1,1);
    glVertex3f(1,1,1);
    glVertex3f(-1,1,1);
    glEnd();
    
    //wall with window
        glColor3f(1.0f, 1.0f, 0.9f);
        glBegin(GL_QUADS);
        glVertex3f(-1,-1,-1);
        glVertex3f(1,-1,-1);
        glVertex3f(1,1,-1);
        glVertex3f(-1,1,-1);
    glEnd();
    
    //wall with door
        glColor3f(1.0f, 1.0f, 0.8f);
        glRotatef(-90, 1.0, 0.0,0.0);
        glBegin(GL_LINE_LOOP);
        //glVertex3f(1,1,1);
        glVertex3f(1,-1,1); //top left corner (z, x, y)
        glVertex3f(1,-1,-1); //bottom left corner
        //door placement
        glVertex3f(1, -0.5, -1);
        glVertex3f(1, -0.5, 0.2);
        glVertex3f(1, 0.1, 0.2);
        glVertex3f(1, 0.1f, -1.0);
        //glVertex3f(1, 0.5, -1);
        glVertex3f(1,1, -1); //bottom right corner
        glVertex3f(1,1,1); //top right corner
        glEnd();
    
    ///because OPENGL is not helping me create the square shape in the wall, I need to fill the wall in separately using GL_POLYGON.
    glBegin(GL_POLYGON);
    glVertex3f(1,-1,1);
    glVertex3f(1,-1,-1);
    glVertex3f(1, -0.5, -1);
    glVertex3f(1, -0.5, 1);
    glEnd();
    
    glRotatef(-90, 1.0, 0.0,0.0);
    glBegin(GL_POLYGON);
    glVertex3f(1,-1,1);
    glVertex3f(1,-1,-1);
    glVertex3f(1, -0.2, -1);
    glVertex3f(1, -0.2, 1);
    glEnd();
    
    glTranslatef(0.0, 0.0, 1.1);
    glRotatef(-90, 1.0, 0.0,0.0);
    glBegin(GL_POLYGON);
    glVertex3f(1,0.1,1);
    glVertex3f(1,0.1,-1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glEnd();
    
    //wall with tapestry
    glColor3f(1.0f, 1.0f, 0.8f);
    glTranslatef(0.0, 1.1, 0.0);
    glBegin(GL_QUADS);
    glVertex3f(-1,1,1);
    glVertex3f(-1,-1,1);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,1,-1);
    glEnd();
}

void bed(){
    // Activate shader program and set texture unit 0
//    glUseProgram(textureShaderProg);
//    glUniform1i(texSampler, 0);
    
    //frame
    glUseProgram(defaultShaderProg);
    glPushMatrix();
    glColor3f(1.0f, 0.9f, 0.75f);
    glTranslatef(0.0f, -0.03f, 0.0f);
    glScalef(0.45f, 0.08f, 0.325f);
    texturecube();
    glPopMatrix();
    
    //mattress
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.08f, 0.0f);
    glScalef(1.7f, 0.15f, 1.3f);
    glutSolidCube(0.5);
    glPopMatrix();
    
    //pillows
    glPushMatrix();
    glColor3f(1.0f, 0.7f, 0.6f);
    glTranslatef(-0.3f, 0.16f, 0.16f);
    glScalef(0.3f, 0.15f, 0.6f);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0f, 0.7f, 0.6f);
    glTranslatef(-0.3f, 0.16f, -0.16f);
    glScalef(0.3f, 0.15f, 0.6f);
    glutSolidCube(0.5);
    glPopMatrix();
}

void fan(){
    glUseProgram(defaultShaderProg);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluSphere(fan_quad, 1, 30, 30);
    
    glPushMatrix();
    glColor3f(0.7f, 0.4f, 0.3f);
    glTranslatef(-1.3f, 0.6f, -0.9f);
    glScalef(1.5f, 1.5f, 1.5f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for(int i=0; i<12; i++)
    {
        glVertex2fv(fan_v[i]);
    }
    glEnd();
    glPopMatrix();
}

void tapestry(){
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glRotatef(90, 0.0f, 1.0f, 0.0f);
    glScalef(0.4f, 0.5f, 0.01f);
    texturecube();
}

void plant(){
    //pot
    // Activate shader program and set texture unit 0
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glColor3f(0.2f, 0.7f, 0.4f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    gluCylinder(pot_quad,0.1f, 0.18f,0.3f, 20, 20);
    
    //plant
    glUseProgram(defaultShaderProg);
    glTranslatef(0.0f, 0.0f, 0.4f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glScalef(0.13f, 0.3f, 0.13f);
    glutSolidCube(1);
}

void window_sill(){
    //outside
    glPushMatrix();
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glColor3f(0.7f, 0.7f, 0.4f);
    glTranslatef(0.0f, -0.37f, -0.008f);
    glScalef(0.43f, 0.36f, 0.01f);
    texturecube();
    glPopMatrix();
    
    glUseProgram(defaultShaderProg);
    //frame
    glPushMatrix();
    glColor3f(0.2f, 0.7f, 0.4f);
    glScalef(1.8f, 0.05f, 0.03f);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.2f, 0.7f, 0.4f);
    glTranslatef(0.0f, -0.73f, 0.0f);
    glScalef(1.8f, 0.05f, 0.03f);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.2f, 0.7f, 0.4f);
    glRotatef(90, 0.0f, 0.0f, 1.0f);
    glTranslatef(-0.37f, -0.43f, 0.0f);
    glScalef(1.45f, 0.05f, 0.03f);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.2f, 0.7f, 0.4f);
    glRotatef(90, 0.0f, 0.0f, 1.0f);
    glTranslatef(-.37f, 0.43f, 0.0f);
    glScalef(1.45f, 0.05f, 0.03f);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.2f, 0.7f, 0.4f);
    glRotatef(90, 0.0f, 0.0f, 1.0f);
    glTranslatef(-.37f, 0.0f, 0.0f);
    glScalef(1.45f, 0.05f, 0.03f);
    glutSolidCube(0.5);
    glPopMatrix();
}

void blinds(){
    glUseProgram(defaultShaderProg);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, -0.37f, -0.008f);
    glScalef(0.5f, 0.36f, 0.01f);
    texturecube();
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(1.0, -0.37f, -0.008f);
    glScalef(0.5f, 0.36f, 0.01f);
    texturecube();
    glPopMatrix();
}

void carpet(){
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glColor3f(0.4f, 0.7f, 1.0f);
    glTranslatef(0.427f, -0.37f, -0.008f);
    glScalef(0.3f, 0.17f, 0.01f);
    texturecube();
}

void table(){
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glColor3f(0.4f, 0.7f, 1.0f);
    glScalef(0.3f, 0.04f, 0.19f);
    texturecube();
    //legs
    glTranslatef(0.9f, -1.9f, 0.9f);
    glScalef(0.07f, 2.4f, 0.07f);
    texturecube();
    
    glTranslatef(0.0f, 0.0f, -25.2f);
    texturecube();
    
    glTranslatef(-25.9f, 0.0f, 0.0f);
    texturecube();
    
    glTranslatef(0.0f, 0.0f, 25.2f);
    texturecube();
    
    //"chairs"
    glUseProgram(defaultShaderProg);
    glTranslatef(-8.0f, -0.85f, -12.0f);
    glColor3f(1.0f, 0.7f, 0.6f);
    glScalef(5.0f,0.2f,10.0f);
    texturecube();
    
    glTranslatef(8.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.7f, 0.6f);
    texturecube();
}

void soda(){
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glScalef(0.07, 0.07, 0.07);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    gluCylinder(soda_quad, 1, 1, 3, 20, 20);
}

void glass(){
    glUseProgram(defaultShaderProg);
    glScalef(0.1, 0.14, 0.1);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    gluCylinder(glass_quad, 1, 1, 3, 20, 20);
}

void fruit_bowl(){
    glUseProgram(defaultShaderProg);
    glColor3f(0.4f, 0.2f, 0.1f);
    glScalef(0.1, 0.14, 0.1);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    gluCylinder(orange_quad, 1, 3, 0.8, 20, 20);
    glColor3f(0.4f, 0.3f, 0.3f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glScalef(1.0, 0.14, 1.0);
    gluSphere(orange_quad, 1, 20, 20);
}

void orange_pyramid(){
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glTranslatef(0.0f, -6.0f, 1.0f);
    glScalef(2.6, 2.6, 2.6);
    gluSphere(orange_quad, 0.5, 20, 20);
    glTranslatef(1.0f, 0.0f, 0.0f);
    gluSphere(orange_quad, 0.5, 20, 20);
    glTranslatef(0.0f, -1.0f, 0.0f);
    gluSphere(orange_quad, 0.5, 20, 20);
}

void door(){
    glUseProgram(textureShaderProg);
    glUniform1i(texSampler, 0);
    glRotated(90, 0.0, 1.0, 0.0);
    //glRotated(180, 1.0, 0.0, 0.0);
    glScalef(2, 4.2, 0.2);
    texturecube();
}
