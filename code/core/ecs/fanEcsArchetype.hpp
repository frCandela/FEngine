#pragma once

#include "fanEcsChunkVector.hpp"
#include "fanEcsComponent.hpp"

namespace fan
{
	//========================================================================================================
	// An archetype is a collection of entities with the same signature
	// All component of the same entity are at the same index
	// components are put in arrays of small buffers called chunks
	//========================================================================================================
	class EcsArchetype
	{
	public:
		EcsArchetype();
		void Create( const std::vector< EcsComponentInfo >& _componentsInfo, const EcsSignature& _signature );
		void AddComponentType( const EcsComponentInfo& _componentsInfo );
		void RemoveEntity( const int _entityIndex );
		void Clear();
		int	 Size() const{ return int( mEntities.size() ); }
		bool Empty() const{ return mEntities.empty(); }
		void PushBackEntityData( const EcsEntityData& _entityData )	{ mEntities.push_back( _entityData ); }

		const EcsEntityData&	GetEntityData ( const int _index ) const	{ return mEntities[_index];	}
		EcsEntityData&			GetEntityData ( const int _index )			{ return mEntities[_index];	}
		const EcsChunkVector&	GetChunkVector( const int _index ) const	{ return mChunkVectors[_index];}
		EcsChunkVector&			GetChunkVector( const int _index )			{ return mChunkVectors[_index];}
		const EcsSignature&		GetSignature  () const						{ return mSignature;			}

	private:
		EcsSignature                  mSignature;
		std::vector< EcsChunkVector > mChunkVectors;		// one index per component type
		std::vector< EcsEntityData >  mEntities;
	};
}