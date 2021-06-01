#include "core/ecs/fanEcsSystem.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/systems/fanRaycast.hpp"
#include "game/components/fanTerrainAgent.hpp"
#include "game/fanDR3Tags.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SMoveAgents : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<TerrainAgent>() | _world.GetSignature<Transform>() | _world.GetSignature<SceneNode>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const Fixed& )
        {
            std::vector<SRaycast::Result> results;
            auto                          sceneNodeIt    = _view.begin<SceneNode>();
            auto                          transformIt    = _view.begin<Transform>();
            auto                          terrainAgentIt = _view.begin<TerrainAgent>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++terrainAgentIt, ++sceneNodeIt )
            {
                Transform   & transform    = *transformIt;
                TerrainAgent& terrainAgent = *terrainAgentIt;

                Ray ray( transform.mPosition + 4 * Vector3::sUp, Vector3::sDown );
                Raycast<TagTerrain>( _world, ray, results );
                if( !results.empty() )
                {
                    transform.mPosition = results[0].mData.mPosition + Vector3( 0, terrainAgent.mHeightOffset, 0 );

                    const Vector3 up = results[0].mData.mNormal.Normalized();
                    const Vector3 left = Vector3::Cross( up, transform.Forward() );
                    const Vector3 forward = Vector3::Cross( left, up );
                    const Quaternion targetRotation = Matrix3(left, up, forward).ToQuaternion();
                    if( Quaternion::AngleBetween(transform.mRotation, targetRotation) > Fixed::Radians(5))
                    {
                        transform.mRotation = Quaternion::Slerp( transform.mRotation, targetRotation, FIXED(0.2));

                        SceneNode   & sceneNode    = *sceneNodeIt;
                        sceneNode.AddFlag( SceneNode::BoundsOutdated );
                    }
                }
            }
        }
    };
}