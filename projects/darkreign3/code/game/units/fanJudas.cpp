#include "game/units/fanJudas.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/components/fanHealth.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::SetInfo( EcsComponentInfo& _info )
    {
        _info.load     = &Judas::Load;
        _info.save     = &Judas::Save;
        _info.postInit = &Judas::PostInit;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Judas& judas = static_cast<Judas&>( _component );
        judas.mAnimIdle = nullptr;
        judas.mAnimWalk = nullptr;
        judas.mAnimRun  = nullptr;
        judas.mAnimFire = nullptr;
        judas.mFireFx = nullptr;
        judas.mDeathFx = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::PostInit( EcsWorld& _world, EcsEntity _entity )
    {
        Unit* unit = _world.SafeGetComponent<Unit>( _entity );
        if( unit != nullptr )
        {
            unit->mDeathDelegate = &Judas::OnDeath;
            unit->mFireDelegate  = &Judas::OnFire;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::OnFire( EcsWorld& _world, EcsEntity _unitEntity )
    {
        const Unit  & unit   = _world.GetComponent<Unit>( _unitEntity );
        const Weapon& weapon = _world.GetComponent<Weapon>( _unitEntity );
        Health      * health = _world.SafeGetComponent<Health>( _world.GetEntity( unit.GetTarget() ) );
        if( health != nullptr )
        {
            health->mHealth -= weapon.mDamage;
        }

        const Judas& judas = _world.GetComponent<Judas>( _unitEntity );
        if( judas.mFireFx != nullptr )
        {
            Transform& unitTransform  = _world.GetComponent<Transform>( _unitEntity );
            Transform& enemyTransform = _world.GetComponent<Transform>( _world.GetEntity( unit.GetTarget() ) );
            const Vector3 unitForward = unitTransform.Forward();
            const Vector3 unitRight   = unitTransform.Right();
            const Vector3 offsetLeft  = FIXED( 3.6 ) * unitForward - FIXED( 0.6 ) * unitRight + FIXED( 2.8 ) * Vector3::sUp;
            const Vector3 offsetRight = FIXED( 2.5 ) * unitForward + FIXED( 1.5 ) * unitRight + FIXED( 2.8 ) * Vector3::sUp;
            Weapon::FireLaser( _world, *judas.mFireFx, unitTransform.mPosition + offsetLeft, enemyTransform.mPosition + FIXED( 2.8 ) * Vector3::sUp );
            Weapon::FireLaser( _world, *judas.mFireFx, unitTransform.mPosition + offsetRight, enemyTransform.mPosition + FIXED( 2.8 ) * Vector3::sUp );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::OnDeath( EcsWorld& _world, EcsEntity _unitEntity )
    {
        const Judas& judas = _world.GetComponent<Judas>( _unitEntity );

        if( judas.mDeathFx != nullptr )
        {
            Scene    & scene     = _world.GetSingleton<Scene>();
            SceneNode* sceneNode = judas.mDeathFx->Instantiate( scene.GetRootNode() );
            Transform& transform = _world.GetComponent<Transform>( _world.GetEntity( sceneNode->mHandle ) );
            transform.mPosition = _world.GetComponent<Transform>( _unitEntity ).mPosition;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::Save( const EcsComponent& _component, Json& _json )
    {
        const Judas& judas = static_cast<const Judas&>( _component );
        Serializable::SaveResourcePtr( _json, "anim_idle", judas.mAnimIdle );
        Serializable::SaveResourcePtr( _json, "anim_walk", judas.mAnimWalk );
        Serializable::SaveResourcePtr( _json, "anim_run", judas.mAnimRun );
        Serializable::SaveResourcePtr( _json, "anim_fire", judas.mAnimFire );
        Serializable::SaveResourcePtr( _json, "fire_fx", judas.mFireFx );
        Serializable::SaveResourcePtr( _json, "death_fx", judas.mDeathFx );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::Load( EcsComponent& _component, const Json& _json )
    {
        Judas& judas = static_cast<Judas&>( _component );
        Serializable::LoadResourcePtr( _json, "anim_idle", judas.mAnimIdle );
        Serializable::LoadResourcePtr( _json, "anim_walk", judas.mAnimWalk );
        Serializable::LoadResourcePtr( _json, "anim_run", judas.mAnimRun );
        Serializable::LoadResourcePtr( _json, "anim_fire", judas.mAnimFire );
        Serializable::LoadResourcePtr( _json, "fire_fx", judas.mFireFx );
        Serializable::LoadResourcePtr( _json, "death_fx", judas.mDeathFx );
    }
}