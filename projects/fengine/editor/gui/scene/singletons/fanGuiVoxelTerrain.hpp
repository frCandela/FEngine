#pragma once

#include "engine/singletons/fanVoxelTerrain.hpp"
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
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _component );

            ImGui::Indent();
            {
                bool generateTerrain = ImGui::Button( "generate" );

                // Amplitude
                static float amplitude = voxelTerrain.mAmplitude.ToFloat();
                if( ImGui::SliderFloat( "amplitude", &amplitude, 0, 1 ) )
                {
                    voxelTerrain.mAmplitude = Fixed::FromFloat( amplitude );
                    generateTerrain = true;
                }

                // frequency
                static float frequency = voxelTerrain.mFrequency.ToFloat();
                if( ImGui::SliderFloat( "frequency", &frequency, 0, 1 ) )
                {
                    voxelTerrain.mFrequency = Fixed::FromFloat( frequency );
                    generateTerrain = true;
                }

                // seed
                if( ImGui::DragInt( "seed", &voxelTerrain.mSeed, 1, 0, 1024 ) )
                {
                    voxelTerrain.mSimplexNoise = SimplexNoise( voxelTerrain.mSeed );
                    generateTerrain = true;
                }



                // chunk size
                ImGui::PushReadOnly();
                int chunkSize = VoxelTerrain::sChunkSize;
                ImGui::DragInt( "chunk size", &chunkSize );

                ImGui::PopReadOnly();

                if( generateTerrain )
                {
                    VoxelTerrain::GenerateTerrain( voxelTerrain );
                }
            }
            ImGui::Unindent();
        }
    };
}