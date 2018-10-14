#ifndef __LPF_MANAGER__
#define __LPF_MANAGER__

#include <string>
#include <vector>
#include <memory>

#include "lpf.unknown.h"

namespace lpf
{
	struct unknown;
	class descriptor;
	struct manager
	{
		virtual bool is_activated(const char* id) const = 0;
		virtual bool is_bad(const char* id) const = 0;
		virtual bool is_activating(const char* id) const = 0;
		virtual bool is_enabled(const char* id) const = 0;

		virtual std::shared_ptr<unknown>
			get(const char* id, const char* name = nullptr) = 0;
		virtual void activate(const char* id, const char* name) = 0;
		virtual void deactivate(const char* id) = 0;
		virtual void shutdown(void) = 0;
		virtual ~manager(void) {}
	};

	class descriptor
	{
		std::string id_;
		std::string path_;
	public:
		descriptor(const char* id, const char* path);
	public:
		static std::shared_ptr<descriptor> of(const char* id, const char* path);
	public:
		std::string get_id(void) const;
		std::string get_path(void) const;
		std::string get_workdir(void) const;
	};

	template<class T>
	std::shared_ptr<T> get(const std::shared_ptr<manager>& mngr,
		const char* id, const char* name = nullptr) noexcept
	{
		std::shared_ptr<unknown> p = mngr->get(id, name);
		if (p == nullptr)
			return nullptr;
		return query_cast<T>(p);
	}
}

#endif

