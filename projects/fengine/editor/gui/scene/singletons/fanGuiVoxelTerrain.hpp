#pragma once

#include "engine/terrain/fanVoxelTerrain.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiVoxelTerrain
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::None16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiVoxelTerrain::OnGui;
            info.mEditorName = "voxel terrain";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsSingleton& _component )
        {
            VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _component );

            ImGui::Indent();
            {
                bool generateTerrain = false;

                if( ImGui::Button( "generate" ) )
                {
                    generateTerrain = true;
                }

                // Amplitude
                static float amplitude = voxelTerrain.mGenerator.mAmplitude.ToFloat();
                if( ImGui::SliderFloat( "amplitude", &amplitude, 0, 10 ) )
                {
                    voxelTerrain.mGenerator.mAmplitude = Fixed::FromFloat( amplitude );
                    generateTerrain = true;
                }

                // frequency
                static float frequency = voxelTerrain.mGenerator.mFrequency.ToFloat();
                if( ImGui::SliderFloat( "frequency", &frequency, 0, 1 ) )
                {
                    voxelTerrain.mGenerator.mFrequency = Fixed::FromFloat( frequency );
                    generateTerrain = true;
                }

                // seed
                if( ImGui::DragInt( "seed", &voxelTerrain.mGenerator.mSeed, 1, 0, 1024 ) )
                {
                    voxelTerrain.mGenerator.Initialize();
                    generateTerrain = true;
                }

                // chunk size
                glm::ivec3 terrainSize = voxelTerrain.mSize;
                if( ImGui::DragInt3( "terrain size", &terrainSize.x ) )
                {
                    if( voxelTerrain.mIsInitialized )
                    {
                        voxelTerrain.ClearTerrain( _world );
                    }
                    voxelTerrain.mSize = terrainSize;
                }
                ImGui::PushReadOnly();
                int chunkSize = VoxelChunk::sSize;
                ImGui::DragInt( "chunk size", &chunkSize );
                ImGui::PopReadOnly();

                if( generateTerrain)
                {
                    if( voxelTerrain.mIsInitialized )
                    {
                        for( int i = 0; i < voxelTerrain.mSize.x * voxelTerrain.mSize.y * voxelTerrain.mSize.z; i++ )
                        {
                            voxelTerrain.mChunks[i].mIsGenerated    = false;
                            voxelTerrain.mChunks[i].mIsMeshOutdated = true;
                        }
                    }
                    else
                    {
                        voxelTerrain.InitializeTerrain( _world );
                    }
                }
            }
            ImGui::Unindent();
        }
    };
}