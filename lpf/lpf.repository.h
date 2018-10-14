#ifndef __LPF_REPOSITORY__
#define __LPF_REPOSITORY__

#include "lpf.manager.h"

#include <vector>
#include <string>
#include <memory>

namespace lpf
{
	class entry;
	struct repository
	{
		virtual bool contains(const char* name) const = 0;
		virtual std::shared_ptr<entry> get(const char* name) const = 0;
		virtual std::vector<std::shared_ptr<entry>> all() const = 0;
		virtual ~repository(void) {}
	};

	class entry
	{
		std::string name_;
		std::string path_;
	public:
		entry(const char* name, const char* path);
	public:
		std::string get_name(void) const;
		std::string get_path(void) const;
	};
}

#endif


