/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * (C) Terra Informatica Software, Inc.
 */

/*
 * Sciter's get resource request object - represents requests made by Element/View.request() functions.
 * 
 *
 * C++ wrapper
 */


#ifndef __sciter_request_hpp__
#define __sciter_request_hpp__

#include "sciter-x-request.h"
#include "sciter-x-api.h"

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

#include "aux-slice.h"

namespace sciter
{

    inline VOID SC_CALLBACK _LPCWSTR2STRING( LPCWSTR str, UINT str_length, LPVOID param )
    {
        sciter::string* s = (sciter::string*)param;
        *s = sciter::string(str,str_length);
    }
    inline VOID SC_CALLBACK _LPCSTR2STRING( LPCSTR str, UINT str_length, LPVOID param )
    {
        sciter::astring* s = (sciter::astring*)param;
        *s = sciter::astring(str,str_length);
    }  


/*  struct writer
  {
    virtual bool write( aux::bytes data ) = 0; // redefine to do actual writing of data.start/data.length
    static SBOOL SCAPI image_write_function(LPVOID prm, const BYTE* data, UINT data_length)
    {
      writer* pw = (writer* )prm;
      return pw->write( aux::bytes(data,data_length) );
    }
  }; */

  class request
  {
  protected:
    HREQUEST hrq;
    
    request(): hrq(0) { ; }
    
  public:
  
    request(HREQUEST h): hrq(h) { if(hrq) rapi()->RequestUse(hrq); }    
    request(const request& im): hrq(im.hrq) { if(hrq) rapi()->RequestUse(hrq); }
    request& operator = (const request& im) 
    { 
      if(hrq) rapi()->RequestUnUse(hrq); 
      hrq = im.hrq; rapi()->RequestUse(hrq); 
      return *this;
    }
    
    bool is_valid() const { return hrq != 0; }

    ~request() {
      rapi()->RequestUnUse( hrq ); 
    }
    
    sciter::astring url() const {
      sciter::astring rv; 
      rapi()->RequestUrl( hrq, _LPCSTR2STRING, &rv ); 
      return rv;
    }

    sciter::astring content_url()  const {
      sciter::astring rv; 
      rapi()->RequestContentUrl( hrq, _LPCSTR2STRING, &rv ); 
      return rv;
    }
    
    SciterResourceType requested_type()  const {
      SciterResourceType rv = SciterResourceType();
      rapi()->RequestGetRequestedDataType( hrq, &rv );
      return rv;
    }
    
    void succeeded( UINT status, LPCBYTE dataOrNull = NULL, UINT dataLength = 0 )  const
    {
      rapi()->RequestSetSucceeded( hrq, status, dataOrNull, dataLength);
    }

    void failed( UINT status, LPCBYTE dataOrNull = NULL, UINT dataLength = 0 )  const
    {
      rapi()->RequestSetFailed( hrq, status, dataOrNull, dataLength);
    }

    void append_data( LPCBYTE data, UINT dataLength)
    {
      rapi()->RequestAppendDataChunk( hrq, data, dataLength);
    }
    
  };

}

#endif //defined(__cplusplus) && !defined( PLAIN_API_ONLY )


#endif

