#include "fanEntityWorld.hpp"

#include "scene/ecs/components/fanSceneNode.hpp"

namespace fan
{
	REGISTER_COMPONENT( PositionComponent, "position" );
	REGISTER_COMPONENT( ColorComponent, "color" );

	REGISTER_SINGLETON_COMPONENT( sc_sunLight, "sunlight" );

	REGISTER_TAG( tag_editorOnly, "editor_only" );
	REGISTER_TAG( tag_alwaysUpdate, "always_update" );	

	REGISTER_SYSTEM( UpdateAABBFromRigidbodySystem, "UpdateAABBFromRigidbodySystem" );

	Signature UpdateAABBFromRigidbodySystem::GetSignature( const EntityWorld& _world )
	{
		return _world.GetSignature<tag_editorOnly>() | _world.GetSignature<ColorComponent>();
	}

	void UpdateAABBFromRigidbodySystem::Run( EntityWorld& _world, const std::vector<Entity*>& _entities, const float _delta )
	{
		sc_sunLight& sunlight = _world.GetSingletonComponent<sc_sunLight>();
		++sunlight.test;
	}

	EntityWorld::EntityWorld()
	{

		AddSingletonComponentType<sc_sunLight>();

		AddComponentType<PositionComponent>();
		AddComponentType<ColorComponent>();
		AddComponentType<SceneNode>();

		AddTagType<tag_alwaysUpdate>();
		AddTagType<tag_editorOnly>();

		assert( m_nextTypeIndex < 1 << ( 8 * sizeof( ComponentIndex ) ) );

	}
}
