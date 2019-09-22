#pragma once

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class EditorWindow {
	public:
		EditorWindow(const std::string _name);
		virtual ~EditorWindow();

		void Draw();
		void SetVisible(bool _value) { m_isVisible = _value; }
		bool IsVisible() const { return  m_isVisible; }

	protected:
		virtual void OnGui() = 0;

	private:
		bool m_isVisible;
		std::string m_name;
		std::string m_jsonShowWindowKey;
	};
}

