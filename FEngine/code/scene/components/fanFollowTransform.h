#pragma once

#include "scene/actors/fanActor.h"
#include "scene/fanComponentPtr.h"

namespace fan
{
	class Transform;

	//================================================================================================================================
	// Makes a transform follow another transform with an offset
	// Specialized of ui
	//================================================================================================================================
	class FollowTransform : public Actor
	{
	public:

		void Start() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::FOLLOW_TRANSFORM; }

		DECLARE_TYPE_INFO( FollowTransform, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		ComponentPtr<Transform> m_followedTransform;
		btVector3 m_offset = btVector3::Zero();

		void UpdatePosition();
	};
}