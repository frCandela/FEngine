#include "editor/windows/fanTerrainWindow.hpp"
#include "editor/fanModals.hpp"
#include "engine/terrain/fanVoxelGeneration.hpp"
#include "engine/terrain/fanVoxelTerrain.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    TerrainWindow::TerrainWindow() : EditorWindow( "terrain editor", ImGui::IconType::Terrain16 )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TerrainWindow::OnGui( EcsWorld& _world )
    {
        VoxelTerrain& terrain = _world.GetSingleton<VoxelTerrain>();

        bool generateTerrain = false;

        ImGui::Icon( ImGui::IconType::Terrain16, {16,16});
        ImGui::SameLine();

        if( ImGui::Button( "generate" ) )
        {
            generateTerrain = true;
        }

        // seed
        if( ImGui::DragInt( "seed", &terrain.mGenerator.mSeed, 1, 0, 1024 ) )
        {
            terrain.mGenerator.mSimplexNoise = SimplexNoise( terrain.mGenerator.mSeed );
            generateTerrain = true;
        }

        // chunk size
        glm::ivec3 terrainSize = terrain.mSize;
        if( ImGui::DragInt3( "terrain size", &terrainSize.x ) )
        {
            if( terrain.mIsInitialized )
            {
                VoxelTerrain::ClearTerrain( _world, terrain );
            }
            terrain.mSize = terrainSize;
        }
        ImGui::PushReadOnly();
        int chunkSize = VoxelChunk::sSize;
        ImGui::DragInt( "chunk size", &chunkSize );
        ImGui::PopReadOnly();

        // treshold
        float treshold = terrain.mGenerator.mThreshold.ToFloat();
        if( ImGui::SliderFloat( "treshold", &treshold, 0, 1 ) )
        {
            terrain.mGenerator.mThreshold = Fixed::FromFloat( treshold );
            generateTerrain = true;
        }

        // clear
        if( ImGui::Checkbox( "clear sides", &terrain.mGenerator.mClearSides ) )
        {
            generateTerrain = true;
        }

        // noise octaves
        const char * noisesNames[3]  = { "2D octave", "3D octave 0", "3D octave 1" };
        NoiseOctave* noiseOctaves[3] = { &terrain.mGenerator.m2DOctave, &terrain.mGenerator.m3DOctaves[0], &terrain.mGenerator.m3DOctaves[1] };
        for( int i = 0; i < 3; i++ )
        {
            generateTerrain |= GuiNoiseOctave( noisesNames[i], *noiseOctaves[i] );
        }

        if( generateTerrain )
        {
            if( terrain.mIsInitialized )
            {
                for( int i = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; i++ )
                {
                    terrain.mChunks[i].mIsGenerated    = false;
                    terrain.mChunks[i].mIsMeshOutdated = true;
                }
            }
            else
            {
                VoxelTerrain::InitializeTerrain( _world, terrain );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool TerrainWindow::GuiNoiseOctave( const char* _name, NoiseOctave& _octave )
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
}