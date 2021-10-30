
#include "camera-capture.h"

namespace camera {


  void device_list::clear()
  {
      for (unsigned i = 0; i < ndevices; i++)
      {
        if(pp_devices[i]) {
          pp_devices[i]->Release();
          pp_devices[i] = nullptr;
        }
      }
      CoTaskMemFree(pp_devices);
      pp_devices = NULL;
      ndevices = 0;
  }

  bool device_list::enumerate_devices()
  {
      HRESULT hr = S_OK;
      
      com::ptr<IMFAttributes> pAttributes;

      clear();

      // Initialize an attribute store. We will use this to 
      // specify the enumeration parameters.

      hr = MFCreateAttributes(pAttributes.target(), 1);

      // Ask for source type = video capture devices
      if (SUCCEEDED(hr))
      {
          hr = pAttributes->SetGUID(
              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, 
              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
              );
      }

      // Enumerate devices.
      if (SUCCEEDED(hr))
      {
          hr = MFEnumDeviceSources(pAttributes, &pp_devices, &ndevices);
      }

      return SUCCEEDED(hr);
  }


  bool device_list::get_device(unsigned index, com::ptr<IMFActivate>& pActivate)
  {
      if (index >= count())
          return false;

      pActivate = pp_devices[index];

      return true;
  }

  bool device_list::get_device(const sciter::string& name, com::ptr<IMFActivate>& pActivate)
  {
    for(unsigned n = 0; n < count(); ++n) {
      sciter::string nname;
      get_device_name(n, nname);
      if(  name == nname )
        return get_device(n,pActivate);
    }
    return false;
  }


  bool device_list::get_device_name(unsigned index, sciter::string& name)
  {
      if (index >= count())
      {
          return false;
      }

      HRESULT hr = S_OK;

      WCHAR *pszName = 0;

      hr = pp_devices[index]->GetAllocatedString(
          MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, 
          &pszName, 
          NULL
          );

      if( pszName && *pszName ) {
        name = pszName;
      }

      return SUCCEEDED(hr);
  }



  //-------------------------------------------------------------------
  //  CreateInstance
  //
  //  Static class method to create the CCapture object.
  //-------------------------------------------------------------------

  capture* capture::create_instance( sciter::video_destination* pdst, const sciter::value& source_name_or_index )
  {
      encoding_parameters params;
      params.bitrate = 240 * 1000;
      params.subtype = MFVideoFormat_WMV3;

      device_list devices;
      devices.enumerate_devices();
      if( devices.count() == 0 ) {
        // no cameras
        //pdst->stop_streaming();
        return nullptr;
      }

      com::ptr<IMFActivate> pActivator;

      if( source_name_or_index.is_int() )
        devices.get_device( source_name_or_index.get(0),pActivator);
      else if( source_name_or_index.is_string() )
        devices.get_device( source_name_or_index.get(L""),pActivator);
      else 
        return nullptr;

      if(!pActivator)
        return nullptr;

      capture* pcapt = new capture(pdst);

      if (pcapt)
        pcapt->start_capture(pActivator, params);

      return pcapt;

  }


  //-------------------------------------------------------------------
  //  constructor
  //-------------------------------------------------------------------

  capture::capture(sciter::video_destination* pdst ) : 
      m_pReader(NULL),
      m_nRefCount(0),
      m_bFirstSample(FALSE),
      m_llBaseTime(0),
      m_pwszSymbolicLink(NULL),
      m_width(0),
      m_height(0),
      m_colorSpace(sciter::COLOR_SPACE_UNKNOWN),
      dest(pdst)
  {
  }

  //-------------------------------------------------------------------
  //  destructor
  //-------------------------------------------------------------------

  capture::~capture()
  {
      assert(m_pReader == nullptr);
  }

  /////////////// IUnknown methods ///////////////

  //-------------------------------------------------------------------
  //  AddRef
  //-------------------------------------------------------------------

  ULONG capture::AddRef()
  {
      return InterlockedIncrement(&m_nRefCount);
  }


  //-------------------------------------------------------------------
  //  Release
  //-------------------------------------------------------------------

  ULONG capture::Release()
  {
      ULONG uCount = InterlockedDecrement(&m_nRefCount);
      if (uCount == 0)
      {
          delete this;
      }
      return uCount;
  }



  //-------------------------------------------------------------------
  //  QueryInterface
  //-------------------------------------------------------------------

  HRESULT capture::QueryInterface(REFIID riid, void** ppv)
  {
      static const QITAB qit[] = 
      {
          QITABENT(capture, IMFSourceReaderCallback),
          { 0 },
      };
      return QISearch(this, qit, riid, ppv);
  }


