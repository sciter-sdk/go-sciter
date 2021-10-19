
#pragma once

#include <new>
#include <windows.h>
#include <comip.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <Wmcodecdsp.h>
#include <assert.h>
#include <Dbt.h>
#include <shlwapi.h>
#include "sciter-x.h"
#include "sciter-x-threads.h"
#include "sciter-x-video-api.h"
#include "aux-asset.h"

// note mf.dll & friends does not exist on all Windows versions by default 

#pragma comment (lib, "delayimp")
#pragma comment (lib, "mfplat.lib")
#pragma comment (lib, "mf.lib")
#pragma comment (lib, "mfreadwrite.lib")
#pragma comment (lib, "mfuuid.lib")
#pragma comment (lib, "shlwapi.lib")

//#pragma comment (linker, "/delayload:mf.dll")
//#pragma comment (linker, "/delayload:mfplat.dll")
//#pragma comment (linker, "/delayload:mfreadwrite.dll")
//#pragma comment (linker, "/delayload:shlwapi.dll")

namespace camera {

	class device_list
	{
		UINT32        ndevices;
		IMFActivate **pp_devices;

	public:
		device_list() : pp_devices(NULL), ndevices(0)
		{
      static bool mf_initialized = false;
      if(!mf_initialized) {
        // Initialize Media Foundation
        MFStartup(MF_VERSION);
        mf_initialized = true;
      }
		}
		~device_list()
		{
			clear();
		}
		
		unsigned count() const { return ndevices; }

		void clear();
		bool enumerate_devices();
		bool get_device(unsigned index, com::ptr<IMFActivate>& pActivate);
    bool get_device(const sciter::string& name, com::ptr<IMFActivate>& pActivate);
		bool get_device_name(unsigned index, sciter::string& name);
	};

	struct encoding_parameters
	{
		GUID      subtype;
		unsigned  bitrate;
	};

	class capture : public IMFSourceReaderCallback
	{
	public:
    static capture* create_instance(sciter::video_destination* pdst, const sciter::value& source_name_or_index);

		// IUnknown methods
		STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		// IMFSourceReaderCallback methods
		STDMETHODIMP OnReadSample(
			HRESULT hrStatus,
			DWORD dwStreamIndex,
			DWORD dwStreamFlags,
			LONGLONG llTimestamp,
			IMFSample *pSample
		);

		STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *)
		{
			return S_OK;
		}

		STDMETHODIMP OnFlush(DWORD)
		{
			return S_OK;
		}

    // methods:

		bool     start_capture(IMFActivate *pActivate, const encoding_parameters& param);
		bool     end_capture();
		bool     is_capturing();
		bool     check_device_lost(DEV_BROADCAST_HDR *pHdr, bool& deviceLost);

	protected:

		enum State
		{
			State_NotReady = 0,
			State_Ready,
			State_Capturing,
		};
		
		// Constructor is private. Use static CreateInstance method to instantiate.
		capture(sciter::video_destination* pdst);

		// Destructor is private. Caller should call Release.
		virtual ~capture();

		void    NotifyError(HRESULT hr) { /*PostMessage(m_hwndEvent, WM_APP_PREVIEW_ERROR, (WPARAM)hr, 0L);*/ }

		HRESULT OpenMediaSource(IMFMediaSource *pSource);
		HRESULT ConfigureCapture(const encoding_parameters& param);

		long                      m_nRefCount;        // Reference count.

    sciter::sync::mutex       m_lock;

		com::ptr<IMFSourceReader> m_pReader;

		BOOL                      m_bFirstSample;
		LONGLONG                  m_llBaseTime;

		WCHAR                    *m_pwszSymbolicLink;

    sciter::COLOR_SPACE       m_colorSpace;
    unsigned                  m_width;
    unsigned                  m_height;

    sciter::om::hasset<sciter::video_destination> dest;

	};

}