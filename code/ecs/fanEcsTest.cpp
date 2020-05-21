#include "ecs/fanEcsTest.hpp"

namespace fan
{
	EcsView EcsWorld2::Match( const EcsSignature _signature ) const
	{
		EcsView view( *this, _signature );
		for( auto it = m_archetypes.begin(); it != m_archetypes.end(); ++it )
		{
			if( ( it->first & _signature ) == _signature && !it->second->Empty() )
			{
				view.m_archetypes.push_back( it->second );
			}
		}
		return view;
	}
}