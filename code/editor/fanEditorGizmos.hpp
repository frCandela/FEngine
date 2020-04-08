#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// allows displaying of the translation manipulator
	// @todo make this a singleton component in the editor ecs world
	//================================================================================================================================	
	class EditorGizmos
	{
		//================================================================
		//================================================================	
		struct GizmoCacheData
		{
			int axisIndex;
			bool pressed = false;
			btVector3 offset;
		};

	public:

		EditorGizmos( EcsWorld& _world );
		bool DrawMoveGizmo( const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition );

		static EditorGizmos& Get() { return *s_editorGizmos; }
		static void Init( EditorGizmos* const _editorGizmos ) { assert( s_editorGizmos == nullptr ); s_editorGizmos = _editorGizmos; }
	private:
		static EditorGizmos* s_editorGizmos; // Used for global debug draw

		std::map< size_t, GizmoCacheData > m_gizmoCacheData;
		EcsWorld* m_world;
	};
}