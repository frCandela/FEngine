#pragma once

#include "scene/components/fanComponent.h"

namespace scene
{
	class Actor : public Component
	{
	public:
		Actor(Gameobject * _gameobject);

		bool IsUnique() const override { return false; }
		std::string GetName() const override { return "Actor"; }

		virtual void Start()=0;
		virtual void Update( const float _delta ) = 0;
		virtual void Stop() = 0;

		virtual void Delete();
	protected:
	};
}