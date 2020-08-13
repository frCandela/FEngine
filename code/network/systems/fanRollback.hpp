#include "ecs/fanEcsSystem.hpp"
#include "network/components/fanClientRollback.hpp"
#include "network/singletons/fanTime.hpp"
#include "game/singletons/fanClientNetworkManager.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// iterates over all ClientRollback components to save the state of the entity for the current frame
	// EcsComponentInfo can contain rollback load/save methods for serializing rollback state, we need to iterate over all components to call them
	//==============================================================================================================================================================
	struct S_RollbackStateSave : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientRollback>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

			auto clientRollbackIt = _view.begin<ClientRollback>();
			for( ; clientRollbackIt != _view.end<ClientRollback>(); ++clientRollbackIt )
			{			
				ClientRollback& clientRollback = *clientRollbackIt;
				const EcsEntity entity = clientRollbackIt.GetEntity();
				const EcsSignature& signature = entity.archetype->GetSignature();

				// iterates over all components and saves rollback state
				for (int i = 0; i < _world.NumComponents(); i++)
				{
					if( signature[i] ) // has component
					{
						const EcsComponentInfo& componentInfo = _world.IndexedGetComponentInfo( i );
						if( componentInfo.rollbackSave != nullptr )
						{
							ClientRollback::RollbackData rollbackData;
							rollbackData.frameIndex = time.frameIndex;
							rollbackData.componentIndex = i;

							// save rollback state
							const EcsComponent&  component = _world.IndexedGetComponent( entity, i );
							componentInfo.rollbackSave( component, rollbackData.data );
							assert( rollbackData.data.getDataSize() > 0 );
							clientRollback.rollbackDatas.push_back( rollbackData );							
						}
					}
				}
			}
		}
	};

	//==============================================================================================================================================================
	// iterates over all ClientRollback components to remove rollback states older than the current server state frame index
	//==============================================================================================================================================================
	struct S_RollbackRemoveOldStates : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientRollback>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
		    if( _delta == 0 ){ return; }

			// get the last server state frame index
			const ClientNetworkManager& netManager = _world.GetSingleton<ClientNetworkManager>();
			if( netManager.persistentHandle == 0 ){ return; }
			const EcsEntity entity = _world.GetEntity( netManager.persistentHandle );
			const ClientGameData& clientData = _world.GetComponent<ClientGameData>( entity );
			const FrameIndex lastFrameIndex = clientData.lastServerState.frameIndex;

			auto clientRollbackIt = _view.begin<ClientRollback>();
			for( ; clientRollbackIt != _view.end<ClientRollback>(); ++clientRollbackIt )
			{
				ClientRollback& clientRollback = *clientRollbackIt;

				// remove older game states in the rollback components 
				while( !clientRollback.rollbackDatas.empty() 
					&&  ( clientRollback.rollbackDatas.front().frameIndex < lastFrameIndex || clientRollback.rollbackDatas.size() > ClientRollback::s_rollbackDatasMaxSize ))
				{
					clientRollback.rollbackDatas.pop_front();
				}
			}
		}
	};

	//==============================================================================================================================================================
	// iterates over all ClientRollback components to restore entity state at a specific frame index
	//==============================================================================================================================================================
	struct S_RollbackRestoreState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientRollback>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const FrameIndex _frameIndex )
		{
			auto clientRollbackIt = _view.begin<ClientRollback>();
			for( ; clientRollbackIt != _view.end<ClientRollback>(); ++clientRollbackIt )
			{				
				ClientRollback& clientRollback = *clientRollbackIt;
				const EcsEntity entity = clientRollbackIt.GetEntity();
				const EcsSignature& signature = entity.archetype->GetSignature();

				// Iterates over all rollback Datas and load the ones with the correct frame index
				for ( const ClientRollback::RollbackData& rollbackData : clientRollback.rollbackDatas)
				{
					if( rollbackData.frameIndex == _frameIndex && signature[rollbackData.componentIndex] )
					{
						const EcsComponentInfo& componentInfo = _world.IndexedGetComponentInfo( rollbackData.componentIndex );
						assert( componentInfo.rollbackLoad != nullptr );

						EcsComponent& component = _world.IndexedGetComponent( entity, rollbackData.componentIndex );
						sf::Packet dataCpy = rollbackData.data;
						componentInfo.rollbackLoad( component, dataCpy );												
					}
				}
			}
		}
	};

	//==============================================================================================================================================================
	// Initialize rollback data :
	// only keep the first frame of all entity rollback data if the RollbackNoOverwrite is not set
	// For now RollbackNoOverwrite is used only for player input,
	// that way the player input is restored every frame and all other data (position, speed etc ) is overridden with new state except for the first one
	//==============================================================================================================================================================
	struct S_RollbackInit : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientRollback>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			std::vector<bool> firstStateFound( _world.NumComponents() );

			auto clientRollbackIt = _view.begin<ClientRollback>();
			for( ; clientRollbackIt != _view.end<ClientRollback>(); ++clientRollbackIt )
			{
				ClientRollback& clientRollback = *clientRollbackIt;
				std::fill( firstStateFound.begin(), firstStateFound.end(), false );

				// iterate over all components and only keep the first rollback state when RollbackNoOverwrite is not set				
				for (int i = 0; i < (int)clientRollback.rollbackDatas.size();)
				{
					const ClientRollback::RollbackData& rollbackData = clientRollback.rollbackDatas[i];
					const EcsComponentInfo& componentInfo = _world.IndexedGetComponentInfo( rollbackData.componentIndex );
					if( ( componentInfo.flags & EcsComponentInfo::RollbackNoOverwrite ) == 0 )
					{
						if( firstStateFound[rollbackData.componentIndex] )
						{
							clientRollback.rollbackDatas.erase( clientRollback.rollbackDatas.begin() + i );
							// do not increment i, we already advanced to the next element by erasing the current element.
						}
						else
						{
							firstStateFound[rollbackData.componentIndex] = true;
							++i;
						}
					}
					else
					{
						++i;
					}
				}
			}
		}
	};
}