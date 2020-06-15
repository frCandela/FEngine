#pragma once

#include "core/fanSingleton.hpp"
#include "core/math/fanVector2.hpp"

struct GLFWwindow;

namespace fan
{
	class InputManager;

	//================================================================================================================================
	// global input manager
	// @todo make an ecs singleton component to store inputs and simplify this.
	// accessing global input should only be done in the editor
	// @todo the input singleton must DIE
	//================================================================================================================================
	class Input : public Singleton<Input>
	{
		friend class Singleton < Input>;

	public:
		void			Setup( GLFWwindow* _window );
		void			NewFrame();
		GLFWwindow*		Window()		{ return m_window; }
		btVector2		WindowSizeF()	{ return btVector2( ( float ) m_windowSize.x, ( float ) m_windowSize.y ); }
		glm::ivec2		WindowSize()	{ return m_windowSize; }
		uint64_t		FrameCount()	{ return m_count; }
		InputManager&	Manager()		{ return *m_eventManager; }

	private:
		Input();

		GLFWwindow* m_window;
		glm::ivec2	m_windowSize;
		uint64_t	m_count;

		InputManager* m_eventManager;

		static void	WindowSizeCallback( GLFWwindow* window, int width, int height );
	};
}
