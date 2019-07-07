#pragma once

#include "scene/components/fanActor.h"

namespace scene
{
	class EditorCamera : public Actor
	{
	public:
		EditorCamera(Gameobject * _gameobject);

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "EditorCamera"; }

		void Start() override;
		void Update( const float _delta ) override;
		void Stop() override;

	private:
	};
}