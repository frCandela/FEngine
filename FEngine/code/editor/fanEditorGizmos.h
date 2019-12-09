#pragma once

namespace fan
{
	class Scene;

	//================================================================================================================================
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

		EditorGizmos( Scene*& _currentScene );
		bool DrawMoveGizmo(const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition);

	private:

		std::map< size_t, GizmoCacheData > m_gizmoCacheData;
		Scene*& m_currentScene;
	};
}