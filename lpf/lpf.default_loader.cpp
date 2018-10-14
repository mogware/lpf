#include "lpf.config.h"
#include "lpf.default_loader.h"

#include <cstring>

#if defined(LPF_WIN32)
static DWORD WINAPI fallback_get_thread_error_mode(void)
{
	/* Prior to Windows Vista, the only way to get the current error
	mode was to set a new one. In our case, we are setting a new
	error mode right after "getting" it while ignoring the error
	mode in effect when setting the new error mode, so that's
	fairly ok. */
	return static_cast<DWORD>(::SetErrorMode(SEM_FAILCRITICALERRORS));
}

static BOOL WINAPI fallback_set_thread_error_mode(DWORD mode, DWORD* oldmode)
{
	/* Prior to Windows 7, there was no way to set the thread local error
	mode, so set the process global error mode instead. */
	DWORD old = static_cast<DWORD>(::SetErrorMode(mode));
	if (oldmode != nullptr)
		*oldmode = old;
	return TRUE;
}

lpf::win32_loadlibrary::win32_loadlibrary(void)
	: get_thread_error_mode(nullptr)
	, set_thread_error_mode(nullptr)
{
	HMODULE kernel32 = ::GetModuleHandleA("kernel32.dll");

	get_thread_error_mode = reinterpret_cast<getthreaderrormode_t *>(
		::GetProcAddress(kernel32, "GetThreadErrorMode")
		);

	if (get_thread_error_mode == nullptr)
		get_thread_error_mode = reinterpret_cast<getthreaderrormode_t *>(
			::GetProcAddress(kernel32, "GetErrorMode")
			);

	if (get_thread_error_mode == nullptr)
		get_thread_error_mode = fallback_get_thread_error_mode;

	set_thread_error_mode = reinterpret_cast<setthreaderrormode_t *>(
		::GetProcAddress(kernel32, "SetThreadErrorMode")
		);

	if (set_thread_error_mode == nullptr)
		set_thread_error_mode = fallback_set_thread_error_mode;
}

std::shared_ptr<lpf::loader> lpf::win32_loadlibrary::create(void)
{
	return std::make_shared<win32_loadlibrary>();
}

std::string lpf::win32_loadlibrary::name(void) const
{
	return "loadlibrary";
}

std::shared_ptr<lpf::module> lpf::win32_loadlibrary::open(const char* name) const
{
	char path[MAX_PATH];

	if (name == nullptr)
		::GetModuleFileNameA(nullptr, path, sizeof(path));
	else if (std::strlen(name) >= MAX_PATH)
		return nullptr;
	else
	{
#if defined(LPF_CYGWIN) && defined(LPF_HAVE_CYGWIN_CONV_PATH)
		if (::cygwin_conv_path(CCP_POSIX_TO_WIN_A, name, path, MAX_PATH))
			return nullptr;
#elif defined(LPF_CYGWIN)
		::cygwin_conv_to_full_win32_path(name, path);
#else
		std::strcpy(path, name);
#endif
	}
	DWORD errormode = get_thread_error_mode();
	set_thread_error_mode(errormode | SEM_FAILCRITICALERRORS, nullptr);

	HMODULE mod = ::LoadLibraryA(path);
	if (mod == nullptr)
		return nullptr;

	DWORD last_error = ::GetLastError();
	set_thread_error_mode(errormode, nullptr);
	::SetLastError(last_error);

	return std::make_shared<win32_loadlibrary_module>(mod);
}

lpf::win32_loadlibrary_module::win32_loadlibrary_module(HMODULE mod)
	: module_(mod)
{
}

lpf::win32_loadlibrary_module::~win32_loadlibrary_module(void)
{
	close();
}

bool lpf::win32_loadlibrary_module::close(void) const
{
	return ::FreeLibrary(module_) != 0;
}

void* lpf::win32_loadlibrary_module::symbol(const char* name) const
{
	return static_cast<void *>(::GetProcAddress(module_, name));
}
#endif

#if defined(LPF_HAVE_DLOPEN)
#if defined(RTLD_GLOBAL)
#define LPF_DL_GLOBAL		RTLD_GLOBAL
#elif defined(DL_GLOBAL)
#define LPF_DL_GLOBAL		DL_GLOBAL
#else
#define LPF_DL_GLOBAL		0
#endif

#if defined(RTLD_LAZY)
#define LPF_DL_LAZY_OR_NOW		RTLD_LAZY
#elif defined(DL_LAZY)
#define LPF_DL_LAZY_OR_NOW		DL_LAZY
#elif defined(RTLD_NOW)
#define LPF_DL_LAZY_OR_NOW		RTLD_NOW
#elif defined(DL_NOW)
#define LPF_DL_LAZY_OR_NOW		DL_NOW
#else
#define LPF_DL_LAZY_OR_NOW		0
#endif

static void *dlopen(const char *libname, int for_execution)
{
	return (void*)::LoadLibraryA(libname);
}
static int dlclose(void *handle)
{
	return ::FreeLibrary((HMODULE)handle);
}
static void *dlsym(void *handle, const char *name)
{
	return (void*)GetProcAddress((HMODULE)handle, name);
}

lpf::dlopen_loader::dlopen_loader(void)
{
}

std::shared_ptr<lpf::loader> lpf::dlopen_loader::create(void)
{
	return std::make_shared<dlopen_loader>();
}

std::string lpf::dlopen_loader::name(void) const
{
	return "dlopen";
}

std::shared_ptr<lpf::module> lpf::dlopen_loader::open(const char* name) const
{
	int flags = 0;
	void* handle = ::dlopen(name, LPF_DL_GLOBAL | LPF_DL_LAZY_OR_NOW);
	if (handle == nullptr)
		return nullptr;
	return std::make_shared<dlopen_loader_module>(handle);
};

lpf::dlopen_loader_module::dlopen_loader_module(void* handle)
	: handle_(handle)
{
}

lpf::dlopen_loader_module::~dlopen_loader_module(void)
{
	close();
}

bool lpf::dlopen_loader_module::close(void) const
{
	return ::dlclose(handle_) != 0;
}

void* lpf::dlopen_loader_module::symbol(const char* name) const
{
	return ::dlsym(handle_, name);
}
#endif

#if defined(LPF_HAVE_DLD_LINK)
lpf::dld_link_loader::dld_link_loader(void)
{
}

std::shared_ptr<lpf::loader> lpf::dld_link_loader::create(void)
{
	return std::make_shared<dld_link_loader>();
}

std::string lpf::dld_link_loader::name(void) const
{
	return "dld_link";
}

std::shared_ptr<lpf::module> lpf::dld_link_loader::open(const char* name) const
{
	return nullptr;
};
#endif

#if defined(LPF_HAVE_DYLD)
lpf::dyld_loader::dyld_loader(void)
{
}

std::shared_ptr<lpf::loader> lpf::dyld_loader::create(void)
{
	return std::make_shared<dyld_loader>();
}

std::string lpf::dyld_loader::name(void) const
{
	return "dyld";
}

std::shared_ptr<lpf::module> lpf::dyld_loader::open(const char* name) const
{
	return nullptr;
};
#endif

#if defined(LPF_HAVE_SHL_LOAD)
lpf::shl_load_loader::shl_load_loader(void)
{
}

std::shared_ptr<lpf::loader> lpf::shl_load_loader::create(void)
{
	return std::make_shared<shl_load_loader>();
}

std::string lpf::shl_load_loader::name(void) const
{
	return "shl_load";
}

std::shared_ptr<lpf::module> lpf::shl_load_loader::open(const char* name) const
{
	return nullptr;
};
#endif