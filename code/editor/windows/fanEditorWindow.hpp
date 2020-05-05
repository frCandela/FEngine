#pragma once

#include <string>
#include "editor/fanImguiIcons.hpp"

namespace fan
{
	//================================================================================================================================
	// base class for editor windows
	// automagic hide/show & show buttons in the tools main menu bar
	//================================================================================================================================
	class EditorWindow
	{
	public:
		EditorWindow( const std::string _name, const ImGui::IconType _iconType );
		virtual ~EditorWindow();

		void Draw();

		void SetVisible( bool _value ) { m_isVisible = _value; }
		bool IsVisible() const { return  m_isVisible; }
		void AddFlag( const ImGuiWindowFlags _flags ) { m_flags |= _flags; }

		std::string		GetName() const { return m_name; }
		ImGui::IconType	GetIconType() const { return m_iconType; }

	protected:
		virtual void OnGui() = 0;

	private:
		bool m_isVisible;

		ImGuiWindowFlags m_flags = ImGuiWindowFlags_None;

		std::string m_name;
		std::string m_jsonShowWindowKey;
		ImGui::IconType m_iconType;
	};
}

