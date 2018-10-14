#ifndef __LPF_LOADER__
#define __LPF_LOADER__

#include <string>
#include <memory>

namespace lpf
{
	struct module
	{
		virtual bool close(void) const = 0;
		virtual void* symbol(const char* name) const = 0;
		virtual ~module(void) {};
	};

	struct loader
	{
		virtual std::string name(void) const = 0;
		virtual std::shared_ptr<module> open(const char* name) const = 0;
		virtual ~loader(void) {};
	};

	struct loader_factory
	{
		virtual std::shared_ptr<loader> instance(void) const = 0;
		virtual ~loader_factory(void) {}
	};

	template<class T>
	T symbol(std::shared_ptr<module> mod, const char* name)
	{
		return static_cast<T>(mod->symbol(name));
	}
}

#endif


