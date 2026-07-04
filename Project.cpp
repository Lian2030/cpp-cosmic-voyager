//
// Project.cpp
// CG Project - The Cosmic Voyager
// Lian Talal Alanazi 446009851
// University of Umm al-Qura
// Scene 1: Launch from Earth - 
// Scene 2: Orbiting Past Rotating Planets
// Scene 3: Alien Encounter 
// Scene 4: Landing on a Distant Moon 
// Scene 5: Return Back to Earth 
//

#define _CRT_SECURE_NO_WARNINGS

#ifdef APPLE
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif
#include <stdlib.h>
#include <iostream>
#include <cmath>
using namespace std;

GLint win_width = 600,
win_hight = 600;

int currentScene = 1;

// Scene 1 variables
float rocketX = 0.0f;
float rocketY = -0.5f;

// Scene 2 variables
float rocketX2 = 0.0f;
float rocketY2 = -1.5f;  // starts below screen
float planetAngle = 0.0f;

// Scene 3 variables
float rocketX3 = 0.0f;
float rocketY3 = -0.8f;
float alienShipY = 1.5f;
float alienShipScale = 0.3f;
float rocketAngle3 = 0.0f;

// Scene 4 variables
float rocketX4 = 0.0f;
float rocketY4 = 0.8f;
float moonScale = 1.0f;
float rocketAngle4 = 0.0f;

// Scene 5 variables
float rocketX5 = 0.0f;
float rocketY5 = 0.8f;
bool landed5 = false;

// texture IDs
GLuint texMercury, texMars, texVenus;
GLuint texEarth;
GLuint texGrass;
GLuint texSaudiFlag;

// texture file paths
char mercuryPath[] = "C:\\Users\\ASUS\\Documents\\Visual Studio 18\\mercury.bmp";
char marsPath[] = "C:\\Users\\ASUS\\Documents\\Visual Studio 18\\mars.bmp";
char venusPath[] = "C:\\Users\\ASUS\\Documents\\Visual Studio 18\\venus.bmp";
char earthPath[] = "C:\\Users\\ASUS\\Documents\\Visual Studio 18\\Earth.bmp";
char grassPath[] = "C:\\Users\\ASUS\\Documents\\Visual Studio 18\\grass.bmp";
char saudiFlagPath[] = "C:\\Users\\ASUS\\Documents\\Visual Studio 18\\SaudiFlag.bmp";

// 30 star positions shared across all scenes
float stars[30][2] = {
    {-0.9f, 0.9f}, {-0.7f, 0.7f}, {-0.5f, 0.8f}, {-0.3f, 0.6f},
    {0.1f, 0.9f},  {0.3f, 0.7f},  {0.5f, 0.85f}, {0.7f, 0.6f},
    {0.9f, 0.8f},  {-0.8f, 0.5f}, {-0.6f, 0.3f}, {-0.1f, 0.4f},
    {0.2f, 0.5f},  {0.6f, 0.4f},  {0.8f, 0.3f},  {-0.4f, 0.2f},
    {0.4f, 0.2f},  {-0.9f, 0.1f}, {0.9f, 0.1f},  {0.0f, 0.6f},
    {-0.2f, 0.8f}, {0.3f, 0.1f},  {-0.7f, 0.1f}, {0.8f, 0.5f},
    {-0.5f, 0.4f}, {0.6f, 0.9f},  {-0.3f, 0.9f}, {0.1f, 0.4f},
    {-0.8f, 0.8f}, {0.5f, 0.5f}
};

// shooting star variables used in scenes 1 2 3 4
float shootingStarX = -1.5f;
float shootingStarY = 0.8f;
float shootingStarX2 = -1.5f;
float shootingStarY2 = 0.3f;
bool shootingStarActive = false;
bool shootingStarActive2 = false;
float shootingStarTimer = 0.0f;
float shootingStarTimer2 = 80.0f;


// draws a filled circle using GL_TRIANGLE_FAN
// OpenGL has no built in circle so we approximate with 60 triangles
void drawCircle(float x, float y, float radius, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// draws a filled rectangle centered at x,y
void drawRect(float x, float y, float w, float h, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x - w / 2, y - h / 2);
    glVertex2f(x + w / 2, y - h / 2);
    glVertex2f(x + w / 2, y + h / 2);
    glVertex2f(x - w / 2, y + h / 2);
    glEnd();
}

// draws text string character by character at position x,y
void drawText(float x, float y, const char* text)
{
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}
// small text using HELVETICA 12 font for instructions
void drawSmallText(float x, float y, const char* text)
{
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
}

// draws 30 white star points using the shared stars array
void drawStars()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 30; i++)
        glVertex2f(stars[i][0], stars[i][1]);
    glEnd();
}

// draws dark space background and all 30 stars
void drawSpaceBackground()
{
    drawRect(0.0f, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.08f);
    drawStars();
}

