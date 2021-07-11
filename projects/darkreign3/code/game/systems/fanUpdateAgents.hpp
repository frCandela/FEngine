#include "ecs/fanEcsSystem.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/systems/fanRaycast.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/components/fanAnimator.hpp"
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

        static void Run( EcsWorld& _world, const EcsView& _view, const Fixed& _delta )
        {
            if( _delta == 0 )
            {
                return;
            }

            RenderDebug& rd = _world.GetSingleton<RenderDebug>();

            auto sceneNodeIt    = _view.begin<SceneNode>();
            auto transformIt    = _view.begin<Transform>();
            auto terrainAgentIt = _view.begin<TerrainAgent>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++terrainAgentIt, ++sceneNodeIt )
            {
                Transform   & transform = *transformIt;
                TerrainAgent& agent     = *terrainAgentIt;
                switch( agent.mState )
                {
                    case TerrainAgent::Stay:
                        break;
                    case TerrainAgent::Move:
                        UpdateTerrainInformation( _world, agent, transform );
                        if( MoveAgent( agent, transform, agent.mMaxMoveSpeed, _delta ) )
                        {
                            _world.AddTag<TagUnitStateNeedsUpdate>( transformIt.GetEntity() );
                        }
                        rd.DrawLine( agent.mPositionOnTerrain, agent.mTargetPosition, Color::sCyan );
                        break;
                    case TerrainAgent::Face:
                        MoveAgent( agent, transform, 0, _delta );
                        break;
                    default:
                        fanAssert( false );
                        break;
                }

                SceneNode& sceneNode = *sceneNodeIt;
                sceneNode.AddFlag( SceneNode::BoundsOutdated );
            }
        }

        static void UpdateTerrainInformation( EcsWorld& _world, TerrainAgent& _agent, Transform& _transform )
        {
            std::vector<SRaycast::Result> results;
            Ray                           ray( _transform.mPosition + 4 * Vector3::sUp, Vector3::sDown );
            Raycast<TagTerrain>( _world, ray, results );
            if( !results.empty() )
            {
                _agent.mTerrainNormal     = results[0].mData.mNormal;
                _agent.mPositionOnTerrain = results[0].mData.mPosition;
            }
        }

        //==================================================================================================================================================================================================
        // returns true when the target is reached
        //==================================================================================================================================================================================================
        static bool MoveAgent( TerrainAgent& _agent, Transform& _transform, const Fixed _moveSpeed, const Fixed& _delta )
        {
            _transform.mPosition = _agent.mPositionOnTerrain + Vector3( 0, _agent.mHeightOffset, 0 );

            // slowly rotates the transform up vector towards the terrain normal
            Vector3 up = _transform.Up();
            if( _agent.mAlignWithTerrain )
            {
                Fixed angleNormal = Vector3::Angle( up, _agent.mTerrainNormal );
                if( angleNormal > 3 )
                {
                    const Vector3 axis       = Vector3::Cross( up, _agent.mTerrainNormal );
                    const Fixed   angleDelta = 90 * _delta;
                    up = Quaternion::AngleAxis( angleDelta, axis ) * up;
                    up.Normalize();
                }
            }
            else
            {
                up = Vector3::sUp;
            }

            // slowly rotates the forwards vector toward the target position
            const Vector3 moveDirection = ( _agent.mTargetPosition - _transform.mPosition ).Normalized();
            Vector3       forward       = _transform.Forward();
            const Vector3 targetForward = ( moveDirection - Vector3::Dot( moveDirection, up ) * up ).Normalized();
            _agent.mForwardAngle = Vector3::SignedAngle( forward, targetForward, up );
            if( Fixed::Abs( _agent.mForwardAngle ) > 5 )
            {
                const Fixed angleDelta = Fixed::Sign( _agent.mForwardAngle ) * _agent.mMaxRotationSpeed * _delta;
                forward = Quaternion::AngleAxis( angleDelta, up ) * forward;
            }

            // apply new orientation
            const Vector3 left       = Vector3::Cross( up, forward );
            const Vector3 newForward = Vector3::Cross( left, up );
            _transform.mRotation = Matrix3( left, up, newForward ).ToQuaternion().Normalized();

            // change position
            if( Fixed::Abs( _agent.mForwardAngle ) < 30 )
            {
                _transform.mPosition += _delta * moveDirection * _moveSpeed;
            }

            // stops when reaching the target
            if( _agent.TargetIsInRange() )// _agent.mSqrDistanceFromDestination < ( _agent.mRange - FIXED( 1 ) ) * ( _agent.mRange - FIXED( 1 ) ) )
            {
                return true;
            }

            return false;
        }
    };
}