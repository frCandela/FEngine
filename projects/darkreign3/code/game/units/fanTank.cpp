#include "game/units/fanTank.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/components/fanHealth.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Tank::SetInfo( EcsComponentInfo& _info )
    {
        _info.load     = &Tank::Load;
        _info.save     = &Tank::Save;
        _info.postInit = &Tank::PostInit;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Tank::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Tank& tank = static_cast<Tank&>( _component );
        tank.mFireFx = nullptr;
        tank.mDeathFx = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Tank::PostInit( EcsWorld& _world, EcsEntity _entity )
    {
        Unit* unit = _world.SafeGetComponent<Unit>( _entity );
        if( unit != nullptr )
        {
            unit->mDeathDelegate = &Tank::OnDeath;
            unit->mFireDelegate  = &Tank::OnFire;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Tank::OnFire( EcsWorld& _world, EcsEntity _unitEntity )
    {
        const Unit  & unit   = _world.GetComponent<Unit>( _unitEntity );
        const Weapon& weapon = _world.GetComponent<Weapon>( _unitEntity );
        Health      * health = _world.SafeGetComponent<Health>( _world.GetEntity( unit.GetTarget() ) );
        if( health != nullptr )
        {
            health->mHealth -= weapon.mDamage;
        }

        const Tank& tank = _world.GetComponent<Tank>( _unitEntity );
        if( tank.mFireFx != nullptr )
        {
            Transform& unitTransform = _world.GetComponent<Transform>( _unitEntity );
            Transform& enemyTransform = _world.GetComponent<Transform>( _world.GetEntity( unit.GetTarget() ) );
            const Vector3 unitForward = unitTransform.Forward();
            const Vector3 unitRight   = unitTransform.Right();
            const Vector3 offsetLeft  = FIXED( 3.3 ) * unitForward - FIXED( 1.5 ) * unitRight + FIXED( 4.6 ) * Vector3::sUp;
            const Vector3 offsetRight = FIXED( 3.3 ) * unitForward + FIXED( 1.5 ) * unitRight + FIXED( 4.6 ) * Vector3::sUp;
            Weapon::FireLaser( _world, *tank.mFireFx,unitTransform.mPosition + offsetLeft, enemyTransform.mPosition + FIXED( 2.8 ) * Vector3::sUp );
            Weapon::FireLaser( _world, *tank.mFireFx,unitTransform.mPosition + offsetRight, enemyTransform.mPosition + FIXED( 2.8 ) * Vector3::sUp );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Tank::OnDeath( EcsWorld& _world, EcsEntity _unitEntity )
    {
        const Tank& tank = _world.GetComponent<Tank>( _unitEntity );

        if( tank.mDeathFx != nullptr )
        {
            Scene    & scene     = _world.GetSingleton<Scene>();
            SceneNode* sceneNode = tank.mDeathFx->Instantiate( scene.GetRootNode() );
            Transform& transform = _world.GetComponent<Transform>( _world.GetEntity( sceneNode->mHandle ) );
            transform.mPosition = _world.GetComponent<Transform>( _unitEntity ).mPosition;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Tank::Save( const EcsComponent& _component, Json& _json )
    {
        const Tank& tank = static_cast<const Tank&>( _component );
        Serializable::SaveResourcePtr( _json, "fire_fx", tank.mFireFx );
        Serializable::SaveResourcePtr( _json, "death_fx", tank.mDeathFx );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Tank::Load( EcsComponent& _component, const Json& _json )
    {
        Tank& tank = static_cast<Tank&>( _component );
        Serializable::LoadResourcePtr( _json, "fire_fx", tank.mFireFx );
        Serializable::LoadResourcePtr( _json, "death_fx", tank.mDeathFx );
    }
}