// loads a BMP file and uploads it to OpenGL as a texture
// handles both 24 bit and 32 bit BMP formats
GLuint LoadTexture(const char* filename, int width, int height)
{
    GLuint texture;
    FILE* file;

    file = fopen(filename, "rb");
    if (file == NULL)
    {
        cout << "Unable to open the image file: " << filename << endl;
        return 0;
    }

    // read pixel data offset from BMP header bytes 10 to 13
    fseek(file, 10, SEEK_SET);
    int dataOffset = 0;
    fread(&dataOffset, 4, 1, file);

    // read bit depth from BMP header bytes 28 to 29
    fseek(file, 28, SEEK_SET);
    short bitDepth = 0;
    fread(&bitDepth, 2, 1, file);

    fseek(file, dataOffset, SEEK_SET);

    int bytesPerPixel = bitDepth / 8;
    unsigned char* rawData = (unsigned char*)malloc(width * height * bytesPerPixel);
    fread(rawData, width * height * bytesPerPixel, 1, file);
    fclose(file);

    // BMP stores colors as BGR so we convert to RGB
    unsigned char* data = (unsigned char*)malloc(width * height * 3);
    for (int i = 0; i < width * height; ++i)
    {
        int srcIndex = i * bytesPerPixel;
        int dstIndex = i * 3;
        data[dstIndex + 0] = rawData[srcIndex + 2];
        data[dstIndex + 1] = rawData[srcIndex + 1];
        data[dstIndex + 2] = rawData[srcIndex + 0];
    }
    free(rawData);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);

    if (glGetError() != GL_NO_ERROR)
        printf("GLError in LoadTexture()\n");

    return texture;
}

// draws the grass ground using texture mapping with GL_REPEAT
void drawTexturedGround(GLuint tex)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(3.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(3.0f, 1.0f); glVertex2f(1.0f, -0.6f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -0.6f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// draws a circle with a texture mapped onto it
void drawTexturedCircle(float x, float y, float radius, GLuint tex)
{
    // solid base circle to fill any gaps
    glColor3f(0.1f, 0.3f, 0.7f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();

    // texture layer on top
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex2f(x, y);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        float tx = 0.5f + 0.5f * cos(angle);
        float ty = 0.5f + 0.5f * sin(angle);
        glTexCoord2f(tx, ty);
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// draws a textured rectangle for the Saudi flag on the moon
void drawTexturedFlag(float x, float y, float w, float h, GLuint tex)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y + h);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// draws one shooting star as a bright line with glowing head
void drawShootingStar(float x, float y)
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x - 0.12f, y + 0.06f);
    glEnd();
    drawCircle(x, y, 0.008f, 1.0f, 1.0f, 0.8f);
}

// draws one meteor with rock body and flame trail
void drawMeteor(float x, float y)
{
    // rock body
    drawCircle(x, y, 0.045f, 0.7f, 0.35f, 0.1f);
    drawCircle(x - 0.01f, y + 0.01f, 0.025f, 0.9f, 0.5f, 0.15f);
    drawCircle(x + 0.015f, y - 0.01f, 0.01f, 0.5f, 0.2f, 0.05f);

    // flame trail
    glColor3f(1.0f, 0.4f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.04f, y + 0.01f);
    glVertex2f(x + 0.04f, y - 0.01f);
    glVertex2f(x + 0.14f, y);
    glEnd();

    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.04f, y + 0.005f);
    glVertex2f(x + 0.04f, y - 0.005f);
    glVertex2f(x + 0.10f, y);
    glEnd();
}

// draws the rocket pointing upward using basic OpenGL primitives
void drawRocket(float x, float y)
{
    // body - white rectangle
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y + 0.25f);
    glVertex2f(x - 0.06f, y + 0.25f);
    glEnd();

    // nose - red triangle pointing up
    glColor3f(0.9f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y + 0.25f);
    glVertex2f(x + 0.06f, y + 0.25f);
    glVertex2f(x, y + 0.38f);
    glEnd();

    // left fin
    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x - 0.06f, y + 0.08f);
    glVertex2f(x - 0.14f, y);
    glEnd();

    // right fin
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y + 0.08f);
    glVertex2f(x + 0.14f, y);
    glEnd();

    // window - small blue circle
    drawCircle(x, y + 0.18f, 0.03f, 0.3f, 0.6f, 1.0f);

    // flame - orange outer and yellow inner
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.04f, y);
    glVertex2f(x + 0.04f, y);
    glVertex2f(x, y - 0.1f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.02f, y);
    glVertex2f(x + 0.02f, y);
    glVertex2f(x, y - 0.06f);
    glEnd();
}

// draws the rocket pointing downward for landing scenes
void drawRocketLanding(float x, float y)
{
    // body
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y - 0.25f);
    glVertex2f(x - 0.06f, y - 0.25f);
    glEnd();

    // nose pointing down
    glColor3f(0.9f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y - 0.25f);
    glVertex2f(x + 0.06f, y - 0.25f);
    glVertex2f(x, y - 0.38f);
    glEnd();

    // left fin
    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x - 0.06f, y - 0.08f);
    glVertex2f(x - 0.14f, y);
    glEnd();

    // right fin
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y - 0.08f);
    glVertex2f(x + 0.14f, y);
    glEnd();

    // window
    drawCircle(x, y - 0.18f, 0.03f, 0.3f, 0.6f, 1.0f);

    // flame pointing up because rocket is descending
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.04f, y);
    glVertex2f(x + 0.04f, y);
    glVertex2f(x, y + 0.1f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.02f, y);
    glVertex2f(x + 0.02f, y);
    glVertex2f(x, y + 0.06f);
    glEnd();
}

