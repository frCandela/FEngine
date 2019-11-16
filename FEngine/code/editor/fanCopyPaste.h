#pragma once

#include "scene/fanRessourcePtr.h"
#include "scene/fanComponentPtr.h"

namespace fan
{
	class Gameobject;
	class Scene;

	class CopyPaste
	{
	public:
		void Copy( const Gameobject * _gameobject );
		void Paste( Scene * _scene, Gameobject * _parentGameobject);


	private:

		Scene* m_scene = nullptr;

		std::vector< GameobjectPtr* >  m_newGameobjectPtr;
		std::vector< ComponentIDPtr* > m_newComponentPtr;
		std::map<uint64_t, uint64_t> m_remapTable;

		void OnSetIDFailed( uint64_t _id, Gameobject * _gameobject );
		void OnGameobjectPtrCreate( GameobjectPtr * _gameobjectPtr );
		void OnComponentIDPtrCreate( ComponentIDPtr * _ptr );
		void ResolvePointers();
	};
}