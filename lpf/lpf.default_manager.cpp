#include "lpf.default_manager.h"
#include "lpf.repository.h"
#include "lpf.exceptions.h"

lpf::default_manager::default_manager(std::shared_ptr<lpf::repository> repo,
		std::shared_ptr<lpf::loader_factory> factory)
	: repository_(repo)
	, loader_factory_(factory)
	, active_plugins_()
	, activating_plugins_()
	, bad_plugins_()
	, disabled_plugins_()
{
}

std::shared_ptr<lpf::manager> lpf::default_manager::of(
	std::shared_ptr<lpf::repository> repo,
	std::shared_ptr<lpf::loader_factory> factory)
{
	return std::make_shared<default_manager>(repo, factory);
}

bool lpf::default_manager::is_activated(const char* id) const
{
	return active_plugins_.count(id) > 0;
}

bool lpf::default_manager::is_bad(const char* id) const
{
	return bad_plugins_.count(id) > 0;
}

bool lpf::default_manager::is_activating(const char* id) const
{
	return activating_plugins_.count(id) > 0;
}

bool lpf::default_manager::is_enabled(const char* id) const
{
	return disabled_plugins_.count(id) < 1;
}

std::shared_ptr<lpf::unknown>
lpf::default_manager::get(const char* id, const char* name)
{
	if (active_plugins_.count(id) > 0)
		return active_plugins_.find(id)->second->get_plugin();
	if (disabled_plugins_.count(id) > 0)
		throw plugin_exception("Plugin disabled");
	std::shared_ptr<entry> ent = repository_->get(name);
	if (ent == nullptr)
		throw plugin_exception("Unknown plugin");
	return activate(descriptor::of(id, ent->get_path().c_str()));
}

void lpf::default_manager::activate(const char* id, const char* name)
{
	if (disabled_plugins_.count(id) > 0)
		throw plugin_exception("Plugin disabled");
	std::shared_ptr<entry> ent = repository_->get(name);
	if (ent == nullptr)
		throw plugin_exception("Unknown plugin");
	activate(descriptor::of(id, ent->get_path().c_str()));
}

void lpf::default_manager::deactivate(const char* id)
{
	if (active_plugins_.count(id) > 1)
		return;
	std::shared_ptr<lpf::descriptor> desc =
		active_plugins_.find(id)->second->get_descriptor();
	deactivate(desc);
}

void lpf::default_manager::shutdown(void)
{
	std::vector<std::shared_ptr<descriptor>> descs = get_active();
	for (auto desc: descs)
		deactivate(desc);
	disabled_plugins_.clear();
}

std::shared_ptr<lpf::unknown>
lpf::default_manager::activate(std::shared_ptr<lpf::descriptor> desc)
{
	std::lock_guard<std::mutex> guard(mutex_);
	std::string id = desc->get_id();
	if (active_plugins_.count(id) > 0)
		return active_plugins_.find(id)->second->get_plugin();
	if (bad_plugins_.count(id) > 0)
		throw plugin_exception("Plugin disabled");
	if (activating_plugins_.count(id) > 0)
		throw plugin_exception("Plugin activating");
	activating_plugins_.insert(id);
	try
	{
		std::shared_ptr<wrapper> pw = wrapper::of(desc, loader_factory_);
		pw->create();
		pw->load();
		active_plugins_.insert(make_pair(id, pw));
		activating_plugins_.erase(id);
		return pw->get_plugin();
	}
	catch (const plugin_exception& pe)
	{
		activating_plugins_.erase(id);
		bad_plugins_.insert(id);
		throw pe;
	}
	catch (...)
	{
		activating_plugins_.erase(id);
		bad_plugins_.insert(id);
		throw plugin_exception("Plugin start failed");
	}
}

void lpf::default_manager::deactivate(std::shared_ptr<lpf::descriptor> desc)
{
	std::lock_guard<std::mutex> guard(mutex_);
	auto it = active_plugins_.find(desc->get_id());
	if (it != active_plugins_.end())
	{
		try
		{
			std::shared_ptr<wrapper> pw = it->second;
			if (pw->is_active())
				pw->unload();
			active_plugins_.erase(it);
		}
		catch (...)
		{
			active_plugins_.erase(it);
		}
	}
	bad_plugins_.erase(desc->get_id());
}

std::vector<std::shared_ptr<lpf::descriptor>>
lpf::default_manager::get_active(void)
{
	std::lock_guard<std::mutex> guard(mutex_);
	std::vector<std::shared_ptr<lpf::descriptor>> descs;
	for (auto kv : active_plugins_)
		descs.push_back(kv.second->get_descriptor());
	return descs;
}