// draws the astronaut with NASA suit face Saudi flag patch and chest pack
void drawAstronaut(float x, float y)
{
    // legs
    drawRect(x - 0.035f, y - 0.13f, 0.042f, 0.09f, 0.88f, 0.88f, 0.88f);
    drawRect(x + 0.035f, y - 0.13f, 0.042f, 0.09f, 0.88f, 0.88f, 0.88f);

    // knee pads
    drawRect(x - 0.035f, y - 0.10f, 0.046f, 0.025f, 0.75f, 0.75f, 0.75f);
    drawRect(x + 0.035f, y - 0.10f, 0.046f, 0.025f, 0.75f, 0.75f, 0.75f);

    // boots
    drawRect(x - 0.035f, y - 0.195f, 0.052f, 0.032f, 0.25f, 0.25f, 0.25f);
    drawRect(x + 0.035f, y - 0.195f, 0.052f, 0.032f, 0.25f, 0.25f, 0.25f);

    // boot sole
    drawRect(x - 0.035f, y - 0.212f, 0.055f, 0.008f, 0.1f, 0.1f, 0.1f);
    drawRect(x + 0.035f, y - 0.212f, 0.055f, 0.008f, 0.1f, 0.1f, 0.1f);

    // body
    drawRect(x, y, 0.13f, 0.17f, 0.92f, 0.92f, 0.92f);

    // side shading
    drawRect(x - 0.055f, y, 0.022f, 0.17f, 0.78f, 0.78f, 0.78f);
    drawRect(x + 0.055f, y, 0.022f, 0.17f, 0.78f, 0.78f, 0.78f);

    // chest pack
    drawRect(x + 0.01f, y + 0.02f, 0.07f, 0.075f, 0.70f, 0.70f, 0.72f);

    // chest box border
    glColor3f(0.5f, 0.5f, 0.55f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 0.024f, y - 0.018f);
    glVertex2f(x + 0.046f, y - 0.018f);
    glVertex2f(x + 0.046f, y + 0.058f);
    glVertex2f(x - 0.024f, y + 0.058f);
    glEnd();

    // chest buttons
    drawCircle(x - 0.010f, y - 0.005f, 0.005f, 0.9f, 0.1f, 0.1f);
    drawCircle(x + 0.000f, y - 0.005f, 0.005f, 0.1f, 0.8f, 0.1f);
    drawCircle(x + 0.010f, y - 0.005f, 0.005f, 0.9f, 0.9f, 0.1f);

    // Saudi flag patch on left arm using texture
    drawTexturedFlag(x - 0.106f, y + 0.018f, 0.032f, 0.020f, texSaudiFlag);

    // shoulder pads
    drawRect(x - 0.082f, y + 0.068f, 0.048f, 0.030f, 0.82f, 0.82f, 0.82f);
    drawRect(x + 0.082f, y + 0.068f, 0.048f, 0.030f, 0.82f, 0.82f, 0.82f);

    // shoulder pad stripe - orange NASA style
    drawRect(x - 0.082f, y + 0.072f, 0.048f, 0.009f, 0.95f, 0.45f, 0.0f);
    drawRect(x + 0.082f, y + 0.072f, 0.048f, 0.009f, 0.95f, 0.45f, 0.0f);

    // upper arms
    drawRect(x - 0.090f, y + 0.030f, 0.038f, 0.065f, 0.88f, 0.88f, 0.88f);
    drawRect(x + 0.090f, y + 0.030f, 0.038f, 0.065f, 0.88f, 0.88f, 0.88f);

    // elbow rings
    drawRect(x - 0.090f, y - 0.005f, 0.042f, 0.012f, 0.70f, 0.70f, 0.70f);
    drawRect(x + 0.090f, y - 0.005f, 0.042f, 0.012f, 0.70f, 0.70f, 0.70f);

    // lower arms
    drawRect(x - 0.090f, y - 0.038f, 0.036f, 0.055f, 0.88f, 0.88f, 0.88f);
    drawRect(x + 0.090f, y - 0.038f, 0.036f, 0.055f, 0.88f, 0.88f, 0.88f);

    // gloves
    drawCircle(x - 0.090f, y - 0.068f, 0.024f, 0.30f, 0.30f, 0.30f);
    drawCircle(x + 0.090f, y - 0.068f, 0.024f, 0.30f, 0.30f, 0.30f);

    // glove wrist rings
    drawRect(x - 0.090f, y - 0.051f, 0.040f, 0.010f, 0.65f, 0.65f, 0.65f);
    drawRect(x + 0.090f, y - 0.051f, 0.040f, 0.010f, 0.65f, 0.65f, 0.65f);

    // backpack
    drawRect(x - 0.082f, y + 0.015f, 0.028f, 0.095f, 0.72f, 0.72f, 0.74f);

    // backpack vents
    drawRect(x - 0.082f, y + 0.040f, 0.022f, 0.007f, 0.55f, 0.55f, 0.58f);
    drawRect(x - 0.082f, y + 0.028f, 0.022f, 0.007f, 0.55f, 0.55f, 0.58f);
    drawRect(x - 0.082f, y + 0.016f, 0.022f, 0.007f, 0.55f, 0.55f, 0.58f);

    // neck ring
    drawRect(x, y + 0.080f, 0.055f, 0.018f, 0.72f, 0.72f, 0.72f);

    // helmet outer - white
    drawCircle(x, y + 0.155f, 0.082f, 0.93f, 0.93f, 0.93f);

    // helmet shading
    glColor3f(0.80f, 0.80f, 0.80f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x - 0.04f, y + 0.155f);
    for (int i = 30; i <= 150; i++)
    {
        float angle = 2.0f * 3.14159f * i / 360;
        glVertex2f(x + cos(angle) * 0.082f, y + 0.155f + sin(angle) * 0.082f);
    }
    glEnd();

    // helmet ring outline
    glColor3f(0.65f, 0.65f, 0.65f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * 0.082f, y + 0.155f + sin(angle) * 0.082f);
    }
    glEnd();

    // visor - dark tinted oval
    glColor3f(0.08f, 0.08f, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y + 0.155f);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * 0.055f, y + 0.155f + sin(angle) * 0.048f);
    }
    glEnd();

    // visor frame
    glColor3f(0.4f, 0.4f, 0.4f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * 0.055f, y + 0.155f + sin(angle) * 0.048f);
    }
    glEnd();

    // face inside visor - skin tone
    drawCircle(x, y + 0.158f, 0.032f, 0.95f, 0.80f, 0.65f);

    // eyes white
    drawCircle(x - 0.011f, y + 0.165f, 0.007f, 1.0f, 1.0f, 1.0f);
    drawCircle(x + 0.011f, y + 0.165f, 0.007f, 1.0f, 1.0f, 1.0f);

    // pupils
    drawCircle(x - 0.011f, y + 0.165f, 0.004f, 0.2f, 0.1f, 0.05f);
    drawCircle(x + 0.011f, y + 0.165f, 0.004f, 0.2f, 0.1f, 0.05f);

    // eye shine
    drawCircle(x - 0.009f, y + 0.167f, 0.0015f, 1.0f, 1.0f, 1.0f);
    drawCircle(x + 0.013f, y + 0.167f, 0.0015f, 1.0f, 1.0f, 1.0f);

    // eyebrows
    glColor3f(0.3f, 0.2f, 0.1f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x - 0.017f, y + 0.174f); glVertex2f(x - 0.005f, y + 0.175f);
    glVertex2f(x + 0.005f, y + 0.175f); glVertex2f(x + 0.017f, y + 0.174f);
    glEnd();

    // nose
    glColor3f(0.85f, 0.68f, 0.52f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.004f, y + 0.157f);
    glVertex2f(x + 0.004f, y + 0.157f);
    glVertex2f(x, y + 0.150f);
    glEnd();

    // smile
    glColor3f(0.7f, 0.3f, 0.3f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++)
    {
        float angle = 3.14159f + 3.14159f * i / 10;
        glVertex2f(x + cos(angle) * 0.012f, y + 0.148f + sin(angle) * 0.006f);
    }
    glEnd();

    // suit seam lines
    glColor3f(0.72f, 0.72f, 0.72f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 0.083f); glVertex2f(x, y - 0.083f);
    glVertex2f(x - 0.065f, y + 0.050f); glVertex2f(x + 0.065f, y + 0.050f);
    glEnd();
}

