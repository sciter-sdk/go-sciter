#include "stdafx.h"
#include "sciter-x.h"
#include "sciter-x-behavior.h"
#include "sciter-x-graphics.hpp"

#if defined(WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <gl/GL.h>
  #include <gl/GLu.h>
  #pragma comment(lib, "OpenGL32.lib")
  #pragma comment(lib, "Glu32.lib")
#elif defined(OSX)
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <OpenGL/glext.h>
  #include <GLUT/glut.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/CGLTypes.h>
#elif defined(LINUX)
  #include <gtk/gtk.h>
  #include <GL/gl.h>
  #include <GL/glx.h>
#endif


namespace sciter
{
/*
SAMPLE:
   See: samples/behaviors/native-drawing.htm
*/

struct molehill_opengl: public event_handler
{
    HWND  hwnd = NULL;
    HGLRC mainGLRC = NULL;
    HGLRC thisGLRC = NULL;
    HDC   hdc = NULL;

    float   theta = 0;
    INT32   color1 = 0xFF0000;
    INT32   color2 = 0x00FF00;
    INT32   color3 = 0x0000FF;

    // ctor
    molehill_opengl() {}
    virtual ~molehill_opengl() {}

    virtual bool subscription( HELEMENT he, UINT& event_groups )
    {
      event_groups = HANDLE_ALL;   // it does drawing
      return true;
    }

    virtual void attached  (HELEMENT he ) 
    {
      hwnd = dom::element(he).get_element_hwnd(true);
    }
    virtual void detached  (HELEMENT he ) { 
      wglDeleteContext(thisGLRC);
      ReleaseDC(hwnd, hdc);
      asset_release(); 
    }

    void init_drawing() {

      if (!mainGLRC) {
        mainGLRC = wglGetCurrentContext();
        hdc = GetDC(hwnd);
        thisGLRC = wglCreateContext(hdc);
        wglShareLists(mainGLRC, thisGLRC);
      }

    }

    void set_color(INT32 clr) {
      glColor3f(
        (clr & 0xFF) / 255.0f,
        ((clr >> 8) & 0xFF) / 255.0f,
        ((clr >> 16) & 0xFF) / 255.0f);
    }

    virtual bool handle_draw   (HELEMENT he, DRAW_PARAMS& params ) 
    {
      if( params.cmd != DRAW_CONTENT ) 
        return false; // drawing only content layer

      HGLRC currentGLRC = wglGetCurrentContext();
      if (!currentGLRC)
        return false;

      sciter::graphics gfx(params.gfx);

      gfx.flush(); // we need to flush current command buffers in order our own 
                   // commands to appear on top of them. 
      init_drawing();

      // Do our own OpenGL drawing:

      // make it the current rendering context 
      wglMakeCurrent(hdc, thisGLRC);

      //--- glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // do not clear what is drawn underneath

      glPushMatrix();
      glRotatef(theta, 0.0f, 0.0f, 1.0f);
      glBegin(GL_TRIANGLES);
      set_color(color1); glVertex2f(0.0f, 1.0f);
      set_color(color2); glVertex2f(0.87f, -0.5f);
      set_color(color3); glVertex2f(-0.87f, -0.5f);
      glEnd();
      glPopMatrix();

      glFlush();

      // make the rendering context not current  
      wglMakeCurrent(hdc, currentGLRC);

      return true; // done drawing
    
    }
    

    // script API

    SOM_PASSPORT_BEGIN_EX(molehill,molehill_opengl)
      //SOM_FUNCS()
      SOM_PROPS(
        SOM_PROP(theta),
        SOM_PROP(color1),
        SOM_PROP(color2),
        SOM_PROP(color3)
      )
    SOM_PASSPORT_END



};

struct molehill_opengl_factory: public behavior_factory {

  molehill_opengl_factory(): behavior_factory("molehill-opengl") { }

  // the only behavior_factory method:
  virtual event_handler* create(HELEMENT he) {
    return new molehill_opengl();
  }

};

// instantiating and attaching it to the global list
molehill_opengl_factory molehill_opengl_factory_instance;


}
