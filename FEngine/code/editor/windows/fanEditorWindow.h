#pragma once

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class Window {
		public:
			Window( const std::string _name );
			virtual ~Window();

			virtual void Draw() = 0;
			void SetVisible(bool _value) { m_isVisible = _value; }
			bool IsVisible() const { return  m_isVisible; }
		private:
			bool m_isVisible;
			std::string m_jsonShowWindowKey;
		};
	}
}
