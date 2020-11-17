#pragma once

#include "engine/components/ui/fanUIText.hpp"
#include "engine/fanSceneTags.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiUIText
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Text16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUIText::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName       = "ui text";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            UIText& text = static_cast<UIText&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                if( ImGui::FanFontPtr( "font", text.mFontPtr ) )
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
                char                mTextBuffer[maxSize];
                memcpy( mTextBuffer, text.mText.c_str(), std::min( maxSize, text.mText.size() + 1 ) );
                if( ImGui::InputText( "text", mTextBuffer, 64 ) )
                {
                    text.mText = mTextBuffer;
                    _world.AddTag<TagUIModified>( _entity );
                }
            }
            ImGui::PopItemWidth();
        }
    };
}