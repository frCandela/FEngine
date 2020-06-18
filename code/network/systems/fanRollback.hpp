#include "ecs/fanEcsSystem.hpp"
#include "network/components/fanClientRollback.hpp"
#include "game/singletons/fanClientNetworkManager.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// iterates over all ClientRollback components to save the state of the entity of the current frame
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

			const Game& game = _world.GetSingleton<Game>();
			ClientRollback::RollbackState rollbackState;
			rollbackState.frameIndex = game.frameIndex;

			auto clientRollbackIt = _view.begin<ClientRollback>();
			for( ; clientRollbackIt != _view.end<ClientRollback>(); ++clientRollbackIt )
			{				
				const EcsEntity entity = clientRollbackIt.Entity();
				const EcsSignature& signature = entity.archetype->GetSignature();

				// iterates over all components and saves rollback state
				for (int i = 0; i < _world.NumComponents(); i++)
				{
					if( signature[i] ) // has component
					{
						const EcsComponentInfo& componentInfo = _world.IndexedGetComponentInfo( i );
						if( componentInfo.rollbackSave != nullptr )
						{
							const EcsComponent&  component = _world.IndexedGetComponent( entity, i );
							componentInfo.rollbackSave( component, rollbackState.data );
						}
					}
				}

				// add rollback state
				if( rollbackState.data.getDataSize() != 0 )
				{
					ClientRollback& clientRollback = *clientRollbackIt;
					clientRollback.previousStates.push_back( rollbackState );
				}
			}
		}
	};

	//==============================================================================================================================================================
	// iterates over all ClientRollback components to remove rollback states older than a frame index
	//==============================================================================================================================================================
	struct S_RollbackRemoveOldStates : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientRollback>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			const ClientNetworkManager& netManager = _world.GetSingleton<ClientNetworkManager>();
			const EcsEntity entity = _world.GetEntity( netManager.persistentHandle );
			const ClientGameData& clientData = _world.GetComponent<ClientGameData>( entity );
			const FrameIndex lastFrameIndex = clientData.lastServerState.frameIndex;

			auto clientRollbackIt = _view.begin<ClientRollback>();
			for( ; clientRollbackIt != _view.end<ClientRollback>(); ++clientRollbackIt )
			{
				ClientRollback& clientRollback = *clientRollbackIt;

				// get the corresponding game state for the client
				while( !clientRollback.previousStates.empty() 
					&&  ( clientRollback.previousStates.front().frameIndex < lastFrameIndex || clientRollback.previousStates.size() > 100 ))
				{
					clientRollback.previousStates.pop_front();
				}
			}
		}
	};
}