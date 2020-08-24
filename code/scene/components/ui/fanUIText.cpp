#include "ecs/fanEcsWorld.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "scene/components/ui/fanUIText.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/singletons/fanRenderResources.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void UIText::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::Text16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &UIText::OnGui;
		_info.load = &UIText::Load;
		_info.save = &UIText::Save;
		_info.editorPath = "ui/";
		_info.name = "ui text";
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIText::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UIText& uiText = static_cast<UIText&>( _component );
        uiText.mText  = "";
        uiText.mModified = true; 
        uiText.mMesh2D = new Mesh2D();
        _world.GetSingleton<RenderResources>().mMesh2DManager->Add( uiText.mMesh2D, "text_mesh" );
	}

    //================================================================================================================================
    //================================================================================================================================
    void UIText::Save( const EcsComponent& _component, Json& _json )
    {
        const UIText& uiText = static_cast<const UIText&>( _component );
        Serializable::SaveString( _json, "text", uiText.mText );
    }

    //================================================================================================================================
    //================================================================================================================================
    void UIText::Load( EcsComponent& _component, const Json& _json )
    {
        UIText& uiText = static_cast<UIText&>( _component );
        Serializable::LoadString( _json, "text", uiText.mText );
    }

    //================================================================================================================================
	//================================================================================================================================
	void UIText::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
        UIText& uiText = static_cast<UIText&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// color
			if( ImGui::Button( "##clear_text" ) )
			{
                uiText.mText.clear();
                uiText.mModified = true;
			}
			ImGui::SameLine();
			static const size_t maxSize = 64;
			char mTextBuffer[maxSize];
			memcpy( mTextBuffer, uiText.mText.c_str(), std::min( maxSize, uiText.mText.size() + 1 ) );
			if( ImGui::InputText("text", mTextBuffer, 64 ) )
            {
                uiText.mText = mTextBuffer;
                uiText.mModified = true;
            }
            ImGui::Checkbox("modified", &uiText.mModified );
		} ImGui::PopItemWidth();
	}
}