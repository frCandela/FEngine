#include "scene/ecs/fanEntity.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ecComponent& Entity::GetComponent( const ComponentIndex _index )
	{
		for( int i = 0; i < componentCount; i++ )
		{
			if( components[i]->GetIndex() == _index )
			{
				return *components[i];
			}
		}
		assert( false );
		return *(ecComponent*)( 0 );
	}
}