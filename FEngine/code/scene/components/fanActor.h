#pragma once

#include "scene/components/fanComponent.h"
#include "core/fanSignal.h"

namespace fan
{
	namespace scene
	{
		//================================================================================================================================
		//================================================================================================================================
		class Actor : public Component
		{
		public:
			static fan::Signal< Actor * > onActorAttach;
			static fan::Signal< Actor * > onActorDetach;

			bool IsUnique() const override	{ return false; }

			virtual void Start() = 0;
			virtual void Update(const float _delta) = 0;

		protected:
			void OnAttach() override;
			void OnDetach() override;

		};
	}
}