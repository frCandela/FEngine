#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	class Renderer;

	//================================================================================================================================
	//================================================================================================================================
	class PreferencesWindow : public EditorWindow {
	public:
		PreferencesWindow( Renderer * _renderer );

	protected:
		void OnGui() override;

	private:
		Renderer * m_renderer;

		void CaptureKeyPopup();
		void DeleteKeyPopup();

		void SetKeyButton( int & _button );
		void CaptureKeyCallback( int _key );
		void StartCaptureKey( int & _button );
		void StopCaptureKey();


		bool	m_openCaptureKeyPopup = false;
		bool	m_openDeletePopup = false;
		int		m_uniqueKeyIndex = 0;					// unique ids for key buttons
		int *	m_buttonCaptureDestination = nullptr;	// pointer on the currently modified keyboard key
		int *	m_buttonDeleteDestination = nullptr;	// pointer on the currently deleted keyboard key
	};
}
