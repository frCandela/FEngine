#pragma once

#include "scene/components/fanComponent.h"

namespace scene
{
	class Actor : public Component
	{
	public:
		Actor();
		Actor(Gameobject * _gameobject);

		bool IsActor()	const final		{ return true; }
		bool IsUnique() const override	{ return false; }

		virtual void Start()						= 0;
		virtual void Update( const float _delta )	= 0;

	protected:
	};
}