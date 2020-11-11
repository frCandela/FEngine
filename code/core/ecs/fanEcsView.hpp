#pragma once

#include "fanEcsArchetype.hpp"

namespace fan
{
	//================================
	//================================
	struct EcsView
	{
		EcsView( const std::unordered_map<uint32_t, int >& _typesToIndex, const EcsSignature _signature ) :
                mTypesToIndex( _typesToIndex )
			, mSignature( _signature )
		{
		}

		//================================
		//================================
		template < typename ComponentType >
		struct iterator
		{
			iterator( const EcsView& _view, const int _componentIndex )
			{
				if( _componentIndex == -1 )
				{
                    mArchetypeIndex = uint16_t( _view.mArchetypes.size() ); // end() iterator
				}
				else
				{
                    fanAssert( _view.mSignature[_componentIndex] );
                    mArchetypesList = _view.mArchetypes;
                    mComponentIndex = _componentIndex;
                    mChunkIndex     = 0;
                    mElementIndex   = 0;
                    mArchetypeIndex = 0;
					if( !mArchetypesList.empty() )
					{
                        mCurrentArchetype = mArchetypesList[mArchetypeIndex];
                        mCurrentChunk     = &mCurrentArchetype->GetChunkVector( _componentIndex ).GetChunk( mChunkIndex );
					}
				}
			}

			inline bool operator!=( const iterator& _other ) const
			{
				return mArchetypeIndex != _other.mArchetypeIndex;
			}

			void operator++() // prefix ++
			{
				++mElementIndex;
				if( mElementIndex >= mCurrentChunk->Size() )
				{
                    mElementIndex = 0;
					++mChunkIndex;

					if( mChunkIndex < mCurrentArchetype->GetChunkVector( mComponentIndex ).NumChunk() )
					{
                        mCurrentChunk = &mCurrentArchetype->GetChunkVector( mComponentIndex ).GetChunk( mChunkIndex );
					}
					else
					{
                        mChunkIndex = 0;
						++mArchetypeIndex;
						if( mArchetypeIndex < mArchetypesList.size() )
						{
                            mCurrentArchetype = mArchetypesList[mArchetypeIndex];
                            mCurrentChunk     = &mCurrentArchetype->GetChunkVector( mComponentIndex ).GetChunk( mChunkIndex );
						}
					}
				}
			}

			inline ComponentType& operator*()
			{
				return *static_cast<ComponentType*>( mCurrentChunk->At( mElementIndex ) );
			}

			EcsEntity GetEntity() const
			{
				const uint32_t index = mChunkIndex * mCurrentChunk->Capacity() + mElementIndex;
				return { mCurrentArchetype, index };
			}
		private:
			std::vector< EcsArchetype* > mArchetypesList;
			int                          mComponentIndex;
			uint16_t                     mArchetypeIndex;
			uint16_t                     mChunkIndex;
			uint16_t                     mElementIndex;
			EcsArchetype*                mCurrentArchetype;
			EcsChunk*                    mCurrentChunk;
		};

		template < typename _ComponentType >
		iterator<_ComponentType> begin() const
		{
			const int index = mTypesToIndex.at( _ComponentType::Info::sType );
			return iterator<_ComponentType>( *this, index );
		}

		template < typename _ComponentType >
		inline iterator<_ComponentType> end() const
		{
			return iterator<_ComponentType>( *this, -1 );
		}

		int Size() const
		{
			int size = 0;
			for( int i = 0; i < (int)mArchetypes.size(); i++ )
			{
				size += mArchetypes[i]->Size();
			}
			return size;
		}

		bool Empty() const
		{
			return Size() == 0;
		}

        const std::unordered_map<uint32_t, int >& mTypesToIndex;
        const EcsSignature                        mSignature;
        std::vector<EcsArchetype*>                mArchetypes;
	};
}