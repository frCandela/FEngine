#pragma once

#include "core/meta/fanHelpers.h"
#include "core/meta/fanTypeList.h"
#include "ecs/fanEcsComponentsKey.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	template < typename _type >
	struct Chunck
	{
		//================================================================
		_type * Alloc( ecsComponentIndex::chunckType& _outIndex )
		{
			if ( m_count < m_data.size() )
			{
				_outIndex = m_count++;
				return &m_data[_outIndex];
			}
			else if ( m_countRecycleList > 0 )
			{
				_outIndex = m_recycleList[--m_countRecycleList];
				return &m_data[_outIndex];
			}
			else{
				return nullptr;
			}
		}

		//================================================================
		void Delete( const ecsComponentIndex::chunckType _index )
		{
			assert( _index < m_count && _index < 256 );
			m_recycleList[m_countRecycleList++] = (uint8_t)_index;
		}

		//================================================================
		inline _type& operator[] ( const ecsComponentIndex::chunckType& _index ) { return m_data[_index]; }
		inline ecsComponentIndex::chunckType Count() const { return m_count - m_countRecycleList; }
		inline ecsComponentIndex::chunckType RecycleCount() const { return m_countRecycleList; }

	private:
		std::array< _type, 256 >	m_data;					// Components data
		std::array< ecsComponentIndex::chunckType, 256>	m_recycleList;			// Unused components
		ecsComponentIndex::chunckType						m_count = 0;			// Number of components
		ecsComponentIndex::chunckType						m_countRecycleList = 0;	// Number of components
	};

	//================================================================================================================================
	//================================================================================================================================
	template < typename _type >	
	class ComponentData 
	{
	public:
		static constexpr size_t index = IndexOfComponent<_type>::value;

		//================================================================
		inline _type& operator[] ( const ecsComponentsKey& _entityData ) {	return Get( _entityData.index[index] ); }
		inline _type& Get ( const ecsComponentIndex& _componentIndex )	{ return (*m_chunks[_componentIndex.chunckIndex])[_componentIndex.componentIndex];	}
		inline const std::vector< Chunck<_type> * >& GetChuncks() const { return m_chunks; }

		//================================================================
		_type& Alloc( ecsComponentIndex& _outecsComponentIndex )
		{
			_type * newElement = nullptr;
			 
			// Find a space in existing chunks
			for ( _outecsComponentIndex.chunckIndex = 0; _outecsComponentIndex.chunckIndex < m_chunks.size() ; _outecsComponentIndex.chunckIndex++)
			{
				newElement = m_chunks[ _outecsComponentIndex.chunckIndex ]->Alloc( _outecsComponentIndex.componentIndex );
				if ( newElement != nullptr ) 
				{
					return *newElement;
				}
			}

			// Alloc a new chunck
			Chunck<_type> * chunck = new Chunck<_type>();
			m_chunks.push_back( chunck );
			assert( _outecsComponentIndex.chunckIndex == m_chunks.size() - 1 ); // index should be correct because of the previous loop
			return *chunck->Alloc( _outecsComponentIndex.componentIndex );
		}

		//================================================================
		void Delete( const ecsComponentIndex& _componentIndex )
		{
			assert( _componentIndex.chunckIndex < m_chunks.size() );
			m_chunks[_componentIndex.chunckIndex]->Delete( _componentIndex.componentIndex );
		}

	private:
		std::vector< Chunck<_type> * > m_chunks;	// List of chuncks
	};

	namespace impl {
		template < size_t _index, typename _type >
		struct	IndexedComponentData {
			ComponentData<_type> data;
		};

		//================================================================================================================================
		// ComponentElement
		// Defines data for a specific _type at an _index
		// _count is the number of ComponentElement of the ComponentsTuple
		//================================================================================================================================
		template < size_t _index, typename _type >
		struct	ComponentElement {
			IndexedComponentData<_index, _type > indexedData;
		};

		//================================================================================================================================
		// ComponentsTupleImpl
		// Inherits multiples ComponentElement of differents types
		// Get() gives access to differents elements of the ComponentsTuple
		//================================================================================================================================
		template < typename... _types >	struct ComponentsTupleImpl;	
		template < template< size_t...> typename _SizeList, size_t... _sizes, typename... _types  >
		struct ComponentsTupleImpl< _SizeList<_sizes...>, _types... > : ComponentElement<_sizes, _types >...
		{		
		private:
			// returns the index of the corresponding _type
			template < typename _type >	using indexElement = typename meta::Find::Type< _type,  _types... >;
			template < size_t _index >	using elementIndex = typename meta::Extract::Type<_index, _types... >::value;

		public:
			// Returns the ComponentData of the corresponding _type
			template < typename _type >
			ComponentData< _type> & Get() {
 				return  ComponentElement< indexElement<_type>::value, _type >::indexedData.data;
 			}
			// Const version
			template < typename _type >
			const ComponentData< _type> & Get() const {
				return  ComponentElement< indexElement<_type>::value, _type >::indexedData.data;
			}

			// Allocates a component
			template < typename _type >
			_type& Alloc( ecsComponentIndex& _outecsComponentIndex )
			{
				ComponentData< _type> & data = Get<_type>();
				return data.Alloc( _outecsComponentIndex );
			}

			template < size_t _index >
			ComponentData< elementIndex<_index> > & Get() {
				return  ComponentElement< _index, elementIndex<_index> >::indexedData.data;
			}

		};
	}

	//================================================================================================================================
	// ComponentsTuple
	// Generates a vectors & bitset for every type passed in _types
	//================================================================================================================================
	template< typename..._types > class ecsComponentsTuple;
	
	// Tuple with a TypeList argument
	template< template < typename... > typename TypeList, typename... _types >
	class ecsComponentsTuple<TypeList<_types...> >  : public impl::ComponentsTupleImpl < typename meta::Range< sizeof...( _types ) >::type, _types... >
	{
	public:
		static constexpr size_t size = sizeof...( _types );		// Number of types in the tuple
	};
}

