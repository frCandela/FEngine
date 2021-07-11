#include "editor/windows/fanSingletonsWindow.hpp"

#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanProfiler.hpp"
#include "editor/fanDragnDrop.hpp"
#include "editor/fanGroupsColors.hpp"
#include "editor/singletons/fanEditorSettings.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SingletonsWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SingletonsWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        SingletonsWindow& singletonsWindow = static_cast<SingletonsWindow&>( _singleton );
        (void)singletonsWindow;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiSingletonsWindow::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        SingletonsWindow& singletonsWindow = static_cast<SingletonsWindow&>( _singleton );
        (void)singletonsWindow;

        SCOPED_PROFILE( singleton_win );
        const EditorSettings               & settings = _world.GetSingleton<EditorSettings>();
        const std::vector<EcsSingletonInfo>& infos    = _world.GetVectorSingletonInfo();
        for( const EcsSingletonInfo        & info : infos )
        {
            const fan::GuiSingletonInfo* guiInfo = settings.GetSingletonInfo( info.mType );
            if( guiInfo != nullptr && guiInfo->onGui != nullptr && guiInfo->mType == GuiSingletonInfo::Type::Default )
            {
                ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3 ); // moves cursor lower to center the icon
                ImGui::Icon( guiInfo->mIcon, { 16, 16 }, settings.mData->mGroupsColors.GetColor( guiInfo->mGroup ) );
                ImGui::SameLine();
                ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3 );        // resets the cursor
                if( ImGui::CollapsingHeader( info.mName.c_str() ) )
                {
                    ImGui::FanBeginDragDropSourceSingleton( _world, info.mType );
                    // draws gui
                    if( guiInfo->onGui != nullptr )
                    {
                        ImGui::Indent();
                        ImGui::Indent();
                        guiInfo->onGui( _world, _world.GetSingleton( info.mType ) );
                        ImGui::Unindent();
                        ImGui::Unindent();
                    }
                }
                else
                {
                    ImGui::FanBeginDragDropSourceSingleton( _world, info.mType );
                }
            }
        }
    }
}