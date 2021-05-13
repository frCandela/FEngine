#include "editor/windows/fanEcsWindow.hpp"

#include <sstream>
#include "core/time/fanProfiler.hpp"
#include "core/fanDebug.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "core/ecs/fanEcsSystem.hpp"
#include "core/ecs/fanEcsTag.hpp"
#include "engine/fanSceneTags.hpp"
#include "editor/fanModals.hpp"
#include "editor/gui/fanGroupsColors.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/singletons/fanEditorSettings.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        EcsWindow& ecsWindow = static_cast<EcsWindow&>( _singleton );
        (void)ecsWindow;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiEcsWindow::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        EcsWindow& ecsWindow = static_cast<EcsWindow&>( _singleton );
        (void)ecsWindow;

        // Global
        if( ImGui::CollapsingHeader( "Global" ) )
        {
            ImGui::Text( "num chunks     : %d", (int)EcsChunk::sAllocator.Size() );
            ImGui::Text( "total size (Mo): %.1f",
                         float( EcsChunk::sAllocator.Size() * EcsChunk::sAllocator.sChunkSize ) * 0.000001f );
        }

        // Archetypes
        if( ImGui::CollapsingHeader( "Archetypes" ) )
        {
            ImGui::Columns( 3 );
            ImGui::Text( "signature" );
            ImGui::NextColumn();
            ImGui::Text( "size" );
            ImGui::NextColumn();
            ImGui::Text( "chunks" );
            ImGui::NextColumn();
            ImGui::Separator();

            std::vector<const EcsArchetype*> archetypes;
            const std::unordered_map<EcsSignature, EcsArchetype*>& archetypesRef = _world.GetArchetypes();
            for( auto it = archetypesRef.begin(); it != archetypesRef.end(); ++it )
            {
                archetypes.push_back( it->second );
            }
            archetypes.push_back( &_world.GetTransitionArchetype() );

            for( const EcsArchetype* archetype : archetypes )
            {
                std::stringstream ssSignature;
                ssSignature << archetype->GetSignature();
                ImGui::Text( ssSignature.str().c_str() );
                ImGui::NextColumn();    // signature
                ImGui::Text( "%d ", archetype->Size() );
                ImGui::NextColumn();    // size
                if( ImGui::IsItemHovered() )
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
                    std::stringstream ss;
                    for( int          i = 0; i < archetype->Size(); i++ )
                    {
                        ss << archetype->GetEntityData( i ).mHandle << " ";
                    }
                    ImGui::TextUnformatted( ss.str().c_str() );
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }


                // chunks
                const fan::EditorSettings          & settings = _world.GetSingleton<fan::EditorSettings>();
                const std::vector<EcsComponentInfo>& infos    = _world.GetComponentInfos();
                for( int componentIndex = 0; componentIndex < _world.NumComponents(); componentIndex++ )
                {
                    if( archetype->GetSignature()[componentIndex] )
                    {
                        const EcsComponentInfo     & info    = infos[componentIndex];
                        const fan::GuiComponentInfo& guiInfo = settings.GetComponentInfo( info.mType );

                        std::stringstream ss;
                        ImGui::Icon( guiInfo.mIcon, { 16, 16 }, settings.mData->mGroupsColors.GetColor( guiInfo.mGroup ) );
                        ImGui::SameLine();
                        ss << info.mName.c_str();
                        for( int i = 0; i < 19 - (int)info.mName.size(); i++ )
                        {
                            ss << " ";
                        }
                        ss << ": ";

                        for( int chunkIndex = 0;
                             chunkIndex < archetype->GetChunkVector( componentIndex ).NumChunk();
                             chunkIndex++ )
                        {
                            ss << archetype->GetChunkVector( componentIndex ).GetChunk( chunkIndex ).Size()
                                    << " ";
                        }
                        ImGui::Text( ss.str().c_str() );

                        std::stringstream ssTooltip;
                        ssTooltip << info.mName << '\n';
                        ssTooltip << "component size: " << info.mSize;
                        ImGui::FanToolTip( ssTooltip.str().c_str() );
                    }
                }
                ImGui::NextColumn();
                ImGui::Separator();
            }
            ImGui::Columns( 1 );
        }

        // Handles
        if( ImGui::CollapsingHeader( "Handles" ) )
        {
            ImGui::Columns( 4 );
            ImGui::Text( "id" );
            ImGui::NextColumn();
            ImGui::Text( "handle" );
            ImGui::NextColumn();
            ImGui::Text( "archetype" );
            ImGui::NextColumn();
            ImGui::Text( "index" );
            ImGui::NextColumn();
            ImGui::Separator();

            int i = 0;
            for( const auto& pair : _world.GetHandles() )
            {
                const EcsHandle handle = pair.first;
                EcsEntity       entity = pair.second;

                ImGui::Text( "%d", i++ );
                ImGui::NextColumn();
                ImGui::Text( "%d", handle );
                ImGui::NextColumn();
                std::stringstream ss;
                ss << entity.mArchetype->GetSignature();
                ImGui::Text( "%s", ss.str().c_str() );
                ImGui::NextColumn();
                ImGui::Text( "%d", entity.mIndex );
                ImGui::NextColumn();
            }
            ImGui::Columns( 1 );
        }
    }

    //================================================================
    // helper function to create a formatted string like "Storage: 1024 (16Ko)"
    //================================================================
    std::string TagCountSize( const char* _tag, const size_t _count, const size_t _size )
    {
        std::stringstream ssStorage;
        ssStorage << _tag << ": " << _count << " (" << _count * _size / 1000 << "Ko)";
        return ssStorage.str();
    }
}