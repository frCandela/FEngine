#include "editor/windows/fanEcsWindow.hpp"

#include "core/time/fanProfiler.hpp"
#include "scene/ecs/fanComponentsCollection.hpp"
#include "scene/ecs/fanSystem.hpp"
#include "scene/ecs/fanTag.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	EcsWindow::EcsWindow() :
		EditorWindow( "ecs", ImGui::IconType::ECS16 )
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
		SCOPED_PROFILE( ecs )

		EcsWorld& world = *m_world;

		//============================	
		if( ImGui::CollapsingHeader( "components" ) )
		{
			ImGui::Columns( 4 );
			ImGui::SetColumnWidth( 0, 150.f );
			ImGui::SetColumnWidth( 1, 100.f );
			ImGui::SetColumnWidth( 2, 100.f );
			ImGui::SetColumnWidth( 3, 100.f );

			ImGui::NextColumn();
			ImGui::Text( "components" );
			ImGui::NextColumn();
			ImGui::Text( "chunks" );
			ImGui::NextColumn();
			ImGui::Text( "size (Ko)" );
			ImGui::NextColumn();
			ImGui::Separator();

			const std::vector< ComponentsCollection >& components = world.GetComponentCollections();
			for( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				const ComponentsCollection& collection = components[componentIndex];
				const std::vector< ComponentsCollection::Chunck >& chunks = collection.GetChuncks();

				size_t numElements = 0;
				for( int chunckIndex = 0; chunckIndex < chunks.size(); chunckIndex++ )
				{
					numElements += chunks[chunckIndex].count;
				}

				ImGui::Text( collection.GetName().c_str() );
				if( ImGui::IsItemHovered() )
				{
					ImGui::BeginTooltip();
					ImGui::Text( "chunk length: %u", collection.GetComponentCount() );
					ImGui::TextUnformatted( "to recycle" );
					for( int chunckIndex = 0; chunckIndex < chunks.size(); chunckIndex++ )
					{
						ImGui::Text( std::to_string( chunks[chunckIndex].recycleList.size() ).c_str() );
					}
					ImGui::EndTooltip();
				}

				// num elements
				ImGui::NextColumn();
				ImGui::Text( std::to_string( numElements ).c_str() );

				// num chunks
				ImGui::NextColumn();
				ImGui::Text( std::to_string( chunks.size() ).c_str() );

				// size of a chunk in Ko
				ImGui::NextColumn();
				ImGui::Text( std::to_string( chunks.size() * collection.GetComponentCount() * collection.GetComponentSize() / 1000 ).c_str() );

				ImGui::NextColumn();
			}
			ImGui::Columns( 1 );
		}

		//============================	
		if( ImGui::CollapsingHeader( "Testing" ) )
		{

			static bool createHandle = false;
			static bool useColor = true;
			static bool usePosition = true;
			static bool useTag_boundsOutdated = true;
			static bool useTag_editorOnly = true;
			static int num = 1;

			ImGui::Checkbox( "create handle", &createHandle );
			ImGui::Checkbox( "position", &usePosition );
			ImGui::Checkbox( "color", &useColor );
			ImGui::Checkbox( "tag bounds outdated", &useTag_boundsOutdated );
			ImGui::Checkbox( "tag editorOnly", &useTag_editorOnly );
			ImGui::DragInt( "num", &num, 1, 1, 100000 );

			if( ImGui::Button( "Sort" ) )
			{
				world.SortEntities();
			}ImGui::SameLine();

			if( ImGui::Button( "RemoveDeadEntities" ) )
			{
				world.RemoveDeadEntities();
			}ImGui::SameLine();

			//============================
			if( ImGui::Button( "Create" ) )
			{
				for( int i = 0; i < num; i++ )
				{
					EntityID id = world.CreateEntity();
					if( useTag_boundsOutdated ) world.AddTag<tag_boundsOutdated>( id );
					if( useTag_editorOnly ) world.AddTag < tag_editorOnly>( id );
					if( createHandle ) { world.CreateHandle( id ); }
				}
			}ImGui::SameLine();

			if( ImGui::Button( "Delete" ) )
			{
				for( int i = 0; i < num; i++ )
				{
					world.KillEntity( (EntityID)world.GetNumEntities() - i - 1 );
				}
			}ImGui::SameLine();
		}

		//============================	
		if( ImGui::CollapsingHeader( "singleton components" ) )
		{
			ImGui::Text( "num components: %d", world.GetNumSingletonComponents() );
		}

		//============================	
		if( ImGui::CollapsingHeader( "handles" ) )
		{
			for( const std::pair< EntityHandle, EntityID > handle : world.GetHandles() )
			{
				ImGui::Text( "%d -> %d", handle.first, handle.second );
			}
		}

		//============================
		const std::vector< Entity >& entities = world.GetEntities();
		std::string name = "entities " + std::to_string( entities.size() );
		if( ImGui::CollapsingHeader( name.c_str() ) )
		{
			for( int entityIndex = 0; entityIndex < entities.size(); entityIndex++ )
			{
				const Entity& entity = entities[entityIndex];
				ImGui::PushID( entityIndex );
				if( ImGui::Button( "X" ) )
				{
					world.KillEntity( entityIndex );
				} ImGui::PopID();
				ImGui::SameLine();
				std::stringstream ss;
				ss << entityIndex << "\t: alive: " << entity.IsAlive() << " ";

				for( int bitIndex = 0; bitIndex < signatureLength; bitIndex++ )
				{
					ss << entity.signature[bitIndex];
				}
				ImGui::Text( ss.str().c_str() );
			}
		}

		if( ImGui::CollapsingHeader( "components & tags" ) )
		{
			for ( const std::pair< uint32_t, ComponentIndex >& pair : world.GetDynamicIndices() )
			{
				ImGui::Text( "%u -> %u ", pair.first, pair.second );
			}			
		}
	}
}