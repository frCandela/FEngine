#include "ecs/fanEcsSystem.hpp"
#include "engine/components/fanScale.hpp"
#include "game/components/fanAnimScale.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateAnimScale : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<AnimScale>() | _world.GetSignature<Scale>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, Fixed _delta )
        {
            auto scaleIt     = _view.begin<Scale>();
            auto animScaleIt = _view.begin<AnimScale>();
            for( ; scaleIt != _view.end<Scale>(); ++scaleIt, ++animScaleIt )
            {
                Scale    & scale     = *scaleIt;
                AnimScale& animScale = *animScaleIt;
                const Vector3 delta = _delta * ( animScale.mTargetScale - animScale.mSourceScale ) / animScale.mDuration;
                scale.mScale += delta;
                animScale.mTimeAccumulator += _delta;
                if( animScale.mTimeAccumulator >= animScale.mDuration )
                {
                    scale.mScale = animScale.mTargetScale;
                    _world.RemoveComponent<AnimScale>( scaleIt.GetEntity() );
                }
            }
        }
    };
}