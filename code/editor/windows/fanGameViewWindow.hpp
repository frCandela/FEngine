#pragma once

#include "core/math/fanVector2.hpp"
#include "ecs/fanSignal.hpp"
#include "editor/windows/fanEditorWindow.hpp"
#include "game/fanLaunchSettings.hpp"
#include "glfw/glfw3.h"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	// show the game in 3D
	//========================================================================================================
	class GameViewWindow : public EditorWindow
	{
	public:
		Signal< VkExtent2D > mOnSizeChanged;
		Signal<>             mOnPlay;
		Signal<>             mOnPause;
		Signal<>             mOnResume;
		Signal<>             mOnStop;
		Signal<>             mOnStep;
		Signal<int>          mOnSelectGame;

		GameViewWindow( const LaunchSettings::Mode _launchMode );
		btVector2 GetSize()			const { return mSize; }
		btVector2 GetPosition()		const { return mPosition; }
		float	  GetAspectRatio()	const { return ( float ) mSize[ 0 ] / ( float ) mSize[ 1 ]; }
		bool	  IsHovered()		const { return mIsHovered; }
		void	  SetCurrentGameSelected( const int _index ) { mCurrentGameSelected = _index;  }
	protected:
		void OnGui( EcsWorld& _world ) override;

	private:
		btVector2 mSize                = btVector2( 1.f, 1.f );
		btVector2 mPosition;
		bool      mIsHovered;
		char      mGameWorldsStr[16];
		int       mCurrentGameSelected = 0;
	};
}