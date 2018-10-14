#include "lpf.manager.h"
#include "sys.path.h"

lpf::descriptor::descriptor(const char* id, const char* path)
	: id_(id)
	, path_(path)
{
}

std::shared_ptr<lpf::descriptor> lpf::descriptor::of(const char* id, const char* path)
{
	return std::make_shared<descriptor>(id, path);
}

std::string lpf::descriptor::get_id(void) const
{
	return id_;
}

std::string lpf::descriptor::get_path(void) const
{
	return path_;
}

std::string lpf::descriptor::get_workdir(void) const
{
	sys::path p(path_);
	return p.remove_filename().c_str();
}