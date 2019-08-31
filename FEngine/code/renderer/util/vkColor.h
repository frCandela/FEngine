#pragma once

namespace vk {
	class Color {
	public:
		Color();
		Color(const float _r, const float _g, const float _b, const float _a);

		glm::vec4 ToGLM() const { return m_colorData; }
		float& operator[]	( const int _index) { return m_colorData[_index]; }
		float operator[]	( const int _index) const { return m_colorData[_index]; }

		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color White;
		static const Color Black;
		static const Color Grey;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Cyan;
		static const Color Brown;
		static const Color Purple;
		static const Color Pink;
		static const Color Orange;
	private:
		glm::vec4 m_colorData;
	};


}