
//#include "stdafx.h"
#include "sciter-x.h"
#include "sciter-x-behavior.h"
#include "sciter-x-threads.h"
#include "sciter-x-video-api.h"

namespace sciter
{
  /*
  BEHAVIOR: video_generated_stream
    - provides synthetic video frames.
    - this code is here solely for the demo purposes - how
      to connect your own video frame stream with the rendering site

  COMMENTS:
     <video style="behavior:video-generator video" />
  SAMPLE:
     See: samples/video/video-generator-behavior.htm
  */

  struct video_generated_stream : public event_handler
  {
    sciter::om::hasset<sciter::video_destination> rendering_site;
    // ctor
    video_generated_stream() {}
    virtual ~video_generated_stream() {}

    virtual bool subscription(HELEMENT he, UINT& event_groups)
    {
      event_groups = HANDLE_BEHAVIOR_EVENT; // we only handle VIDEO_BIND_RQ here
      return true;
    }

    virtual void attached(HELEMENT he) {
      he = he;
    }

    virtual void detached(HELEMENT he) { asset_release(); }
    virtual bool on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason)
    {
      if (type != VIDEO_BIND_RQ)
        return false;
      // we handle only VIDEO_BIND_RQ requests here

      //printf("VIDEO_BIND_RQ %d\n",reason);

      if (!reason)
        return true; // first phase, consume the event to mark as we will provide frames

      rendering_site = (sciter::video_destination*) reason;
      sciter::om::hasset<sciter::fragmented_video_destination> fsite;

      if (rendering_site->asset_get_interface(FRAGMENTED_VIDEO_DESTINATION_INAME, fsite.target()))
      {
        sciter::thread(generation_thread, fsite);
      }

      return true;
    }

    static void generation_thread(sciter::fragmented_video_destination* dst) {
      sciter::om::hasset<sciter::fragmented_video_destination> rendering_site = dst;
      // simulate video stream
      sciter::sync::sleep(100);

      const int VIDEO_WIDTH = 1200;
      const int VIDEO_HEIGHT = 800;
      const int FRAGMENT_WIDTH = 256;
      const int FRAGMENT_HEIGHT = 32;

      srand((unsigned int)(UINT_PTR)dst);

      // let's pretend that we have 640*480 video frames
      rendering_site->start_streaming(VIDEO_WIDTH, VIDEO_HEIGHT, COLOR_SPACE_RGB32);

      unsigned int figure[FRAGMENT_WIDTH*FRAGMENT_HEIGHT];// = {0xFFFF00FF};

      auto generate_fill_color = [&]() {
        unsigned color =
          0xff000000 |
          ((unsigned(rand()) & 0xff) << 16) |
          ((unsigned(rand()) & 0xff) << 8) |
          ((unsigned(rand()) & 0xff) << 0);
        for (int i = 0; i < FRAGMENT_WIDTH * FRAGMENT_HEIGHT; ++i)
          figure[i] = color;
      };

      generate_fill_color();

      int xpos = 0;
      int ypos = 0;
      int stepx = +1;
      int stepy = +1;

      while (rendering_site->is_alive())
      {
        sciter::sync::sleep(40); // simulate 24 FPS rate

        xpos += stepx;
        if (xpos < 0) { xpos = 0; stepx = -stepx; generate_fill_color(); }
        if (xpos >= VIDEO_WIDTH - FRAGMENT_WIDTH) { xpos = VIDEO_WIDTH - FRAGMENT_WIDTH; stepx = -stepx; generate_fill_color(); }

        ypos += stepy;
        if (ypos < 0) { ypos = 0; stepy = -stepy; generate_fill_color(); }
        if (ypos >= VIDEO_HEIGHT - FRAGMENT_HEIGHT) { ypos = VIDEO_HEIGHT - FRAGMENT_HEIGHT; stepy = -stepy; generate_fill_color(); }

        rendering_site->render_frame_part((const unsigned char*)figure, sizeof(figure), xpos, ypos, FRAGMENT_WIDTH, FRAGMENT_HEIGHT);
      }

    }

  };

  struct video_generated_stream_factory : public behavior_factory {

    video_generated_stream_factory() : behavior_factory("video-generator") {

    }

    // the only behavior_factory method:
    virtual event_handler* create(HELEMENT he) {
      return new video_generated_stream();
    }

  };

  // instantiating and attaching it to the global list
  video_generated_stream_factory video_generated_stream_factory_instance;


}
