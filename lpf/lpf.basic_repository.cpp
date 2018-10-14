#include "lpf.basic_repository.h"

#include <utility>

lpf::basic_repository::basic_repository(void)
	: entry_map_()
{
}

bool lpf::basic_repository::contains(const char* name) const
{
	return name == nullptr ? false : entry_map_.count(name) > 0;
}

std::shared_ptr<lpf::entry> lpf::basic_repository::get(const char* name) const
{
	if (contains(name))
		return entry_map_.find(name)->second;
	return nullptr;
}

std::vector<std::shared_ptr<lpf::entry>> lpf::basic_repository::all() const 
{
	std::vector<std::shared_ptr<lpf::entry>> res;
	for (auto it = entry_map_.cbegin(); it != entry_map_.cend(); ++it)
		res.push_back(it->second);
	return res;
}

void lpf::basic_repository::add_entry(const char* name, const char* path)
{
	entry_map_.insert(std::make_pair(name, std::make_shared<entry>(name, path)));
}