#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/fanColor.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
	//========================================================================================================
	// The grid in the 3D view of the editor
	//========================================================================================================
	struct EditorGrid : public EcsSingleton
	{
		ECS_SINGLETON( EditorGrid )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		Vector3   mOffset;
		Color     mColor;
		Fixed     mSpacing;
		int       mLinesCount;
		bool      mIsVisible;

		static void Draw( EcsWorld& _world );
	};
}