// draws the Saudi flag planted on the moon surface
void drawMoonFlag(float x, float y)
{
    // flag pole - thin grey rectangle
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x, y + 0.22f);
    glEnd();

    // flag base on ground
    drawRect(x, y + 0.01f, 0.04f, 0.01f, 0.6f, 0.6f, 0.6f);

    // Saudi flag texture on rectangle
    drawTexturedFlag(x, y + 0.13f, 0.18f, 0.09f, texSaudiFlag);

    // flag border outline
    glColor3f(0.0f, 0.3f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y + 0.13f);
    glVertex2f(x + 0.18f, y + 0.13f);
    glVertex2f(x + 0.18f, y + 0.22f);
    glVertex2f(x, y + 0.22f);
    glEnd();
}

// draws one alien with body head eyes smile antennae arms and legs
void drawAlien(float x, float y)
{
    // body - green oval
    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * 0.04f, y + sin(angle) * 0.06f);
    }
    glEnd();

    // head
    drawCircle(x, y + 0.08f, 0.045f, 0.2f, 0.9f, 0.2f);

    // eyes
    drawCircle(x - 0.015f, y + 0.09f, 0.012f, 0.0f, 0.0f, 0.0f);
    drawCircle(x + 0.015f, y + 0.09f, 0.012f, 0.0f, 0.0f, 0.0f);

    // eye shine
    drawCircle(x - 0.012f, y + 0.095f, 0.004f, 1.0f, 1.0f, 1.0f);
    drawCircle(x + 0.018f, y + 0.095f, 0.004f, 1.0f, 1.0f, 1.0f);

    // smile
    glColor3f(0.0f, 0.4f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++)
    {
        float angle = 3.14159f + 3.14159f * i / 10;
        glVertex2f(x + cos(angle) * 0.02f, y + 0.065f + sin(angle) * 0.01f);
    }
    glEnd();

    // antennae
    glColor3f(0.0f, 0.8f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x - 0.02f, y + 0.12f); glVertex2f(x - 0.04f, y + 0.18f);
    glVertex2f(x + 0.02f, y + 0.12f); glVertex2f(x + 0.04f, y + 0.18f);
    glEnd();

    // antenna tips
    drawCircle(x - 0.04f, y + 0.18f, 0.008f, 0.0f, 1.0f, 0.0f);
    drawCircle(x + 0.04f, y + 0.18f, 0.008f, 0.0f, 1.0f, 0.0f);

    // arms
    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.04f, y + 0.02f);
    glVertex2f(x - 0.04f, y - 0.02f);
    glVertex2f(x - 0.09f, y);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.04f, y + 0.02f);
    glVertex2f(x + 0.04f, y - 0.02f);
    glVertex2f(x + 0.09f, y);
    glEnd();

    // legs
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.02f, y - 0.05f);
    glVertex2f(x - 0.04f, y - 0.05f);
    glVertex2f(x - 0.03f, y - 0.10f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.02f, y - 0.05f);
    glVertex2f(x + 0.04f, y - 0.05f);
    glVertex2f(x + 0.03f, y - 0.10f);
    glEnd();
}

