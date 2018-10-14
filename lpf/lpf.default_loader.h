#ifndef __LPF_STANDARD_LOADER__
#define __LPF_STANDARD_LOADER__

#include "lpf.loader.h"

namespace lpf
{
	class default_loader_factory : public loader_factory
	{
		int hint_;
	public:
		static const int use_best;
		static const int use_loadlibrary;
		static const int use_dlopen;
		static const int use_dld_link;
		static const int use_dyld;
		static const int use_shl_load;
	public:
		default_loader_factory(const int& hint = use_best);
	public:
		static std::shared_ptr<loader_factory>
				create(const int& hint = use_best);
	public:
		std::shared_ptr<loader> instance(void) const;
	};

#if defined(LPF_WIN32)
	typedef DWORD(WINAPI getthreaderrormode_t)(void);
	typedef BOOL(WINAPI setthreaderrormode_t)(DWORD, DWORD *);

	class win32_loadlibrary : public loader
	{
		getthreaderrormode_t* get_thread_error_mode;
		setthreaderrormode_t* set_thread_error_mode;
	public:
		win32_loadlibrary(void);
	public:
		static std::shared_ptr<loader> create(void);
	public:
		std::string name(void) const;
	public:
		std::shared_ptr<module> open(const char* name) const;
	};

	class win32_loadlibrary_module : public module
	{
		HMODULE module_;
	public:
		win32_loadlibrary_module(HMODULE mod);
		~win32_loadlibrary_module(void);
	public:
		bool close(void) const;
		void* symbol(const char* name) const;
	};
#endif

#if defined(LPF_HAVE_DLOPEN)
	class dlopen_loader : public loader
	{
	public:
		dlopen_loader(void);
	public:
		static std::shared_ptr<loader> create(void);
	public:
		std::string name(void) const;
	public:
		std::shared_ptr<module> open(const char* name) const;
	};

	class dlopen_loader_module : public module
	{
		void* handle_;
	public:
		dlopen_loader_module(void* handle);
		~dlopen_loader_module(void);
	public:
		bool close(void) const;
		void* symbol(const char* name) const;
	};
#endif

#if defined(LPF_HAVE_DLD_LINK)
	class dld_link_loader : public loader
	{
	public:
		dld_link_loader(void);
	public:
		static std::shared_ptr<loader> create(void);
	public:
		std::string name(void) const;
	public:
		std::shared_ptr<module> open(const char* name) const;
	};
#endif

#if defined(LPF_HAVE_DYLD)
	class dyld_loader : public loader
	{
	public:
		dyld_loader(void);
	public:
		static std::shared_ptr<loader> create(void);
	public:
		std::string name(void) const;
	public:
		std::shared_ptr<module> open(const char* name) const;
	};
#endif

#if defined(LPF_HAVE_SHL_LOAD)
	class shl_load_loader : public loader
	{
	public:
		shl_load_loader(void);
	public:
		static std::shared_ptr<loader> create(void);
	public:
		std::string name(void) const;
	public:
		std::shared_ptr<module> open(const char* name) const;
	};
#endif
}

#endif

