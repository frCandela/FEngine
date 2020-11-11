#include "editor/windows/fanEditorWindow.hpp"

#include <sstream>
#include "core/memory/fanSerializedValues.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	EditorWindow::EditorWindow( const std::string _name, const ImGui::IconType _iconType ) :
	     mName( _name ),
		 mIsVisible( true ),
		 mIconType( _iconType )
	{

		std::stringstream ss;
		ss << "show_window_" << _name;
        mJsonShowWindowKey = ss.str();

		bool value = false;
		if ( SerializedValues::Get().GetBool( mJsonShowWindowKey.c_str(), value ) )
		{
			SetVisible( value );
		}
	}

	//========================================================================================================
	//========================================================================================================
	EditorWindow::~EditorWindow()
	{
		SerializedValues::Get().SetBool( mJsonShowWindowKey.c_str(), mIsVisible );
	}

	//========================================================================================================
	//========================================================================================================
	void EditorWindow::Draw( EcsWorld& _world )
	{
		if ( mIsVisible == true )
		{
			if ( ImGui::Begin( mName.c_str(), &mIsVisible, mFlags ) )
			{
				OnGui( _world );
			}
			ImGui::End();
			SetVisible( mIsVisible );
		}
	}
}
