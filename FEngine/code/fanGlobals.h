#pragma once

#include "core/fanSingleton.h"

namespace fan
{
	class Engine;

	//================================================================================================================================
	//================================================================================================================================	
	class Globals : public Singleton<Globals>
	{
		friend class Singleton < Globals >;

	public:
		Engine * engine = nullptr;

	};
}