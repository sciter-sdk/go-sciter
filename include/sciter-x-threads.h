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
 * Asynchronous GUI Task Queue.
 * Use these primitives when you need to run code in GUI thread.
 */


#if !defined(__SCITER_THREADS_H__)
#define __SCITER_THREADS_H__

#if defined(WINDOWS)
  #include <specstrings.h>
  #include <windows.h>
  #include <stdio.h>
  #include <assert.h>
  #include <functional>

  #ifndef THIS_HINSTANCE
  EXTERN_C IMAGE_DOS_HEADER __ImageBase;
  #define THIS_HINSTANCE ((HINSTANCE)&__ImageBase)
  #endif


  namespace sciter {

    namespace sync {

      class mutex
      {
        CRITICAL_SECTION cs;
     public:
        void lock()     { EnterCriticalSection(&cs); }
        void unlock()   { LeaveCriticalSection(&cs); }
        mutex()         { InitializeCriticalSection(&cs); }
        ~mutex()        { DeleteCriticalSection(&cs); }
      };

      class critical_section {
        mutex& m;
      public:
        critical_section(mutex& guard) : m(guard) { m.lock(); }
        ~critical_section() { m.unlock(); }
      };

      struct event
      {
        HANDLE h;
        event()       { h = CreateEvent(NULL, FALSE, FALSE, NULL);  }
        ~event()      { CloseHandle(h); }
        void signal()                   { SetEvent(h); }
        bool wait(unsigned int ms = INFINITE)   { return WaitForSingleObject(h, ms) == WAIT_OBJECT_0; }
      private:
        event( const event& );
        event& operator=( const event& );
      };

      inline void yield() { Sleep(0); }
      inline void sleep(unsigned ms) { Sleep(ms); }

      class gui_thread_ctx
      {
        HHOOK _hook;

        typedef std::function<void(void)> gui_block;

        static DWORD thread_id()
        {
          static DWORD _thread_id = ::GetCurrentThreadId();
          return _thread_id;
        }
        static UINT message()
        {
          static UINT _message = ::RegisterWindowMessage( TEXT("GUI-THREAD-EXEC_RQ"));
          return _message;
        }

        void install_hook()
        {
          message(); // force message to be registered
          // setup the WH_GETMESSAGE hook.
          // Using hooks here allows this mechanism to work even under modal dialogs.
          _hook = ::SetWindowsHookEx(WH_GETMESSAGE,&exec_hook,THIS_HINSTANCE, thread_id());
        }
        void release_hook()
        {
          if(_hook)  ::UnhookWindowsHookEx(_hook);
        }

        // message hook to handle WM_EXEC in GUI thread
        static LRESULT CALLBACK exec_hook(int code, WPARAM wParam, LPARAM lParam )
        {
          MSG* pmsg = reinterpret_cast<MSG*>(lParam);
          if(wParam == PM_REMOVE && pmsg->message == message())
          {
            event* pe = reinterpret_cast<sciter::sync::event*>(pmsg->wParam);
            gui_block* pf = reinterpret_cast<gui_block*>(pmsg->lParam);
            (*pf)();      // execute the block in this GUI thread
            pe->signal(); // signal that we've done with it
                          // this will resume execution of worker thread.
          }
          return CallNextHookEx(0,code, wParam,lParam);
        }

      public:
        gui_thread_ctx() { install_hook(); }
        ~gui_thread_ctx() { release_hook(); }

        // this function is called from worker threads to
        // execute the gui_block in GUI thread
        static void exec( gui_block code )
        {
          event evt;
          PostThreadMessage(thread_id(), message(), WPARAM(&evt),LPARAM(&code));
          evt.wait(); // suspend worker thread until GUI will execute the block.
        }

      };

      #define GUI_CODE_START sciter::sync::gui_thread_ctx::exec([&]() {
      #define GUI_CODE_END });

    }

    template<typename F, typename P>
    class thread_ctx
    {
      F _f;
      P _p;
      static DWORD WINAPI ThreadProc(LPVOID lpData)
      {
          thread_ctx* self = reinterpret_cast<thread_ctx*>(lpData);
          try {
            self->_f(self->_p);
          }
          catch(...) {
            assert(false);
          }
          delete self;
          return 0;
      }
      thread_ctx(const F& f, const P& p): _f(f),_p(p) {

        DWORD dwThreadID;
		    HANDLE hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, &dwThreadID);
		    assert(hThread != NULL);
        ::CloseHandle(hThread);
      }
    public:
      static void start( const F& f, const P& p )
      {
        new thread_ctx<F,P>(f,p);
      }
    };

    template<typename F, typename P>
      inline void thread( F f, P p )
      {
        thread_ctx<F,P>::start(f,p);
      }

  }

#else

  #include <mutex>
  #include <condition_variable>
  #include <thread>
  #include <chrono>

  namespace sciter {

    namespace sync {

      class mutex :public std::recursive_mutex
      {
          friend class event;
          typedef std::recursive_mutex super;
          mutex( const mutex& ) = delete;
          mutex& operator=(const mutex&) = delete;
      public:
          mutex():super() {}
          //void lock() { super::lock(); }
          //void unlock() { super::unlock(); }
      };

      class critical_section: public std::lock_guard<mutex>
      {
          typedef std::lock_guard<mutex> super;
          critical_section(const critical_section& rs) = delete;
          critical_section& operator=(const critical_section&) = delete;

      public:
          critical_section(mutex& m) : super(m) {}
          ~critical_section() {}
      };

      class event
      {
        //typedef std::condition_variable super;
        event( const event& ) = delete;
        event& operator=(const event&) = delete;

        std::condition_variable_any _var;
        std::mutex                  _mtx;
      public:
        event() {}
        void signal() {
            _var.notify_one();
        }
        void wait(mutex& m) {
           std::unique_lock<mutex> lock(m);
           _var.wait(lock);
        }
        void wait(unsigned int ms = unsigned(-1)) {
            std::unique_lock<std::mutex> lock(_mtx);
            if( ms == unsigned(-1) )
              _var.wait(lock);
            else
              _var.wait_for(lock, std::chrono::milliseconds(ms));
        }

      };

      inline void sleep(uint ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
      inline void yield() { std::this_thread::yield(); }

     }
     template<typename F, typename P>
     inline void thread( F f, P p )
     {
          new std::thread(f,p); // will leak the instance?
     }

  }

#endif

#endif // __SCITER_THREADS_H__
