#include "lpf.config.h"
#include "lpf.default_loader.h"

const int lpf::default_loader_factory::use_best = 0;
const int lpf::default_loader_factory::use_loadlibrary = 1;
const int lpf::default_loader_factory::use_dlopen = 2;
const int lpf::default_loader_factory::use_dld_link = 3;
const int lpf::default_loader_factory::use_dyld = 4;
const int lpf::default_loader_factory::use_shl_load = 5;

lpf::default_loader_factory::default_loader_factory(const int& hint)
	: hint_(hint)
{
}

std::shared_ptr<lpf::loader_factory>
lpf::default_loader_factory::create(const int& hint)
{
	return std::make_shared<default_loader_factory>(hint);
}

std::shared_ptr<lpf::loader>
lpf::default_loader_factory::instance(void) const
{
	static const int use_best_loader =
#if defined(LPF_WIN32)
		use_loadlibrary;
#elif defined(LPF_HAVE_DLOPEN)
		use_dlopen;
#elif defined(LPF_HAVE_DLD_LINK)
		use_dld_link;
#elif defined(LPF_HAVE_DYLD)
		use_dyld;
#elif defined(LPF_HAVE_SHL_LOAD)
		use_shl_load;
#else
		-1;	// no loader	
#endif

	int which = (hint_ == use_best) ? use_best_loader : hint_;
	for (;;)
	{
		switch (which)
		{
#if defined(LPF_WIN32)
		case use_loadlibrary:
			return win32_loadlibrary::create();
#endif
#if defined(LPF_HAVE_DLOPEN)
		case use_dlopen:
			return dlopen_loader::create();
#endif
#if defined(LPF_HAVE_DLD_LINK)
		case use_dlopen:
			return dld_link_loader::create();
#endif
#if defined(LPF_HAVE_DYLD)
		case use_dyld:
			return dyld_loader::create();
#endif
#if defined(LPF_HAVE_SHL_LOAD)
		case use_shl_load:
			return shl_load_loader::create();
#endif
		default:
			if (which != use_best_loader)
				which = use_best_loader;
			else
				throw std::runtime_error("Internal error - no loader found");
		}
	}
}
