#pragma once

#include "editor/fanImguiIcons.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class EditorWindow {
	public:
		EditorWindow(const std::string _name, const ImGui::IconType _iconType );
		virtual ~EditorWindow();

		void Draw();

		void SetVisible(bool _value) { m_isVisible = _value; }
		bool IsVisible() const { return  m_isVisible; }

		std::string				GetName() const		{ return m_name;		}
		ImGui::IconType	GetIconType() const { return m_iconType;	}

	protected:
		virtual void OnGui() = 0;

	private:
		bool m_isVisible;
		std::string m_name;
		std::string m_jsonShowWindowKey;
		ImGui::IconType m_iconType;
	};
}

