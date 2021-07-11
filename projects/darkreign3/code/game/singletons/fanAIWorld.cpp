#include "game/singletons/fanAIWorld.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/fanDebug.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/physics/fanTransform.hpp"
#include "game/fanDR3Tags.hpp"
#include "game/components/fanUnit.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AIWorld::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &AIWorld::Save;
        _info.load = &AIWorld::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AIWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        AIWorld& aiWorld = static_cast<AIWorld&>( _singleton );
        aiWorld.mEnemies.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AIWorld::UpdateEnemiesLists( EcsWorld& _world )
    {
        AIWorld& aiWorld = _world.GetSingleton<AIWorld>();
        aiWorld.mEnemies.clear();
        _world.RunLambda<Transform, Unit, TagEnemy>(
                [&]( EcsWorld& _world, const EcsView& _view )
                {
                    auto transformIt = _view.begin<Transform>();
                    for( ; transformIt != _view.end<Transform>(); ++transformIt )
                    {
                        Transform& transform = *transformIt;
                        AIWorld::EnemyInfo info = { _world.GetHandle( transformIt.GetEntity() ), &transform };
                        aiWorld.mEnemies.push_back( info );
                    }
                }
        );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsHandle AIWorld::FindEnemyInRange( const Vector3& _position, const Fixed _range )
    {
        const Fixed sqrRange        = _range * _range;
        EcsHandle   mEnemyHandle    = 0;
        Fixed       mMinSqrDistance = Fixed::sMaxValue;
        for( int    i               = 0; i < (int)mEnemies.size(); i++ )
        {
            const EnemyInfo& enemy = mEnemies[i];
            Fixed sqrDistance = Vector3::SqrDistance( _position, enemy.mTransform->mPosition );
            if( sqrDistance < sqrRange && sqrDistance < mMinSqrDistance )
            {
                mMinSqrDistance = sqrDistance;
                mEnemyHandle    = enemy.mHandle;
            }
        }
        return mEnemyHandle;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AIWorld::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const AIWorld& aiWorld = static_cast<const AIWorld&>( _singleton );
        (void)aiWorld;
        (void)_json;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AIWorld::Load( EcsSingleton& _singleton, const Json& _json )
    {
        AIWorld& aiWorld = static_cast<AIWorld&>( _singleton );
        (void)aiWorld;
        (void)_json;
    }
}