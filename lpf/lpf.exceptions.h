#ifndef __LPF_EXCEPTIONS__
#define __LPF_EXCEPTIONS__

#include <stdexcept>
#include <string>

namespace lpf
{
	class plugin_exception : public std::logic_error
	{
	public:
		explicit plugin_exception(const std::string& what)
			: std::logic_error(what) { }
	};
}

#endif

