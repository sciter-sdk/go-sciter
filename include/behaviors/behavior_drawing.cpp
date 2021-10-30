//#include "stdafx.h"
#include "sciter-x.h"
#include "sciter-x-behavior.h"
#include "sciter-x-graphics.hpp"
#include <time.h>   
#include <cmath>

using namespace std;

namespace sciter
{
/*
BEHAVIOR: native-clock
  - draws content layer using sciter-x-graphics.hpp primitives.

SAMPLE:
   See: samples/behaviors/native-drawing.htm
*/

struct native_clock: public event_handler
{
    // ctor
    native_clock() {}
    virtual ~native_clock() {}
        
    virtual bool subscription( HELEMENT he, UINT& event_groups )
    {
      event_groups = HANDLE_DRAW   // it does drawing
                   | HANDLE_TIMER; // and handles timer
      return true;
    }

    virtual void attached  (HELEMENT he ) 
    {
      dom::element(he).start_timer(1000,this);
    }
    virtual void detached  (HELEMENT he ) { 
      dom::element(he).stop_timer(this);
      asset_release(); 
    }


    virtual bool handle_timer  (HELEMENT he,TIMER_PARAMS& params )
    {
      dom::element(he).refresh(); // refresh element's area
      return true; // keep ticking
    }

    virtual bool handle_draw   (HELEMENT he, DRAW_PARAMS& params ) 
    {
      if( params.cmd != DRAW_CONTENT ) return false; // drawing only content layer

      const float PI = 3.141592653f;

      float w = float(params.area.right - params.area.left);
      float h = float(params.area.bottom - params.area.top);

      float scale = w < h? w / 300.0f: h / 300.0f;

      time_t rawtime;
      time (&rawtime);
      struct tm timeinfo = *localtime (&rawtime);

      sciter::graphics gfx(params.gfx);
      gfx.state_save();

      gfx.translate(params.area.left + w / 2.0f,params.area.top + h / 2.0f);
      gfx.scale(scale,scale);    
      gfx.rotate(-PI/2);
      gfx.line_color(0);
      gfx.line_width(8.f);
      gfx.line_cap(SCITER_LINE_CAP_ROUND);
       
      // Hour marks
      gfx.state_save();
        gfx.line_color(gcolor(0x32,0x5F,0xA2));
        for (int i = 0; i < 12; ++i) {
          gfx.rotate(PI/6);
          gfx.line(137.f,0,144.f,0);
        }
      gfx.state_restore();

      // Minute marks
      gfx.state_save();
        gfx.line_width(3.f);
        gfx.line_color(gcolor(0xA5,0x2A, 0x2A));
        for (int i = 0; i < 60; ++i) {
          if ( i % 5 != 0)
            gfx.line(143,0,146,0);
          gfx.rotate(PI/30.f);
        }
      gfx.state_restore();

      int sec = timeinfo.tm_sec;
      int min = timeinfo.tm_min;
      int hr  = timeinfo.tm_hour;
      hr = hr >= 12 ? hr - 12 : hr;
  
      // draw Hours
      gfx.state_save();
        gfx.rotate( hr*(PI/6) + (PI/360)*min + (PI/21600)*sec );
        gfx.line_width(14);
        gfx.line_color(gcolor(0x32,0x5F,0xA2));
        gfx.line(-20,0,70,0);
        gfx.state_restore();

        // draw Minutes
        gfx.state_save();
        gfx.rotate( (PI/30)*min + (PI/1800)*sec );
        gfx.line_width(10);
        gfx.line_color(gcolor(0x32,0x5F,0xA2));
        gfx.line(-28,0,100,0);
      gfx.state_restore();
      
      // draw seconds
      gfx.state_save();
        gfx.rotate(sec * PI/30);
        gfx.line_color(gcolor(0xD4,0,0));
        gfx.fill_color(gcolor(0xD4,0,0));
        gfx.line_width(6);
        gfx.line(-30,0,83,0);
        gfx.ellipse(0,0,10,10);
    
        gfx.fill_color(0);
        gfx.ellipse(95,0,10,10);
      gfx.state_restore();
    
      gfx.state_restore();

      char buffer[16] = { 0 };
      sprintf(buffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

      //sciter::text text(aux::chars_of(buffer), he);
      sciter::text text = sciter::text::create_with_style(aux::a2w(buffer), he, const_wchars("font-size:24pt;color:brown"));
      gfx.draw_text(text, params.area.left + w / 2.0f, params.area.top + h / 4.0f, 5);
      
      return false;
    
    }


    // generation of Graphics.Path object on native side to be passed to script for drawing
    
    sciter::value getPath(float x, float y, float w, float h, float t, bool closed)
    {
      float samples[6];
	    float sx[6], sy[6];
	    float dx = w/5.0f;
	    
      samples[0] = (1+sinf(t*1.2345f+cosf(t*0.33457f)*0.44f))*0.5f;
	    samples[1] = (1+sinf(t*0.68363f+cosf(t*1.3f)*1.55f))*0.5f;
	    samples[2] = (1+sinf(t*1.1642f+cosf(t*0.33457f)*1.24f))*0.5f;
	    samples[3] = (1+sinf(t*0.56345f+cosf(t*1.63f)*0.14f))*0.5f;
	    samples[4] = (1+sinf(t*1.6245f+cosf(t*0.254f)*0.3f))*0.5f;
	    samples[5] = (1+sinf(t*0.345f+cosf(t*0.03f)*0.6f))*0.5f;

	    for (int i = 0; i < 6; i++) {
		    sx[i] = x+i*dx;
		    sy[i] = y+h*samples[i]*0.8f;
	    }

      // creating path:
      sciter::path p = sciter::path::create();

      p.move_to(sx[0], sy[0],false);
      for(int i = 1; i < 6; ++i)
        p.bezier_curve_to(sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i],false);

      if( closed ) {
        p.line_to(x+w,y+h,false);
        p.line_to(x+0,y+h,false);
        p.close_path();
      }
       // .line_to(points[n],points[n+1]);
     return p.to_value(); // wrap the path into sciter::value;
    }

    class IMGPainter : public sciter::painter {
      public:
      void paint(sciter::graphics& gfx, UINT width, UINT height)
      {
          gfx.line_width(3);
          gfx.line_color(sciter::gcolor(255, 0, 0));
          gfx.line(0, 0, sciter::POS(width), sciter::POS(height));
          gfx.line(sciter::POS(width), 0, 0, sciter::POS(height));
      }
    };

    sciter::value getImage(int width, int height)
    {
      BYTE *b = new BYTE[width * height * 4];
      memset(b, 127, width * height * 4);

      sciter::image img = sciter::image::create(width, height, false, b);

      IMGPainter painter;
      img.paint(&painter);
      return img.to_value();
    }

    SOM_PASSPORT_BEGIN_EX(nativeClock,native_clock)
      SOM_FUNCS(
        SOM_FUNC(getPath),
        SOM_FUNC(getImage)
      )
    SOM_PASSPORT_END

};

struct native_clock_factory: public behavior_factory {

  native_clock_factory(): behavior_factory("native-clock") { }

  // the only behavior_factory method:
  virtual event_handler* create(HELEMENT he) {
    return new native_clock();
  }

};

// instantiating and attaching it to the global list
native_clock_factory native_clock_factory_instance;


}