// draws the alien spaceship with dome rim lights exhaust and 5 aliens inside
void drawAlienShip(float x, float y, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // ship bottom dark grey
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 3.14159f * i / 60;
        glVertex2f(cos(angle) * 0.38f, -sin(angle) * 0.12f);
    }
    glEnd();

    // rim silver
    glColor3f(0.75f, 0.75f, 0.85f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(cos(angle) * 0.38f, sin(angle) * 0.09f);
    }
    glEnd();

    // rim outline
    glColor3f(0.5f, 0.5f, 0.6f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(cos(angle) * 0.38f, sin(angle) * 0.09f);
    }
    glEnd();

    // green dome
    glColor3f(0.1f, 0.65f, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.12f);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(cos(angle) * 0.24f, 0.12f + sin(angle) * 0.18f);
    }
    glEnd();

    // dome outline
    glColor3f(0.0f, 0.4f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(cos(angle) * 0.24f, 0.12f + sin(angle) * 0.18f);
    }
    glEnd();

    // 6 colored lights on rim
    float lightColors[6][3] = {
        {1,0,0}, {0,1,0}, {1,1,0}, {0,0.5f,1}, {1,0,1}, {1,0.5f,0}
    };
    for (int i = 0; i < 6; i++)
    {
        float angle = 2.0f * 3.14159f * i / 6;
        float lx = cos(angle) * 0.32f;
        float ly = sin(angle) * 0.07f;
        glColor3f(lightColors[i][0], lightColors[i][1], lightColors[i][2]);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(lx, ly);
        for (int j = 0; j <= 20; j++)
        {
            float a = 2.0f * 3.14159f * j / 20;
            glVertex2f(lx + cos(a) * 0.028f, ly + sin(a) * 0.028f);
        }
        glEnd();
    }

    // blue exhaust glow under ship
    glColor3f(0.0f, 0.8f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, -0.10f);
    for (int i = 0; i <= 30; i++)
    {
        float angle = 3.14159f + 3.14159f * i / 30;
        glVertex2f(cos(angle) * 0.15f, -0.10f + sin(angle) * 0.05f);
    }
    glEnd();

    // 5 aliens sitting inside the dome
    float alienPositions[5] = { -0.17f, -0.085f, 0.0f, 0.085f, 0.17f };
    for (int i = 0; i < 5; i++)
        drawAlien(alienPositions[i], 0.08f);

    glPopMatrix();
}

// ============================================
// SCENE 1: Launch from Earth
// ============================================

// draws dark night sky with 20 stars
void drawBackground()
{
    glColor3f(0.02f, 0.02f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // only 20 stars - bottom ones hidden behind ground
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 20; i++)
        glVertex2f(stars[i][0], stars[i][1]);
    glEnd();
}

// draws textured grass ground and launch pad with legs
void drawGround()
{
    drawTexturedGround(texGrass);

    // launch pad
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-0.15f, -0.6f);
    glVertex2f(0.15f, -0.6f);
    glVertex2f(0.15f, -0.55f);
    glVertex2f(-0.15f, -0.55f);
    glEnd();

    // launch pad legs using GL_LINES - 2 vertices per line
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, -0.55f); glVertex2f(-0.2f, -0.6f);
    glVertex2f(0.15f, -0.55f); glVertex2f(0.2f, -0.6f);
    glEnd();
}

void drawScene1()
{
    drawBackground();
    drawGround();
    drawRocket(rocketX, rocketY);

    // shooting stars
    if (shootingStarActive)  drawShootingStar(shootingStarX, shootingStarY);
    if (shootingStarActive2) drawShootingStar(shootingStarX2, shootingStarY2);

    glColor3f(0.9f, 0.9f, 0.9f);
    drawSmallText(-0.98f, -0.95f, "Arrow Keys: Move | N: Next Scene");
}

