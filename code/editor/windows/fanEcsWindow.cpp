#include "editor/windows/fanEcsWindow.hpp"

#include <sstream>
#include "core/time/fanProfiler.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "ecs/fanEcsSystem.hpp"
#include "ecs/fanEcsTag.hpp"
#include "scene/fanSceneTags.hpp"
#include "editor/fanModals.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	EcsWindow::EcsWindow( EcsWorld& _world ) :
		EditorWindow( "ecs", ImGui::IconType::ECS16 )
		, m_world( &_world )
	{
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

	//================================================================================================================================
	//================================================================================================================================
	void EcsWindow::OnGui()
	{
		// Global
		if( ImGui::CollapsingHeader( "Global" ) )
		{
			ImGui::Text( "num chunks     : %d", EcsChunk::s_allocator.Size() );
			ImGui::Text( "total size (Mo): %.1f", float( EcsChunk::s_allocator.Size() * EcsChunk::s_allocator.chunkSize ) * 0.000001f );
		}

		// Archetypes
		if( ImGui::CollapsingHeader( "Archetypes" ) )
		{
			ImGui::Columns( 3 );
			ImGui::Text( "signature" ); ImGui::NextColumn();
			ImGui::Text( "size" );      ImGui::NextColumn();
			ImGui::Text( "chunks" );    ImGui::NextColumn();
			ImGui::Separator();

			std::vector<const EcsArchetype*> archetypes;
			const std::unordered_map< EcsSignature, EcsArchetype* >& archetypesRef = m_world->GetArchetypes();
			for( auto it = archetypesRef.begin(); it != archetypesRef.end(); ++it )
			{
				archetypes.push_back( it->second );
			} archetypes.push_back( &m_world->GetTransitionArchetype() );

			for( const EcsArchetype* archetype : archetypes )
			{
				std::stringstream ssSignature;
				ssSignature << archetype->m_signature;
				ImGui::Text( ssSignature.str().c_str() );	ImGui::NextColumn();		// signature
				ImGui::Text( "%d ", archetype->Size() );	ImGui::NextColumn();	// size

				// chunks
				const std::vector< EcsComponentInfo >& infos = m_world->GetVectorComponentInfo();
				for( int componentIndex = 0; componentIndex < archetype->m_chunks.size(); componentIndex++ )
				{
					if( archetype->m_chunks[componentIndex].NumChunk() != 0 )
					{
						const EcsComponentInfo& info = infos[componentIndex];
						std::stringstream ss;
						ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine();
						ss << info.name.c_str();
						for( int i = 0; i < 19 - info.name.size(); i++ )
						{
							ss << " ";
						}
						ss << ": ";

						for( int chunkIndex = 0; chunkIndex < archetype->m_chunks[componentIndex].NumChunk(); chunkIndex++ )
						{
							ss << archetype->m_chunks[componentIndex].GetChunk( chunkIndex ).Size() << " ";
						}
						ImGui::Text( ss.str().c_str() );


						std::stringstream ssTooltip;
						ssTooltip << info.name << '\n';
						ssTooltip << "component size: " << info.size;
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
			ImGui::Text( "id" );		ImGui::NextColumn();
			ImGui::Text( "handle" );	ImGui::NextColumn();
			ImGui::Text( "archetype" ); ImGui::NextColumn();
			ImGui::Text( "index" );	ImGui::NextColumn();
			ImGui::Separator();

			int i = 0;
			for( const auto& pair : m_world->GetHandles() )
			{
				const EcsHandle handle = pair.first;
				EcsEntity entity = pair.second;

				ImGui::Text( "%d", i++ );		ImGui::NextColumn();
				ImGui::Text( "%d", handle );	ImGui::NextColumn();
				std::stringstream ss;
				ss << entity.archetype->m_signature;
				ImGui::Text( "%s", ss.str().c_str() );	ImGui::NextColumn();
				ImGui::Text( "%d", entity.index );	ImGui::NextColumn();
			}
			ImGui::Columns( 1 );
		}
	}
}