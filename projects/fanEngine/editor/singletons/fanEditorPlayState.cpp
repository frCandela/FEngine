#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayState::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayState::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        EditorPlayState& editorPlayState = static_cast<EditorPlayState&>( _component );
        editorPlayState.mState = State::STOPPED;
    }
}