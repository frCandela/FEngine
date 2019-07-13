#include "fanIncludes.h"

#include "scene/components/fanActor.h"
#include "scene/fanGameobject.h"
#include "util/fanSignal.h"
#include "fanEngine.h"

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	Actor::Actor(Gameobject * _gameobject) :
		Component(_gameobject) {
		fan::Engine::GetEngine().AddActor(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Actor::Delete() {
		fan::Engine::GetEngine().RemoveActor(this);
	};
}