
#include "f4se_plugin.h"

extern "C" {

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
	{
		return f4se::Plugin::GetInstance().Query(f4se, info);
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		return f4se::Plugin::GetInstance().Load(f4se);
	}
#else
	bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		return f4se::Plugin::GetInstance().Load(f4se);
	}

	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
		1,
		"HHS",
		"PK0",
		0,	// not version independent
		0,	// not version independent (extended field)
		{ RUNTIME_VERSION_1_10_984, 0 },	// compatible with 1.10.980
		0,	// works with any version of the script extender. you probably do not need to put anything here
	};
#endif
}
