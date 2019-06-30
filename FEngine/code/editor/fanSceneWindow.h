#pragma once

namespace editor {
	class SceneWindow {
	public:
		SceneWindow();

		void Draw();
		void SetVisible(bool _value) { m_isVisible = _value;  }
		bool IsVisible() const { return  m_isVisible; }

		void NewGameobjectModal();
	private:
		bool m_isVisible;
		std::array<char, 64> m_textBuffer;
	};
}
