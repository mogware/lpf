#ifndef __LPF_PLUGIN_MANAGER__
#define __LPF_PLUGIN_MANAGER__

#include "lpf.manager.h"
#include "lpf.unknown.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <set>

namespace lpf
{
	class wrapper;
	struct repository;
	struct loader_factory;
	class default_manager : public manager
	{
		std::shared_ptr<repository> repository_;
		std::shared_ptr<loader_factory> loader_factory_;

		std::mutex mutex_;

		std::unordered_map<std::string,
			std::shared_ptr<wrapper>> active_plugins_;
		std::set<std::string> activating_plugins_;
		std::set<std::string> bad_plugins_;
		std::set<std::string> disabled_plugins_;
	public:
		default_manager(std::shared_ptr<repository> repo,
			std::shared_ptr<loader_factory> factory);
	public:
		static std::shared_ptr<manager> of(std::shared_ptr<repository> repo,
			std::shared_ptr<loader_factory> factory);
	public:
		bool is_activated(const char* id) const;
		bool is_bad(const char* id) const;
		bool is_activating(const char* id) const;
		bool is_enabled(const char* id) const;
	public:
		std::shared_ptr<unknown> get(const char* id, const char* name);
		void activate(const char* id, const char* name);
		void deactivate(const char* id);
		void shutdown(void);
	private:
		std::shared_ptr<unknown> activate(std::shared_ptr<descriptor> desc);
		void deactivate(std::shared_ptr<descriptor> desc);
		std::vector<std::shared_ptr<descriptor>> get_active(void);
	};

	struct loader;
	struct module;
	class wrapper
	{
		std::shared_ptr<unknown> plugin_;
		std::shared_ptr<module> module_;
		enum state { initialized, created, loaded, unloaded } state_;
		std::shared_ptr<descriptor> descriptor_;
		std::shared_ptr<loader_factory> loader_factory_;
	public:
		wrapper(std::shared_ptr<descriptor> desc,
			std::shared_ptr<loader_factory> factory);
		~wrapper(void);
	public:
		static std::shared_ptr<wrapper> of(std::shared_ptr<descriptor> desc,
			std::shared_ptr<loader_factory> factory);
	public:
		std::shared_ptr<descriptor> get_descriptor(void) const;
		std::shared_ptr<unknown> get_plugin(void) const;
	public:
		void create(void);
		void load(void);
		void unload(void);
	public:
		bool is_active(void);
	private:
		std::shared_ptr<loader> get_loader(void);
	};

	typedef lpf::unknown* (*init_func)(const char* workdir);
}

#endif

