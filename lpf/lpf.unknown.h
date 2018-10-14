#ifndef __LPF_UNKNOWN__
#define __LPF_UNKNOWN__

#include "lpf.config.h"

#include <memory>

#define IID_PPV_ARG(T,E) typeid(T).name(), \
		reinterpret_cast<void**>(static_cast<T**>(E))

namespace lpf
{
	struct LPF_EXPORT unknown
	{
		virtual bool query(const char* iid, void** ppv) = 0;
		virtual void dispose(void) = 0;
		virtual ~unknown() {}
	};

	template<class T>
	std::shared_ptr<T> query_cast(const std::shared_ptr<unknown>& r) noexcept
	{
		typename std::shared_ptr<T>::element_type* p = nullptr;
		if (r->query(IID_PPV_ARG(typename std::shared_ptr<T>::element_type, &p)))
			return std::shared_ptr<T>(r, p);
		return nullptr;
	}
}

#endif