// ============================================
// SCENE 2: Orbiting Past Rotating Planets
// ============================================
void drawScene2()
{
    drawSpaceBackground();

    // Mercury - top left - rotates forward
    glPushMatrix();
    glTranslatef(-0.5f, 0.5f, 0.0f);
    glRotatef(planetAngle, 0.0f, 0.0f, 1.0f);
    drawTexturedCircle(0.0f, 0.0f, 0.2f, texMercury);
    glPopMatrix();

    // Venus - right - rotates in opposite direction
    glPushMatrix();
    glTranslatef(0.5f, 0.0f, 0.0f);
    glRotatef(-planetAngle, 0.0f, 0.0f, 1.0f);
    drawTexturedCircle(0.0f, 0.0f, 0.25f, texVenus);
    glPopMatrix();

    // Mars - bottom - rotates at half speed
    glPushMatrix();
    glTranslatef(0.0f, -0.55f, 0.0f);
    glRotatef(planetAngle * 0.5f, 0.0f, 0.0f, 1.0f);
    drawTexturedCircle(0.0f, 0.0f, 0.18f, texMars);
    glPopMatrix();

    drawRocket(rocketX2, rocketY2);

    // shooting stars
    if (shootingStarActive)  drawShootingStar(shootingStarX, shootingStarY);
    if (shootingStarActive2) drawShootingStar(shootingStarX2, shootingStarY2);

    glColor3f(0.9f, 0.9f, 0.9f);
    drawSmallText(-0.98f, -0.95f, "UP: Spin Planets | N: Next Scene");
}

// ============================================
// SCENE 3: Alien Encounter
// ============================================
void drawScene3()
{
    drawSpaceBackground();

    drawAlienShip(0.0f, alienShipY, alienShipScale);

    // rocket rotates using glRotatef when moving left or right
    glPushMatrix();
    glTranslatef(rocketX3, rocketY3, 0.0f);
    glRotatef(rocketAngle3, 0.0f, 0.0f, 1.0f);
    drawRocket(0.0f, 0.0f);
    glPopMatrix();

    // shooting stars
    if (shootingStarActive)  
        drawShootingStar(shootingStarX, shootingStarY);
    if (shootingStarActive2) 
        drawShootingStar(shootingStarX2, shootingStarY2);

    glColor3f(0.9f, 0.9f, 0.9f);
    drawSmallText(-0.98f, -0.95f, "UP: Aliens Get Closer | LR: Rotate | N: Next");
}

// ============================================
// SCENE 4: Landing on a Distant Moon
// ============================================
void drawScene4()
{
    drawSpaceBackground();

    // moon scales bigger as rocket descends using glScalef
    glPushMatrix();
    glTranslatef(0.0f, -1.2f, 0.0f);
    glScalef(moonScale, moonScale, 1.0f);

    drawCircle(0.0f, 0.0f, 0.9f, 0.82f, 0.82f, 0.82f);

    // large craters
    drawCircle(-0.3f, 0.4f, 0.12f, 0.70f, 0.70f, 0.70f);
    drawCircle(0.4f, 0.2f, 0.10f, 0.70f, 0.70f, 0.70f);
    drawCircle(-0.1f, -0.2f, 0.14f, 0.70f, 0.70f, 0.70f);
    drawCircle(0.3f, -0.4f, 0.09f, 0.70f, 0.70f, 0.70f);
    drawCircle(-0.5f, -0.1f, 0.08f, 0.70f, 0.70f, 0.70f);

    // small craters
    drawCircle(0.1f, 0.5f, 0.05f, 0.68f, 0.68f, 0.68f);
    drawCircle(-0.55f, 0.3f, 0.06f, 0.68f, 0.68f, 0.68f);
    drawCircle(0.55f, -0.2f, 0.05f, 0.68f, 0.68f, 0.68f);
    drawCircle(-0.2f, 0.6f, 0.04f, 0.68f, 0.68f, 0.68f);
    drawCircle(0.45f, 0.5f, 0.04f, 0.68f, 0.68f, 0.68f);

    // crater shadows
    drawCircle(-0.3f, 0.4f, 0.08f, 0.60f, 0.60f, 0.60f);
    drawCircle(0.4f, 0.2f, 0.06f, 0.60f, 0.60f, 0.60f);
    drawCircle(-0.1f, -0.2f, 0.09f, 0.60f, 0.60f, 0.60f);
    drawCircle(0.3f, -0.4f, 0.05f, 0.60f, 0.60f, 0.60f);
    drawCircle(-0.5f, -0.1f, 0.04f, 0.60f, 0.60f, 0.60f);
    glPopMatrix();

    // rocket flips using glRotatef as it descends 
    glPushMatrix();
    glTranslatef(rocketX4, rocketY4, 0.0f);
    glRotatef(rocketAngle4, 0.0f, 0.0f, 1.0f);
    drawRocket(0.0f, 0.0f);
    glPopMatrix();

    // astronaut and flag appear when rocket reaches moon surface
    if (rocketY4 <= -0.1f)
    {
        drawAstronaut(rocketX4 + 0.25f, -0.18f);
        drawMoonFlag(rocketX4 + 0.52f, -0.28f);
    }

    // shooting stars
    if (shootingStarActive)  drawShootingStar(shootingStarX, shootingStarY);
    if (shootingStarActive2) drawShootingStar(shootingStarX2, shootingStarY2);

    glColor3f(0.9f, 0.9f, 0.9f);
    drawSmallText(-0.98f, -0.95f, "DOWN: Land & Moon Grows | N: Next Scene");
}