  /////////////// IMFSourceReaderCallback methods ///////////////

  //-------------------------------------------------------------------
  // OnReadSample
  //
  // Called when the IMFMediaSource::ReadSample method completes.
  //-------------------------------------------------------------------

  HRESULT capture::OnReadSample(
      HRESULT hrStatus,
      DWORD /*dwStreamIndex*/,
      DWORD /*dwStreamFlags*/,
      LONGLONG llTimeStamp,
      IMFSample *pSample      // Can be NULL
      )
  {
      sciter::sync::critical_section _(m_lock);

      if (!is_capturing() || !dest)
      {
          return S_OK;
      }

      HRESULT hr = S_OK;

      if (FAILED(hrStatus))
      {
          hr = hrStatus;
          goto done;
      }

      if (pSample)
      {
          if (m_bFirstSample)
          {
              m_llBaseTime = llTimeStamp;
              m_bFirstSample = FALSE;
              dest->start_streaming(m_width,m_height,m_colorSpace);
          }

          // rebase the time stamp
          llTimeStamp -= m_llBaseTime;

          hr = pSample->SetSampleTime(llTimeStamp);

          if (FAILED(hr)) 
            goto done;

          com::ptr<IMFMediaBuffer> pBuffer;

          hr = pSample->ConvertToContiguousBuffer(pBuffer.target());
          if (FAILED(hr)) 
            goto done; 

          LPBYTE data; DWORD maxLength, length;

          hr = pBuffer->Lock(&data,&maxLength,&length);
          if (FAILED(hr))
            goto done; 

          if(!dest->render_frame(data,length)) { 
            pBuffer->Unlock();
            goto done; 
          }
          pBuffer->Unlock();
          if (FAILED(hr)) 
            goto done;
      }

      // Read another sample.
      hr = m_pReader->ReadSample(
          (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
          0,
          NULL,   // actual
          NULL,   // flags
          NULL,   // timestamp
          NULL    // sample
          );

  done:
      if (FAILED(hr))
      {
        if(dest) 
          dest->stop_streaming();
      }
      return hr;
  }


  //-------------------------------------------------------------------
  // OpenMediaSource
  //
  // Set up preview for a specified media source. 
  //-------------------------------------------------------------------

  HRESULT capture::OpenMediaSource(IMFMediaSource *pSource)
  {
      HRESULT hr = S_OK;

      com::ptr<IMFAttributes> pAttributes;

      hr = MFCreateAttributes(pAttributes.target(), 2);

      if (SUCCEEDED(hr))
      {
          hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
      }

      if (SUCCEEDED(hr))
      {
          hr = MFCreateSourceReaderFromMediaSource(
              pSource,
              pAttributes,
              m_pReader.target()
              );
      }
      return hr;
  }


  //-------------------------------------------------------------------
  // StartCapture
  //
  // Start capturing.
  //-------------------------------------------------------------------

  bool capture::start_capture(
      IMFActivate *pActivate, 
      const encoding_parameters& param
      )
  {
      end_capture();

      HRESULT hr = S_OK;

      com::ptr<IMFMediaSource> pSource;

      sciter::sync::critical_section _(m_lock);

      // Create the media source for the device.
      hr = pActivate->ActivateObject(
          __uuidof(IMFMediaSource), 
          (void**) pSource.target()
          );

      // Get the symbolic link. This is needed to handle device-
      // loss notifications. (See CheckDeviceLost.)

      if (SUCCEEDED(hr))
      {
          hr = pActivate->GetAllocatedString(
              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
              &m_pwszSymbolicLink,
              NULL
              );
      }

      if (SUCCEEDED(hr))
      {
          hr = OpenMediaSource(pSource);
      }

      // Set up the encoding parameters.
      if (SUCCEEDED(hr))
      {
          hr = ConfigureCapture(param);
      }

      if (SUCCEEDED(hr))
      {
          m_bFirstSample = TRUE;
          m_llBaseTime = 0;

          // Request the first video frame.

          hr = m_pReader->ReadSample(
              (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
              0,
              NULL,
              NULL,
              NULL,
              NULL
              );
      }
      return SUCCEEDED(hr);
  }


  //-------------------------------------------------------------------
  // EndCaptureSession
  //
  // Stop the capture session. 
  //
  // NOTE: This method resets the object's state to State_NotReady.
  // To start another capture session, call SetCaptureFile.
  //-------------------------------------------------------------------

  bool capture::end_capture()
  {
      sciter::sync::critical_section _(m_lock);
      if(m_pwszSymbolicLink) {
        CoTaskMemFree(m_pwszSymbolicLink);
        m_pwszSymbolicLink = nullptr;
      }
      m_pReader = nullptr;
      if(dest)
        dest->stop_streaming();
      return true;
  }


  bool capture::is_capturing() 
  { 
      sciter::sync::critical_section _(m_lock);

      return dest && dest->is_alive();
  
  }

  //-------------------------------------------------------------------
  //  check_device_lost
  //  Checks whether the video capture device was removed.
  //
  //  The application calls this method when is receives a 
  //  WM_DEVICECHANGE message.
  //-------------------------------------------------------------------

  bool capture::check_device_lost(DEV_BROADCAST_HDR *pHdr, bool& deviceLost)
  {
      sciter::sync::critical_section _(m_lock);

      DEV_BROADCAST_DEVICEINTERFACE *pDi = NULL;

      deviceLost = false;
    
      if (!is_capturing())
      {
          return true;
      }
      if (pHdr == NULL)
      {
          return true;
      }
      if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
      {
          return true;
      }

      // Compare the device name with the symbolic link.

      pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;

      if (m_pwszSymbolicLink)
      {
          if (_wcsicmp(m_pwszSymbolicLink, pDi->dbcc_name) == 0)
          {
              deviceLost = true;
          }
      }

      return true;
  }


  /////////////// Private/protected class methods ///////////////



  //-------------------------------------------------------------------
  //  ConfigureSourceReader
  //
  //  Sets the media type on the source reader.
  //-------------------------------------------------------------------

  HRESULT ConfigureSourceReader(IMFSourceReader *pReader, sciter::COLOR_SPACE& color_space)
  {
      color_space = sciter::COLOR_SPACE_UNKNOWN;
      // The list of acceptable types.
      GUID subtypes[] = { 
          MFVideoFormat_NV12, 
          MFVideoFormat_YUY2, 
          MFVideoFormat_RGB32, 
          MFVideoFormat_RGB24, 
          MFVideoFormat_IYUV
      };

      sciter::COLOR_SPACE color_spaces[] = {
        sciter::COLOR_SPACE_NV12, 
        sciter::COLOR_SPACE_YUY2,
        sciter::COLOR_SPACE_RGB32,
        sciter::COLOR_SPACE_RGB24,
        sciter::COLOR_SPACE_IYUV // a.k.a. I420  
      };

      HRESULT hr = S_OK;

      GUID subtype = { 0 };

      com::ptr<IMFMediaType> pType;

      // If the source's native format matches any of the formats in 
      // the list, prefer the native format.

      // Note: The camera might support multiple output formats, 
      // including a range of frame dimensions. The application could
      // provide a list to the user and have the user select the
      // camera's output format. That is outside the scope of this
      // sample, however.

      hr = pReader->GetNativeMediaType(
          (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
          0,  // Type index
          pType.target()
          );

      if (FAILED(hr)) { goto done; }

      hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);

      if (FAILED(hr)) { goto done; }

      for (UINT32 i = 0; i < ARRAYSIZE(subtypes); i++)
      {
          if (subtype == subtypes[i])
          {
              hr = pReader->SetCurrentMediaType(
                  (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 
                  NULL, 
                  pType
                  );

              color_space = color_spaces[i];
              break;
          }
      }

  done:
      if(FAILED(hr))
        return hr;
      return color_space != sciter::COLOR_SPACE_UNKNOWN ? S_OK : E_FAIL;
  }

  //-------------------------------------------------------------------
  // ConfigureCapture
  //
  // Configures the capture session.
  //
  //-------------------------------------------------------------------

  HRESULT capture::ConfigureCapture(const encoding_parameters& param)
  {
      HRESULT hr = S_OK;
      DWORD sink_stream = 0;

      com::ptr<IMFMediaType> pType;

      hr = ConfigureSourceReader(m_pReader, m_colorSpace);

      if (SUCCEEDED(hr))
      {
          hr = m_pReader->GetCurrentMediaType(
              (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 
              pType.target()
              );
      }

      if (SUCCEEDED(hr))
      {
          // Register the color converter DSP for this process, in the video 
          // processor category. This will enable the sink writer to enumerate
          // the color converter when the sink writer attempts to match the
          // media types.

          hr = MFTRegisterLocalByCLSID(
              __uuidof(CColorConvertDMO),
              MFT_CATEGORY_VIDEO_PROCESSOR,
              L"",
              MFT_ENUM_FLAG_SYNCMFT,
              0,
              NULL,
              0,
              NULL
              );

           hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &m_width, &m_height);
           assert(hr != MF_E_ATTRIBUTENOTFOUND);

      }

      return hr;
  }

}
 