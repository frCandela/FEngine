#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class GameWindow : public EditorWindow {
	public:
		Signal< btVector2 > onSizeChanged;

		GameWindow();
		btVector2 GetSize()			const { return m_size; }
		btVector2 GetPosition()		const { return m_position; }
		float	  GetAspectRatio()	const { return (float)m_size[0] / (float)m_size[1]; }
		bool IsHovered()			const { return m_isHovered; }

	protected:
		void OnGui() override;

	private:
		btVector2	m_size = btVector2(1.f,1.f);
		btVector2	m_position;
		bool		m_isHovered;
	};

}