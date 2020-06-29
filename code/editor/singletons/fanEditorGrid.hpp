#pragma once

#include "bullet/LinearMath/btVector3.h"
#include "ecs/fanEcsSingleton.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//================================================================================================================================
	// The grid in the 3D view of the editor
	//================================================================================================================================
	struct EditorGrid : public EcsSingleton
	{
		ECS_SINGLETON( EditorGrid )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		btVector3	offset;
		Color		color;
		float		spacing;
		int			linesCount;
		bool		isVisible;

		static void Draw( EcsWorld& _world );
	};
}