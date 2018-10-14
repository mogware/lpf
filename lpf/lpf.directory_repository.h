#ifndef __LPF_DIRECTORY_REPOSITORY__
#define __LPF_DIRECTORY_REPOSITORY__

#include "lpf.basic_repository.h"

#include <string>
#include <memory>

namespace lpf
{
	class directory_repository : public basic_repository
	{
	public:
		directory_repository(const char* path);
		static std::shared_ptr<repository> of(const char* path);
	private:
		void split(const std::string& name,
				std::string& base, std::string& ext);
	};
}

#endif




