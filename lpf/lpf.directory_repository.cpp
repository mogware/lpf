#include "lpf.config.h"
#include "lpf.directory_repository.h"
#include "sys.path.h"
#include "sys.dir.h"

#include <utility>
#include <algorithm>
#include <string>
#include <array>

lpf::directory_repository::directory_repository(const char* str)
{
	sys::path p(str);
	p.make_preferred();
	p.make_canonical();

	sys::dir d(p.c_str());
	for (std::string dname; d.advance(dname); )
	{
		std::array <std::string, 3> exts = {"dll","dylib","so"};

		std::string base, ext;
		split(dname, base, ext);

		if (std::any_of(exts.begin(), exts.end(),
			[&ext](const std::string &s) { return s == ext; }))
		{
			sys::path lib(p);
			lib.append(dname.c_str());
			add_entry(base.c_str(), lib.c_str());
		}
	}
}

std::shared_ptr<lpf::repository> lpf::directory_repository::of(const char* path)
{
	return std::make_shared<directory_repository>(path);
}

void lpf::directory_repository::split(const std::string& name,
	std::string& base, std::string& ext)
{
	std::string::size_type pos = name.rfind(".");
	if (pos != std::string::npos)
	{
		base = name.substr(0, pos);
		ext = name.substr(pos+1);
	}
	else
	{
		base = name;
		ext.clear();
	}
}