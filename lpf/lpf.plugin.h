#ifndef __LPF_PLUGIN__
#define __LPF_PLUGIN__

#include "lpf.unknown.h"

namespace lpf
{
	struct LPF_EXPORT plugin : public unknown
	{
		virtual void load(void) = 0;
		virtual void unload(void) = 0;
	};
}

#endif



