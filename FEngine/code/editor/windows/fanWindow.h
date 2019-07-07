#pragma once

namespace editor {
	class Window {
	public:
		Window();
		virtual void Draw() = 0;
		void SetVisible(bool _value) { m_isVisible = _value;  }
		bool IsVisible() const { return  m_isVisible; }
	private:
		bool m_isVisible;
	};
}
