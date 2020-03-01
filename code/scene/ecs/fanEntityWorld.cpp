#include "fanEntityWorld.hpp"

namespace fan
{
	REGISTER_COMPONENT( PositionComponent, "position" );
	REGISTER_COMPONENT( ColorComponent, "color" );

	REGISTER_SINGLETON_COMPONENT( sc_sunLight, "sunlight" );

	REGISTER_TAG( tag_editorOnly, "editor_only" );
	REGISTER_TAG( tag_alwaysUpdate, "always_update" );	

	REGISTER_SYSTEM( UpdateAABBFromRigidbodySystem, "UpdateAABBFromRigidbodySystem" );
}
