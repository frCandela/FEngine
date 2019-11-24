#pragma once

#include "scene/components/fanComponent.h"
#include "scene/fanRessourcePtr.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class UITransform : public Component
	{
	public:

		glm::ivec2	GetPixelPosition() const;
		void		SetPixelPosition( const glm::ivec2 _position );
		glm::ivec2	GetPixelSize() const;
		void		SetPixelSize( const glm::ivec2 _size );

		void OnGui() override;
		virtual ImGui::IconType GetIcon() const { return ImGui::IconType::TRANSFORM; };

		DECLARE_TYPE_INFO( UITransform, Component );

	protected:
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnAttach() override;
		void OnDetach() override;
	};
}