#include "game/systems/fanUpdateGameCamera.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanCamera.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/singletonComponents/fanGameCamera.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateGameCamera::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<SpaceShip>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateGameCamera::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }
		if( _view.Empty() ) { return; }

		// bounding box
		btVector3 low( 10000.f, 0.f, 10000.f );
		btVector3 high( -10000.f, 0.f, -10000.f );
		btVector3 center = btVector3::Zero();

		// calculates players center and bounding box	
		for( auto transformIt = _view.begin<Transform>(); transformIt != _view.end<Transform>(); ++transformIt )
		{
			const Transform& transform = *transformIt;
			const btVector3 position = transform.GetPosition();
			center += position;

			// bounding box
			low[0] = low[0] < position[0] ? low[0] : position[0];
			low[2] = low[2] < position[2] ? low[2] : position[2];
			high[0] = high[0] > position[0] ? high[0] : position[0];
			high[2] = high[2] > position[2] ? high[2] : position[2];
		}

		center /= (float)_view.Size();
		assert( low[0] <= high[0] && low[2] <= high[2] );

		// set main camera
		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		if( gameCamera.cameraNode != nullptr )
		{
			// set position
			const EcsEntity cameraID = _world.GetEntity( gameCamera.cameraNode->handle );
			Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
			cameraTransform.SetPosition( center + gameCamera.heightFromTarget * btVector3::Up() );

			// set size
			Camera& camera = _world.GetComponent<Camera>( cameraID );
			if( _view.Size() == 1 )
			{
				camera.orthoSize = 10.f;
			}
			else
			{
				const float requiredSizeX = 0.5f * ( 1.f + gameCamera.marginRatio[0] ) * ( high[0] - low[0] ) / camera.aspectRatio;
				const float requiredSizeZ = ( 1.f + gameCamera.marginRatio[1] ) * ( high[2] - low[2] ) / camera.aspectRatio;

				const float orthoSize = std::max( std::max( requiredSizeX, requiredSizeZ ), gameCamera.minOrthoSize );
				camera.orthoSize =  orthoSize ;
			}
		}


		
	}
}