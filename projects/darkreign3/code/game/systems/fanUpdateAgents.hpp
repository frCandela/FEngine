#include "core/ecs/fanEcsSystem.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/systems/fanRaycast.hpp"
#include "game/components/fanTerrainAgent.hpp"
#include "game/fanDR3Tags.hpp"
#include "engine/singletons/fanRenderDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SSetSelectedAgentsDestination : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<TerrainAgent>() | _world.GetSignature<TagSelected>();
        }

        static void Run( EcsWorld&, const EcsView& _view, const Vector3 _destination )
        {
            auto agentIt = _view.begin<TerrainAgent>();
            for( ; agentIt != _view.end<TerrainAgent>(); ++agentIt )
            {
                TerrainAgent& agent = *agentIt;
                agent.mDestination = _destination;
                agent.mState       = TerrainAgent::State::Move;
            }
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SMoveAgents : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<TerrainAgent>() | _world.GetSignature<Transform>() | _world.GetSignature<SceneNode>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const Fixed& _delta )
        {
            std::vector<SRaycast::Result> results;
            auto                          sceneNodeIt    = _view.begin<SceneNode>();
            auto                          transformIt    = _view.begin<Transform>();
            auto                          terrainAgentIt = _view.begin<TerrainAgent>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++terrainAgentIt, ++sceneNodeIt )
            {
                Transform   & transform = *transformIt;
                TerrainAgent& agent     = *terrainAgentIt;

                {
                    Ray ray( transform.mPosition + 4 * Vector3::sUp, Vector3::sDown );
                    Raycast<TagTerrain>( _world, ray, results );
                    if( !results.empty() )
                    {
                        agent.mTerrainNormal = ( FIXED( 0.7 ) * agent.mTerrainNormal + FIXED( 0.3 ) * results[0].mData.mNormal ).Normalized();
                        transform.mPosition = results[0].mData.mPosition + Vector3( 0, agent.mHeightOffset, 0 );
                    }
                }

                const Vector3 moveDirection = ( agent.mDestination - transform.mPosition ).Normalized();
                Vector3       forward       = transform.Forward();
                if( agent.mState == TerrainAgent::State::Move )
                {
                    const Vector3 up            = agent.mTerrainNormal;
                    const Vector3 targetForward = ( moveDirection - Vector3::Dot( moveDirection, up ) * up ).Normalized();
                    const Fixed   angle         = Vector3::SignedAngle( forward, targetForward, up );
                    if( Fixed::Abs( angle ) > 10 )
                    {
                        const Fixed angleDelta = Fixed::Sign( angle ) * agent.mRotationSpeed * _delta;
                        forward = Quaternion::AngleAxis( angleDelta, up ) * forward;
                    }

                    // align to terrain
                    const Vector3 left       = Vector3::Cross( up, forward );
                    const Vector3 newForward = Vector3::Cross( left, up );

                    transform.mRotation = Matrix3( left, up, newForward ).ToQuaternion().Normalized();
                    transform.mPosition += _delta * moveDirection * agent.mMoveSpeed;

                    if( Vector3::SqrDistance( transform.mPosition, agent.mDestination ) < 1 ){ agent.mState = TerrainAgent::State::Stay; }
                }
                SceneNode& sceneNode = *sceneNodeIt;
                sceneNode.AddFlag( SceneNode::BoundsOutdated );
            }
        }
    };
}