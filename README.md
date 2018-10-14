# lpf
 Lightweight C++ Plugin Framework

## Usage

### Create plugin

Create plugin class that implements the plugin interface

```c++
class plugin : public lpf::plugin
{
	void load(void);
	void unload(void);
	bool query(const char* iid, void** ppv);
	void dispose(void);
};

void plugin::load(void)
{
}

void plugin::unload(void)
{
}

bool plugin::query(const char* iid, void** ppv)
{
	void* pv = nullptr;
	if (std::strcmp(typeid(lpf::plugin).name(), iid) == 0)
		pv = static_cast<lpf_plugin*>(this);
	else if (std::strcmp(typeid(lpf::unknown).name(), iid) == 0)
		pv = static_cast<lpf_plugin*>(this);
	else
		return false;

	if (ppv != nullptr)
		*ppv = pv;
	return true;
}

void lpf_plugin::dispose(void)
{
	delete this;
}

extern "C" {
	LPF_EXPORT lpf::plugin* plugin_init()
	{
		return new plugin;
	}
}
```

### Create application

Create program using the plugin manager interface

```c++
int main()
{
	std::shared_ptr<lpf::loader_factory> fac =
		lpf::default_loader_factory::create();

	std::shared_ptr<lpf::repository> repo =
		lpf::directory_repository::of(sys::path::executable_path().c_str());

	std::shared_ptr<lpf::manager> mngr = lpf::default_manager::of(repo, fac);

	mngr->activate("p1", "plugin");

	mngr->shutdown();
}
```