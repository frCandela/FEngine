#pragma once

#include "scene/components/fanComponent.h"
#include "scene/fanRessourcePtr.h"

namespace fan
{
	class UIMesh;
	class Gameobject;
	class Texture;

	//================================================================================================================================
	//================================================================================================================================
	class UIMeshRenderer : public Component
	{
	public:

		UIMesh *		GetMesh()		{ return m_uiMesh; }
		const UIMesh *	GetMesh() const { return m_uiMesh; }

		glm::ivec2	GetPixelPosition() const;
		void		SetPixelPosition( const glm::ivec2 _position );
		glm::ivec2	GetPixelSize() const;
		void		SetPixelSize( const glm::ivec2 _size );
		glm::ivec2	GetTextureSize() const;
		bool		IsVisible() const { return m_visible; }
		void		SetVisible( const bool _visible ) { m_visible = _visible; }

		Color GetColor() const				{ return m_color; }
		void SetColor( const Color& _color) { m_color = _color; }
		Texture * GetTexture() const { return *m_texture; }

		void OnGui() override;
		virtual ImGui::IconType GetIcon() const { return ImGui::IconType::IMAGE; };

		DECLARE_TYPE_INFO( UIMeshRenderer, Component );

	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnAttach() override;
		void OnDetach() override;

	private:
		UIMesh * m_uiMesh;
		Color m_color;
		TexturePtr m_texture;
		bool m_visible = true;
	};
}