#include "lpf.default_manager.h"
#include "lpf.loader.h"
#include "lpf.plugin.h"
#include "lpf.exceptions.h"

#include <functional>

lpf::wrapper::wrapper(std::shared_ptr<lpf::descriptor> desc,
	std::shared_ptr<loader_factory> factory)
	: plugin_(nullptr)
	, module_(nullptr)
	, state_(state::initialized)
	, descriptor_(desc)
	, loader_factory_(factory)
{
}

lpf::wrapper::~wrapper(void)
{
	plugin_ = nullptr;
	module_ = nullptr;
}

std::shared_ptr<lpf::wrapper> lpf::wrapper::of(
	std::shared_ptr<descriptor> desc,
	std::shared_ptr<loader_factory> factory)
{
	return std::make_shared<wrapper>(desc, factory);
}

std::shared_ptr<lpf::descriptor> lpf::wrapper::get_descriptor(void) const
{
	return descriptor_;
}

std::shared_ptr<lpf::unknown> lpf::wrapper::get_plugin(void) const
{
	if (state_ != state::loaded)
		throw plugin_exception("Invalid plugin state");
	return plugin_;
}

void lpf::wrapper::create(void)
{
	if (state_ != state::initialized)
		throw plugin_exception("Invalid plugin state");
	std::string path = descriptor_->get_path();
	module_ = get_loader()->open(path.c_str());
	if (module_ == nullptr)
		throw plugin_exception("Plugin library not found.");
	init_func pif = symbol<init_func>(module_, "plugin_init");
	if (pif == nullptr)
		throw plugin_exception("Plugin init function not found");
	unknown* punk = (*pif)(descriptor_->get_workdir().c_str());
	if (punk == nullptr)
		throw plugin_exception("Cannot construct plugin");
	std::shared_ptr<lpf::unknown> unk(punk, std::mem_fn(&lpf::unknown::dispose));
	plugin_ = query_cast<lpf::unknown>(unk);
	state_ = state::created;
}

void lpf::wrapper::load(void)
{
	if (state_ != state::created)
		throw plugin_exception("Invalid plugin state");
	std::shared_ptr<plugin> p = query_cast<plugin>(plugin_);
	if (p != nullptr)
		p->load();
	state_ = state::loaded;
}

void lpf::wrapper::unload(void)
{
	if (state_ != state::loaded)
		throw plugin_exception("Invalid plugin state");
	std::shared_ptr<plugin> p = query_cast<plugin>(plugin_);
	if (p != nullptr)
		p->unload();
	state_ = state::unloaded;
}

bool lpf::wrapper::is_active(void)
{
	return 	state_ == state::loaded;
}

std::shared_ptr<lpf::loader> lpf::wrapper::get_loader(void)
{
	static std::shared_ptr<loader> ldr = nullptr;
	if (ldr == nullptr)
		ldr = loader_factory_->instance();
	return ldr;
}

