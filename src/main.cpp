
#include "f4se_plugin.h"

extern "C" {

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
	{
		return f4se::Plugin::GetSingleton().Query(f4se, info);
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		return f4se::Plugin::GetSingleton().Load(f4se);
	}
#else
	bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		return f4se::Plugin::GetSingleton().Load(f4se);
	}

	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
		1,
		"HHS",
		"PK0",
		0,
		0,
		{ 
#if CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_10_980
			RUNTIME_VERSION_1_10_980
#else 
			RUNTIME_VERSION_1_10_984
#endif
			, 0 },
		0,
	};
#endif
}
