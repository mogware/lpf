#ifndef __LPF_BASE_REPOSITORY__
#define __LPF_BASE_REPOSITORY__

#include "lpf.repository.h"

#include <map>

namespace lpf
{
	class basic_repository : public repository
	{
		std::map<std::string, std::shared_ptr<entry>> entry_map_;
	public:
		basic_repository(void);
	public:
		bool contains(const char* name) const;
		std::shared_ptr<entry> get(const char* name) const;
		std::vector<std::shared_ptr<entry>> all() const;
	public:
		void add_entry(const char* name, const char* path);
	};
}

#endif



