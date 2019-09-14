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

			virtual void Start() = 0;
			virtual void Update(const float _delta) = 0;

			bool IsUnique() const override { return false; }
			void OnGui() override;

		protected:
			void OnAttach() override;
			void OnDetach() override;

		};
	}
}