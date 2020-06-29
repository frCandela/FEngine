#pragma once

#include "core/math/fanVector2.hpp"
#include "ecs/fanSignal.hpp"
#include "editor/windows/fanEditorWindow.hpp"
#include "editor/fanLaunchSettings.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// show the game in 3D
	//================================================================================================================================
	class GameViewWindow : public EditorWindow
	{
	public:
		Signal< btVector2 > onSizeChanged;
		Signal<>	onPlay;
		Signal<>	onPause;
		Signal<>	onResume;
		Signal<>	onStop;
		Signal<>	onStep;
		Signal<int> onSelectGame;

		GameViewWindow( const LaunchSettings::Mode _launchMode );
		btVector2 GetSize()			const { return m_size; }
		btVector2 GetPosition()		const { return m_position; }
		float	  GetAspectRatio()	const { return ( float ) m_size[ 0 ] / ( float ) m_size[ 1 ]; }
		bool	  IsHovered()		const { return m_isHovered; }
		void	  SetCurrentGameSelected( const int _index ) { m_currentGameSelected = _index;  }
	protected:
		void OnGui( EcsWorld& _world ) override;

	private:
		btVector2	m_size = btVector2( 1.f, 1.f );
		btVector2	m_position;
		bool		m_isHovered;
		char		m_gameWorldsStr[16];
		int			m_currentGameSelected = 0;
	};
}