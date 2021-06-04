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

                if( agent.mState == TerrainAgent::State::Move )
                {

                    // raycast on the terrain and place the agent on it
                    Ray ray( transform.mPosition + 4 * Vector3::sUp, Vector3::sDown );
                    Raycast<TagTerrain>( _world, ray, results );
                    if( !results.empty() )
                    {
                        agent.mTerrainNormal = results[0].mData.mNormal;
                        transform.mPosition  = results[0].mData.mPosition + Vector3( 0, agent.mHeightOffset, 0 );
                    }


                    // slowly rotates the transform up vector towards the terrain normal
                    Vector3 up          = transform.Up();
                    Fixed   angleNormal = Vector3::Angle( up, agent.mTerrainNormal );
                    if( angleNormal > 3 )
                    {
                        const Vector3 axis       = Vector3::Cross( up, agent.mTerrainNormal );
                        const Fixed   angleDelta = 90 * _delta;
                        up = Quaternion::AngleAxis( angleDelta, axis ) * up;
                        up.Normalize();
                    }

                    const Vector3 moveDirection = ( agent.mDestination - transform.mPosition ).Normalized();

                    // slowly rotates the forwards vector toward the target position
                    Vector3       forward       = transform.Forward();
                    const Vector3 targetForward = ( moveDirection - Vector3::Dot( moveDirection, up ) * up ).Normalized();
                    const Fixed   forwardAngle  = Vector3::SignedAngle( forward, targetForward, up );
                    if( Fixed::Abs( forwardAngle ) > 5 )
                    {
                        const Fixed angleDelta = Fixed::Sign( forwardAngle ) * agent.mRotationSpeed * _delta;
                        forward = Quaternion::AngleAxis( angleDelta, up ) * forward;
                    }

                    // apply new orientation
                    const Vector3 left       = Vector3::Cross( up, forward );
                    const Vector3 newForward = Vector3::Cross( left, up );
                    transform.mRotation = Matrix3( left, up, newForward ).ToQuaternion().Normalized();
                    if( Fixed::Abs( forwardAngle ) < 30 )
                    {
                        transform.mPosition += _delta * moveDirection * agent.mMoveSpeed;
                    }

                    // stops when reaching the target
                    if( Vector3::SqrDistance( transform.mPosition, agent.mDestination ) < 1 )
                    {
                        agent.mState = TerrainAgent::State::Stay;
                    }
                }

                SceneNode& sceneNode = *sceneNodeIt;
                sceneNode.AddFlag( SceneNode::BoundsOutdated );
            }
        }
    };
}