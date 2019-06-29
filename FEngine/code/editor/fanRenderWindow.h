#pragma once

namespace editor {
	class RenderWindow {
	public:
		RenderWindow();

		void Draw();
		void SetVisible(bool _value) { m_isVisible = _value;  }
		bool IsVisible() const { return  m_isVisible; }
	private:
		bool m_isVisible;
	};
}
