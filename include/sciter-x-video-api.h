/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */

/*
 * Sciter's custom video rendering primitives
 */

#pragma once

#include "aux-asset.h"
#include "sciter-x-types.h"

namespace sciter {

  enum COLOR_SPACE {
    COLOR_SPACE_UNKNOWN,
    COLOR_SPACE_YV12,
    COLOR_SPACE_IYUV, // a.k.a. I420  
    COLOR_SPACE_NV12,
    COLOR_SPACE_YUY2,
    COLOR_SPACE_RGB24,
    COLOR_SPACE_RGB555,
    COLOR_SPACE_RGB565,
    COLOR_SPACE_RGB32 // with alpha, sic!
  };

  #define VIDEO_SOURCE_INAME "source.video.sciter.com"

  struct video_source : public sciter::om::iasset<video_source>
  {
    virtual bool play() = 0;
    virtual bool pause() = 0;
    virtual bool stop() = 0;
    virtual bool get_is_ended(bool& eos) = 0;
    virtual bool get_position(double& seconds) = 0;
    virtual bool set_position(double seconds) = 0;
    virtual bool get_duration(double& seconds) = 0;
    // audio
    virtual bool get_volume(double& vol) = 0;
    virtual bool set_volume(double vol) = 0;
    virtual bool get_balance(double& vol) = 0;
    virtual bool set_balance(double vol) = 0;
  };

  #define VIDEO_DESTINATION_INAME "destination.video.sciter.com"

  // video_destination interface, represents video rendering site 
  struct video_destination : public sciter::om::iasset<video_destination>
  {
     // true if this instance of video_renderer is attached to DOM element and is capable of playing.
     virtual bool is_alive() = 0; 

     // start streaming/rendering 
     virtual bool start_streaming( int frame_width        // width
                                 , int frame_height       // height 
                                 , int color_space        // COLOR_SPACE above
                                 , video_source* src = 0 ) = 0;  // video_source interface implementation, can be null

     // stop streaming, eof.
     virtual bool stop_streaming() = 0;

     // render frame request, false - video_destination is not available ( isn't alive, document unloaded etc.) 
     virtual bool render_frame(const BYTE* frame_data, UINT frame_data_size) = 0;
     virtual bool render_frame_with_stride(const BYTE* frame_data, UINT frame_data_size, UINT stride) = 0;

  };

  #define FRAGMENTED_VIDEO_DESTINATION_INAME "fragmented.destination.video.sciter.com"

  struct fragmented_video_destination : public video_destination 
  {
     // render frame part request, returns false - video_destination is not available ( isn't alive, document unloaded etc.) 
     virtual bool render_frame_part(const BYTE* frame_data, UINT frame_data_size, int x, int y, int width, int height) = 0;

  };


}