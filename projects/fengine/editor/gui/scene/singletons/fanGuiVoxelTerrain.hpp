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

        static bool GuiNoiseOctave( const char* _name, NoiseOctave& _octave )
        {
            bool generateTerrain = false;
            ImGui::PushID( _name );
            if( ImGui::CollapsingHeader( _name ) )
            {

                ImGui::Indent();
                // Amplitude
                 float amplitude = _octave.mAmplitude.ToFloat();
                if( ImGui::SliderFloat( "amplitude", &amplitude, 0, 10 ) )
                {
                    _octave.mAmplitude = Fixed::FromFloat( amplitude );
                    generateTerrain = true;
                }

                // frequency
                 float frequency = _octave.mFrequency.ToFloat();
                if( ImGui::SliderFloat( "frequency", &frequency, 0, 1 ) )
                {
                    _octave.mFrequency = Fixed::FromFloat( frequency );
                    generateTerrain = true;
                }

                // height weight
                 float heightWeight = _octave.mHeightWeight.ToFloat();
                if( ImGui::SliderFloat( "height weight", &heightWeight, 0, 1 ) )
                {
                    _octave.mHeightWeight = Fixed::FromFloat( heightWeight );
                    generateTerrain = true;
                }

                // height offset
                 float heightOffset = _octave.mHeightOffset.ToFloat();
                if( ImGui::DragFloat( "height offset", &heightOffset, 0.05f, -1, 1 ) )
                {
                    _octave.mHeightOffset = Fixed::FromFloat( heightOffset );
                    generateTerrain = true;
                }

                // weight
                float weight = _octave.mWeight.ToFloat();
                if( ImGui::SliderFloat( "weight", &weight, 0, 1 ) )
                {
                    _octave.mWeight = Fixed::FromFloat( weight );
                    generateTerrain = true;
                }

                ImGui::Unindent();
            }
            ImGui::PopID();
            return generateTerrain;
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

                // seed
                if( ImGui::DragInt( "seed", &voxelTerrain.mGenerator.mSeed, 1, 0, 1024 ) )
                {
                    voxelTerrain.mGenerator.mSimplexNoise = SimplexNoise( voxelTerrain.mGenerator.mSeed );
                    generateTerrain = true;
                }

                // chunk size
                glm::ivec3 terrainSize = voxelTerrain.mSize;
                if( ImGui::DragInt3( "terrain size", &terrainSize.x ) )
                {
                    if( voxelTerrain.mIsInitialized )
                    {
                        VoxelTerrain::ClearTerrain( _world, voxelTerrain );
                    }
                    voxelTerrain.mSize = terrainSize;
                }
                ImGui::PushReadOnly();
                int chunkSize = VoxelChunk::sSize;
                ImGui::DragInt( "chunk size", &chunkSize );
                ImGui::PopReadOnly();

                // treshold
                float treshold =  voxelTerrain.mGenerator.mThreshold.ToFloat();
                if( ImGui::SliderFloat( "treshold", &treshold, 0, 1 ) )
                {
                    voxelTerrain.mGenerator.mThreshold = Fixed::FromFloat( treshold );
                    generateTerrain = true;
                }

                // clear
                if( ImGui::Checkbox( "clear sides", & voxelTerrain.mGenerator.mClearSides ) )
                {
                    generateTerrain = true;
                }

                // noise octaves
                const char * noisesNames[3]  = { "2D octave", "3D octave 0", "3D octave 1" };
                NoiseOctave* noiseOctaves[3] = { &voxelTerrain.mGenerator.m2DOctave, &voxelTerrain.mGenerator.m3DOctaves[0], &voxelTerrain.mGenerator.m3DOctaves[1] };
                for( int i = 0; i < 3; i++ )
                {
                    generateTerrain |= GuiNoiseOctave( noisesNames[i], *noiseOctaves[i] );
                }

                if( generateTerrain )
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
                        VoxelTerrain::InitializeTerrain( _world, voxelTerrain );
                    }
                }
            }
            ImGui::Unindent();
        }
    };
}