#include "core/ecs/fanEcsWorld.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "scene/components/ui/fanUIText.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "scene/fanSceneTags.hpp"
#include "render/resources/fanFont.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIText::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Text16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIText::OnGui;
		_info.load        = &UIText::Load;
		_info.save        = &UIText::Save;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui text";
	}

	//========================================================================================================
	//========================================================================================================
	void UIText::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
        UIText& text = static_cast<UIText&>( _component );
        text.mText   = "";
        text.mMesh2D = new Mesh2D();
        text.mSize   = 18;
        text.mFontPtr = nullptr;

        _world.GetSingleton<RenderResources>().mMesh2DManager->Add( text.mMesh2D, "text_mesh" );
        _world.AddTag<TagUIModified>( _entity );
	}

    //========================================================================================================
    //========================================================================================================
    void UIText::Save( const EcsComponent& _component, Json& _json )
    {
        const UIText& text = static_cast<const UIText&>( _component );
        Serializable::SaveString( _json, "text", text.mText );
        Serializable::SaveInt( _json, "size", text.mSize );
        Serializable::SaveFontPtr( _json, "font", text.mFontPtr );
    }

    //========================================================================================================
    //========================================================================================================
    void UIText::Load( EcsComponent& _component, const Json& _json )
    {
        UIText& text = static_cast<UIText&>( _component );
        Serializable::LoadString( _json, "text", text.mText );
        Serializable::LoadInt( _json, "size", text.mSize );
        Serializable::LoadFontPtr( _json, "font", text.mFontPtr );
    }

    //========================================================================================================
	//========================================================================================================
	void UIText::OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
        UIText& text = static_cast<UIText&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
            if( ImGui::FanFontPtr("font", text.mFontPtr ) )
            {
                _world.AddTag<TagUIModified>( _entity );
            }

            if( ImGui::Button( "##clear_font_size" ) )
            {
                text.mSize = 18;
                _world.AddTag<TagUIModified>( _entity );
            }
            ImGui::SameLine();
            ImGui::DragInt( "size", &text.mSize, 1, 8, 300 );
            if( ImGui::IsItemDeactivatedAfterEdit() )
            {
                _world.AddTag<TagUIModified>( _entity );
            }

			// color
			if( ImGui::Button( "##clear_text" ) )
			{
                text.mText.clear();
                _world.AddTag<TagUIModified>( _entity );
			}
			ImGui::SameLine();
			static const size_t maxSize = 64;
			char mTextBuffer[maxSize];
			memcpy( mTextBuffer, text.mText.c_str(), std::min( maxSize, text.mText.size() + 1 ) );
			if( ImGui::InputText("text", mTextBuffer, 64 ) )
            {
                text.mText = mTextBuffer;
                _world.AddTag<TagUIModified>( _entity );
            }
		} ImGui::PopItemWidth();
	}
}