// ============================================
// SCENE 5: Return Back to Earth
// ============================================
void drawScene5()
{
    drawRect(0.0f, 0.0f, 2.0f, 2.0f, 0.05f, 0.1f, 0.3f);

    // only top stars visible above Earth
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 15; i++)
        if (stars[i][1] > 0.3f)
            glVertex2f(stars[i][0], stars[i][1]);
    glEnd();

    // Earth with texture at bottom of screen
    drawTexturedCircle(0.0f, -1.1f, 0.85f, texEarth);

    drawRocketLanding(rocketX5, rocketY5);

    // landing message appears when rocket reaches Earth
    if (rocketY5 <= 0.0f)
    {
        landed5 = true;

        // centered dark message box in middle of screen
        drawRect(0.0f, 0.5f, 0.90f, 0.32f, 0.05f, 0.05f, 0.15f);

        // box border 
        glColor3f(0.3f, 0.6f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.45f, 0.34f);
        glVertex2f( 0.45f, 0.34f);
        glVertex2f( 0.45f, 0.66f);
        glVertex2f(-0.45f, 0.66f);
        glEnd();

        // welcome message 
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.22f, 0.56f, "Welcome Home!");

        // completion message 
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.33f, 0.46f, "The Voyage is Complete!");
    }

    glColor3f(0.7f, 0.7f, 0.7f);
    if (!landed5)
        drawSmallText(-0.98f, -0.95f, "Arrow Keys: Move | Land on Earth!");
    else
        drawSmallText(-0.98f, -0.95f, "R: Restart | Q: Quit");
}

// ============================================
// Display
// ============================================
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (currentScene)
    {
    case 1:
        drawScene1();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.22f, 0.88f, "Scene 1: Launch!");
        break;
    case 2:
        drawScene2();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.25f, 0.88f, "Scene 2: The Planets");
        break;
    case 3:
        drawScene3();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.28f, 0.88f, "Scene 3: Alien Encounter!");
        break;
    case 4:
        drawScene4();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.25f, 0.88f, "Scene 4: Moon Landing");
        break;
    case 5:
        drawScene5();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.28f, 0.88f, "Scene 5: Return to Earth");
        break;
    }

    glutSwapBuffers();
}

// ============================================
// Update
// ============================================

