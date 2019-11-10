#pragma once

namespace fan
{
	class Gameobject;

	//================================================================================================================================
	// A gameobject pointer is used to reference a gameobject using a pointer and a id that can be serialized
	//================================================================================================================================
	class GameobjectPtr
	{		
	public:
		static Signal< GameobjectPtr * > s_onCreateUnresolved;
		static Signal< GameobjectPtr * > s_onSetFromSelection;

		explicit GameobjectPtr( Gameobject * _gameobject );		
		GameobjectPtr();

		uint64_t GetID( ) const { return m_id; }
		void	 InitUnresolved( const uint64_t _id );
		Gameobject* operator->() const	{ return m_gameobject; }
		Gameobject* operator*() const{ return m_gameobject; }

	private:
		Gameobject *	m_gameobject = nullptr;
		uint64_t		m_id = 0;
	};



}

namespace ImGui
{
	void GameobjectPtr( const char * _label, fan::GameobjectPtr * _ptr );
}
