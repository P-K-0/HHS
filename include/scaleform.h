#pragma once

#include "version.h"

namespace Scaleform {

	class TestHeightInGame_HHS :
		public GFxFunctionHandler {

	public:

		virtual void Invoke(Args* args);
	};

	class CreateTxtInGame_HHS :
		public GFxFunctionHandler {

	public:

		virtual void Invoke(Args* args);
	};

	class DeleteTxtInGame_HHS :
		public GFxFunctionHandler {

	public:

		virtual void Invoke(Args* args);
	};

	class SetSetting_HHS :
		public GFxFunctionHandler {

	public:

		virtual void Invoke(Args* args);
	};

	extern [[nodiscard]] bool Register(GFxMovieView* view, GFxValue* value) noexcept;
}