// timer callback called every 16ms for approximately 60fps animation
void update(int value)
{
    // shooting stars active in scenes 1 2 3 4
    if (currentScene >= 1 && currentScene <= 4)
    {
        shootingStarTimer++;

        // activate shooting star 1 every 120 frames
        if (shootingStarTimer > 120)
        {
            shootingStarActive = true;
            shootingStarX = -1.2f;
            shootingStarY = 0.6f + ((rand() % 40) / 100.0f);
            shootingStarTimer = 0;
        }

        // activate shooting star 2 with offset
        if (shootingStarTimer > 60)
            shootingStarActive2 = true;

        // move shooting star 1
        if (shootingStarActive)
        {
            shootingStarX += 0.018f;
            shootingStarY -= 0.008f;
            if (shootingStarX > 1.3f)
            {
                shootingStarActive = false;
                shootingStarX = -1.2f;
            }
        }

        // move shooting star 2 at different speed
        if (shootingStarActive2)
        {
            shootingStarX2 += 0.014f;
            shootingStarY2 -= 0.006f;
            if (shootingStarX2 > 1.3f)
            {
                shootingStarActive2 = false;
                shootingStarX2 = -1.2f;
                shootingStarY2 = 0.2f + ((rand() % 50) / 100.0f);
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ============================================
// Keyboard
// ============================================

// handles regular keys N R Q
void key(unsigned char keyPressed, int x, int y)
{
    switch (keyPressed)
    {
    case 'n': case 'N':
        if (currentScene < 5) currentScene++;
        break;

    case 'r': case 'R':
        currentScene = 1;
        rocketX = 0.0f;    rocketY = -0.5f;
        rocketX2 = 0.0f;   rocketY2 = -1.5f;
        rocketX3 = 0.0f;   rocketY3 = -0.8f;
        rocketX4 = 0.0f;   rocketY4 = 0.8f;
        rocketX5 = 0.0f;   rocketY5 = 0.8f;
        planetAngle = 0.0f;
        moonScale = 1.0f;
        alienShipY = 1.5f;
        alienShipScale = 0.3f;
        rocketAngle3 = 0.0f;
        rocketAngle4 = 0.0f;
        landed5 = false;
        shootingStarX = -1.5f;  shootingStarY = 0.8f;
        shootingStarX2 = -1.5f; shootingStarY2 = 0.3f;
        shootingStarActive = false;
        shootingStarActive2 = false;
        shootingStarTimer = 0.0f;

        break;

    case 'q': case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

// handles arrow keys - behavior changes per scene
void specialKey(int key, int x, int y)
{
    switch (currentScene)
    {
    case 1:
        if (key == GLUT_KEY_UP)    rocketY += 0.03f;
        if (key == GLUT_KEY_DOWN)  rocketY -= 0.03f;
        if (key == GLUT_KEY_LEFT)  rocketX -= 0.03f;
        if (key == GLUT_KEY_RIGHT) rocketX += 0.03f;
        break;

    case 2:
        if (key == GLUT_KEY_UP)
        {
            rocketY2 += 0.03f;
            planetAngle += 10.0f;
            if (planetAngle > 360.0f) planetAngle = 0.0f;
        }
        if (key == GLUT_KEY_DOWN)
        {
            rocketY2 -= 0.03f;
            planetAngle -= 10.0f;
            if (planetAngle < 0.0f) planetAngle = 360.0f;
        }
        if (key == GLUT_KEY_LEFT)  rocketX2 -= 0.03f;
        if (key == GLUT_KEY_RIGHT) rocketX2 += 0.03f;
        break;

    case 3:
        if (key == GLUT_KEY_UP)
        {
            rocketY3 += 0.03f;
            alienShipY -= 0.02f;
            if (alienShipScale < 1.2f)
                alienShipScale += 0.008f;
        }
        if (key == GLUT_KEY_DOWN)
        {
            rocketY3 -= 0.03f;
            alienShipY += 0.02f;
            if (alienShipScale > 0.3f)
                alienShipScale -= 0.008f;
        }
        if (key == GLUT_KEY_LEFT)
        {
            rocketX3 -= 0.03f;
            rocketAngle3 += 15.0f;
            if (rocketAngle3 > 360.0f) rocketAngle3 = 0.0f;
        }
        if (key == GLUT_KEY_RIGHT)
        {
            rocketX3 += 0.03f;
            rocketAngle3 -= 15.0f;
            if (rocketAngle3 < 0.0f) rocketAngle3 = 360.0f;
        }
        break;

    case 4:
        if (key == GLUT_KEY_UP)
        {
            rocketY4 += 0.03f;
            rocketAngle4 -= 10.0f;
            if (rocketAngle4 < 0.0f) rocketAngle4 = 0.0f;
            if (moonScale > 1.0f) moonScale -= 0.02f;
        }
        if (key == GLUT_KEY_DOWN)
        {
            rocketY4 -= 0.03f;
            moonScale += 0.02f;
            if (moonScale > 2.0f) moonScale = 2.0f;
            rocketAngle4 += 10.0f;
            if (rocketAngle4 > 180.0f) rocketAngle4 = 180.0f;
        }
        if (key == GLUT_KEY_LEFT)  rocketX4 -= 0.03f;
        if (key == GLUT_KEY_RIGHT) rocketX4 += 0.03f;
        break;

    case 5:
        if (!landed5)
        {
            if (key == GLUT_KEY_UP)    rocketY5 += 0.03f;
            if (key == GLUT_KEY_DOWN)  rocketY5 -= 0.03f;
            if (key == GLUT_KEY_LEFT)  rocketX5 -= 0.03f;
            if (key == GLUT_KEY_RIGHT) rocketX5 += 0.03f;
        }
        break;
    }
    glutPostRedisplay();
}

void reshapeFunc(GLint new_width, GLint new_hight)
{
    glViewport(0, 0, new_width, new_hight);
    win_width = new_width;
    win_hight = new_hight;
}

// ============================================
// Init
// ============================================
void init()
{
    glClearColor(0.0f, 0.0f, 0.08f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    // load all textures at startup
    texMercury = LoadTexture(mercuryPath, 2048, 1024);
    texMars = LoadTexture(marsPath, 2048, 800);
    texVenus = LoadTexture(venusPath, 2048, 1024);
    texEarth = LoadTexture(earthPath, 2048, 1024);
    texGrass = LoadTexture(grassPath, 4352, 3264);
    texSaudiFlag = LoadTexture(saudiFlagPath, 2000, 1143); 

    cout << "================================" << endl;
    cout << "   The Cosmic Voyager           " << endl;
    cout << "   The Orbiters                 " << endl;
    cout << "================================" << endl;
    cout << " Arrow Keys  Move Rocket        " << endl;
    cout << " Scene 2 UP  Planets Spin       " << endl;
    cout << " Scene 3 UP  Alien Ship Closer  " << endl;
    cout << " Scene 3 LR  Rocket Rotates     " << endl;
    cout << " Scene 4 DN  Moon Grows         " << endl;
    cout << " N           Next Scene         " << endl;
    cout << " R           Restart            " << endl;
    cout << " Q / ESC     Quit               " << endl;
    cout << "================================" << endl;
}

// ============================================
// Main
// ============================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("The Cosmic Voyager - The Orbiters");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshapeFunc);
    glutKeyboardFunc(key);
    glutSpecialFunc(specialKey);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
