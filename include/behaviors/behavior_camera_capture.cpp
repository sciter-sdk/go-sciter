#include "stdafx.h"
#include "sciter-x-dom.hpp"
#include "sciter-x-behavior.h"

#if TRUE // change it to FALSE to disable camera functionality

#include "camera/camera-capture.h"
#include "camera/camera-capture.cpp"

namespace sciter 
{
/*

BEHAVIOR: camera_stream
   provides video frames from camera 
COMMENTS: 
   <video style="behavior:camera video" />
SAMPLE:
   See: samples/video/video-camera-behavior.htm
*/

struct camera_stream: public event_handler
{
    com::ptr<camera::capture> pcapt;
    sciter::om::hasset<sciter::video_destination> rendering_site;
    // ctor
    camera_stream() {}
    virtual ~camera_stream() {}

    virtual bool subscription( HELEMENT he, UINT& event_groups )
    {
      event_groups = HANDLE_BEHAVIOR_EVENT; // we only handle VIDEO_BIND_RQ here
      return true;
    }

    virtual void attached  (HELEMENT he ) { } 
    virtual void detached  (HELEMENT he ) { asset_release(); } 

    virtual bool on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason ) 
    { 
      if(type != VIDEO_BIND_RQ)
        return false;
      // we handle only VIDEO_BIND_RQ requests here

      if( !reason )
        return true; // first phase, consume the event to mark as we will provide frames 
      
      rendering_site = (sciter::video_destination*) reason;
      return true;
    }

    sciter::value get_devices() {
      camera::device_list devices;
      devices.enumerate_devices();
      sciter::value r;
      for( unsigned n = 0; n < devices.count(); ++n ) {
        sciter::string name;
        if(devices.get_device_name(n,name))
          r.append( sciter::value(name) );
      }
      return r;
    }

    sciter::value stream_from( const sciter::value& device ) // either int (index) or string (name)
    {
      if(pcapt)
        pcapt->end_capture();
      pcapt = camera::capture::create_instance(rendering_site,device);
      return sciter::value(true);
    }
    
	// scripting methods
    BEGIN_FUNCTION_MAP
      FUNCTION_0("devices",get_devices)    // devices() : (array of strings), get list of names of devices 
      FUNCTION_1("streamFrom",stream_from) // streamFrom(indexOrName: int | string), start streaming from the camera
    END_FUNCTION_MAP
   
};

struct camera_stream_factory: public behavior_factory {

  camera_stream_factory(): behavior_factory("camera") {}

  // the only behavior_factory method:
  virtual event_handler* create(HELEMENT he) { return new camera_stream(); }

};

// instantiating and attaching it to the global list
camera_stream_factory camera_stream_factory_instance;


}

#endif