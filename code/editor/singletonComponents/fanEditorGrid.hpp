#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "ecs/fanSingletonComponent.hpp"

namespace fan
{

	//================================================================================================================================
	// The grid in the 3D view of the editor
	//================================================================================================================================
	struct EditorGrid : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		btVector3	offset;
		Color		color;
		float		spacing;
		int			linesCount;
		bool		isVisible;

		static void Draw( const EditorGrid& _grid );
	};
}