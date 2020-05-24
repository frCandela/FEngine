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
			// Archetypes
			if( ImGui::CollapsingHeader( "archetypes" ) )
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

				for ( const EcsArchetype* archetype : archetypes )
				{
					std::stringstream ssSignature;
					ssSignature << archetype->m_signature;
					ImGui::Text(  ssSignature.str().c_str() );	ImGui::NextColumn();		// signature
					ImGui::Text( "%d ", archetype->Size() );	ImGui::NextColumn();	// size

					// chunks
					const std::vector< EcsComponentInfo >& infos = m_world->GetVectorComponentInfo();
					for (int componentIndex = 0; componentIndex < archetype->m_chunks.size(); componentIndex++)
					{
						if( archetype->m_chunks[componentIndex].NumChunk() != 0 )
						{
							const EcsComponentInfo& info = infos[componentIndex];
							std::stringstream ss;
							ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine();
							ss << info.name.c_str();
							for (int i = 0; i < 19 - info.name.size(); i++)
							{
								ss << " ";
							}
							ss <<": ";

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
			/*
			// Entities
			if( ImGui::CollapsingHeader( "Entities" ) )
			{
				enum class Mode{ removeEntity, removeComponent, addComponent, addTag, removeTag, createHandle, removeHandle };
				static Mode mode = Mode::removeEntity;

				bool removeEntityVal = mode == Mode::removeEntity;
				if( ImGui::Checkbox( "remove entity", &removeEntityVal ) ) { if( removeEntityVal ) mode = Mode::removeEntity; } ImGui::SameLine();
				bool removeComponentVal = mode == Mode::removeComponent;
				if( ImGui::Checkbox( "remove component", &removeComponentVal ) ) { if( removeComponentVal ) mode = Mode::removeComponent; } ImGui::SameLine();
				bool addComponentVal = mode == Mode::addComponent;
				if( ImGui::Checkbox( "add component", &addComponentVal ) ) { if( addComponentVal ) mode = Mode::addComponent; } ImGui::SameLine();
				bool removeTagVal = mode == Mode::removeTag;
				if( ImGui::Checkbox( "remove tag", &removeTagVal ) ) { if( removeTagVal ) mode = Mode::removeTag; } ImGui::SameLine();
				bool addTagVal = mode == Mode::addTag;
				if( ImGui::Checkbox( "add tag", &addTagVal ) ) { if( addTagVal ) mode = Mode::addTag; } ImGui::SameLine();
				bool createHandleVal = mode == Mode::createHandle;
				if( ImGui::Checkbox( "create handle", &createHandleVal ) ) { if( createHandleVal ) mode = Mode::createHandle; } ImGui::SameLine();
				bool removeHandleVal = mode == Mode::removeHandle;
				if( ImGui::Checkbox( "remove handle", &removeHandleVal ) ) { if( removeHandleVal ) mode = Mode::removeHandle; }

				const std::unordered_map< EcsSignature, EcsArchetype* >& archetypesRef = m_world2.GetArchetypes();
				for( std::pair<EcsSignature, EcsArchetype*> pair : archetypesRef )
				{
					EcsArchetype& archetype = *pair.second;
					std::stringstream ss;
					ss << archetype.m_signature;
					ImGui::Text( "%s", ss.str().c_str() );
					ImGui::PushID( ss.str().c_str() );
					for( int i = 0; i < archetype.Size(); i++ )
					{
						ImGui::SameLine();
						if( ImGui::Button( std::to_string(i).c_str() ) )
						{
							EcsEntity entity = { &archetype, uint32_t( i ) };
							switch( mode )
							{
							case Mode::removeEntity:
								m_world2.Kill( entity );
								break;
							case Mode::removeComponent:
								if( hasPosition ) { m_world2.RemoveComponent( entityID, Position2::Info::s_type ); }
								if( hasSpeed ) { m_world2.RemoveComponent( entityID, Speed2::Info::s_type ); }
								if( hasExpiration ) { m_world2.RemoveComponent( entityID, Expiration2::Info::s_type ); }
								break;
							case Mode::addComponent:
								if( hasPosition ) { m_world2.AddComponent( entityID, Position2::Info::s_type ); }
								if( hasSpeed ) { m_world2.AddComponent( entityID, Speed2::Info::s_type ); }
								if( hasExpiration ) { m_world2.AddComponent( entityID, Expiration2::Info::s_type ); }
								break;
							case Mode::removeTag:
								if( hasTagBounds ) { m_world2.RemoveTag( entityID, TagBoundsOutdated::s_type ); }
								break;
							case Mode::addTag:
								if( hasTagBounds ) { m_world2.AddTag( entityID, TagBoundsOutdated::s_type ); }
								break;
							case Mode::createHandle:
								m_world2.AddHandle( entity );
								break;
							case Mode::removeHandle:
								m_world2.RemoveHandle( entity );
								break;
							default:
								assert( false );
								break;
							}

						}
					}
					ImGui::PopID();
				}
			}*/

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
				for ( const auto& pair : m_world->GetHandles() )
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



// 		SCOPED_PROFILE( ecs ) @migration
// 
// 		EcsWorld& world = *m_world;
// 
//  		//============================	
// 		if( ImGui::CollapsingHeader( "components" ) )
// 		{
// 			ImGui::Columns( 4 );
// 			ImGui::SetColumnWidth( 0, 150.f );
// 			ImGui::SetColumnWidth( 1, 100.f );
// 			ImGui::SetColumnWidth( 2, 100.f );
// 			ImGui::SetColumnWidth( 3, 100.f );
// 
// 			ImGui::NextColumn();
// 			ImGui::Text( "components" );
// 			ImGui::NextColumn();
// 			ImGui::Text( "chunks" );
// 			ImGui::NextColumn();
// 			ImGui::Text( "size (Ko)" );
// 			ImGui::NextColumn();
// 			ImGui::Separator();
// 
// 			const std::vector< ComponentsCollection >& components = world.GetComponentCollections();
// 			for( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
// 			{
// 				const ComponentsCollection& collection = components[componentIndex];
// 				const std::vector< ComponentsCollection::Chunck >& chunks = collection.GetChuncks();
// 
// 				size_t numElements = 0;
// 				for( int chunckIndex = 0; chunckIndex < chunks.size(); chunckIndex++ )
// 				{
// 					numElements += chunks[chunckIndex].count;
// 				}
// 
// 				ImGui::Text( collection.GetName().c_str() );
// 				if( ImGui::IsItemHovered() )
// 				{
// 					ImGui::BeginTooltip();
// 					ImGui::Text( "chunk length: %u", collection.GetComponentCount() );
// 					ImGui::TextUnformatted( "to recycle" );
// 					for( int chunckIndex = 0; chunckIndex < chunks.size(); chunckIndex++ )
// 					{
// 						ImGui::Text( std::to_string( chunks[chunckIndex].recycleList.size() ).c_str() );
// 					}
// 					ImGui::EndTooltip();
// 				}
// 
// 				// num elements
// 				ImGui::NextColumn();
// 				ImGui::Text( std::to_string( numElements ).c_str() );
// 
// 				// num chunks
// 				ImGui::NextColumn();
// 				ImGui::Text( std::to_string( chunks.size() ).c_str() );
// 
// 				// size of a chunk in Ko
// 				ImGui::NextColumn();
// 				ImGui::Text( std::to_string( chunks.size() * collection.GetComponentCount() * collection.GetComponentSize() / 1000 ).c_str() );
// 
// 				ImGui::NextColumn();
// 			}
// 			ImGui::Columns( 1 );
// 		}
// 
// 		//============================	
// 		if( ImGui::CollapsingHeader( "Testing" ) )
// 		{
// 
// 			static bool createHandle = false;
// 			static bool useColor = true;
// 			static bool usePosition = true;
// 			static bool useTag_boundsOutdated = true;
// 			static bool useTag_editorOnly = true;
// 			static int num = 1;
// 
// 			ImGui::Checkbox( "create handle", &createHandle );
// 			ImGui::Checkbox( "position", &usePosition );
// 			ImGui::Checkbox( "color", &useColor );
// 			ImGui::Checkbox( "tag bounds outdated", &useTag_boundsOutdated );
// 			ImGui::Checkbox( "tag editorOnly", &useTag_editorOnly );
// 			ImGui::DragInt( "num", &num, 1, 1, 100000 );
// 
// 			if( ImGui::Button( "Sort" ) )
// 			{
// 				world.SortEntities();
// 			}ImGui::SameLine();
// 
// 			if( ImGui::Button( "RemoveDeadEntities" ) )
// 			{
// 				world.RemoveDeadEntities();
// 			}ImGui::SameLine();
// 
// 			//============================
// 			if( ImGui::Button( "Create" ) )
// 			{
// 				for( int i = 0; i < num; i++ )
// 				{
// 					EcsEntity id = world.CreateEntity();
// 					if( useTag_boundsOutdated ) world.AddTag<tag_boundsOutdated>( id );
// 					if( useTag_editorOnly ) world.AddTag < tag_editorOnly>( id );
// 					if( createHandle ) { world.AddHandle( id ); }
// 				}
// 			}ImGui::SameLine();
// 
// 			if( ImGui::Button( "Delete" ) )
// 			{
// 				for( int i = 0; i < num; i++ )
// 				{
// 					world.Kill( (EcsEntity)world.GetNumEntities() - i - 1 );
// 				}
// 			}ImGui::SameLine();
// 		}
// 
// 		//============================	
// 		if( ImGui::CollapsingHeader( "handles" ) )
// 		{
// 			for( const std::pair< EcsHandle, EcsEntity > handle : world.GetHandles() )
// 			{
// 				ImGui::Text( "%d -> %d", handle.first, handle.second );
// 			}
// 		}
// 
// 		//============================
// 		const std::vector< Entity >& entities = world.GetEntities();
// 		std::string name = "entities " + std::to_string( entities.size() );
// 		if( ImGui::CollapsingHeader( name.c_str() ) )
// 		{
// 			for( int entityIndex = 0; entityIndex < entities.size(); entityIndex++ )
// 			{
// 				const Entity& entity = entities[entityIndex];
// 				ImGui::PushID( entityIndex );
// 				if( ImGui::Button( "X" ) )
// 				{
// 					world.Kill( entityIndex );
// 				} ImGui::PopID();
// 				ImGui::SameLine();
// 				std::stringstream ss;
// 				ss << entityIndex << "\t: alive: " << entity.IsAlive() << " ";
// 
// 				for( int bitIndex = 0; bitIndex < signatureLength; bitIndex++ )
// 				{
// 					ss << entity.signature[bitIndex];
// 				}
// 				ImGui::Text( ss.str().c_str() );
// 			}
// 		}
// 
// 		if( ImGui::CollapsingHeader( "components & tags" ) )
// 		{
// 			for ( const std::pair< uint32_t, int >& pair : world.GetDynamicIndices() )
// 			{
// 				ImGui::Text( "%u -> %u ", pair.first, pair.second );
// 			}			
// 		}
	}
}