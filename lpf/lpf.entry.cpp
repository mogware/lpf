#include "lpf.repository.h"

lpf::entry::entry(const char* name, const char* path)
	: name_(name)
	, path_(path)
{
}

std::string lpf::entry::get_name(void) const
{
	return name_;
}

std::string lpf::entry::get_path(void) const
{
	return path_;
}
