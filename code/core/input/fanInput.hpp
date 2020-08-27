#pragma once

#include "core/fanSingleton.hpp"
#include "core/math/fanVector2.hpp"

struct GLFWwindow;

namespace fan
{
	class InputManager;

	//========================================================================================================
	// global input manager
	// @todo make an ecs singleton component to store inputs and simplify this.
	// accessing global input should only be done in the editor
	// @todo the input singleton must DIE
	//========================================================================================================
	class Input : public Singleton<Input>
	{
		friend class Singleton < Input>;
	public:
		void			Setup( GLFWwindow* _window );
		void			NewFrame();
		GLFWwindow*		Window()		{ return mWindow; }
		uint64_t		FrameCount()	{ return mCount; }
        InputManager&	Manager()		{ return *mInputManager; }

	private:
		Input();

		GLFWwindow*     mWindow;
		uint64_t        mCount;
		InputManager*   mInputManager;
	};
}
