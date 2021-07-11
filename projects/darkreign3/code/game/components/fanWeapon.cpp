#include "game/components/fanWeapon.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanScale.hpp"
#include "engine/components/fanSceneNode.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Weapon::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &Weapon::Load;
        _info.save = &Weapon::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Weapon::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Weapon& weapon = static_cast<Weapon&>( _component );
        weapon.mDamage          = 20;
        weapon.mRateOfFire      = 1;
        weapon.mTimeAccumulator = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Weapon::FireLaser( EcsWorld& _world, Prefab& _prefab, const Vector3& _origin, const Vector3& _target )
    {
        Scene& scene = _world.GetSingleton<Scene>();
        EcsEntity fxEntity = _world.GetEntity( _prefab.Instantiate( scene.GetRootNode() )->mHandle );
        Transform& fxTransform = _world.GetComponent<Transform>( fxEntity );

        const Vector3 originToTarget = _target - _origin;
        const Vector3 middlePosition = _origin + FIXED( 0.5 ) * originToTarget;

        fxTransform.mPosition = middlePosition;
        fxTransform.mRotation = Quaternion::LookRotation( originToTarget );

        Scale& scale = _world.GetComponent<Scale>( fxEntity );
        scale.mScale.z = originToTarget.Magnitude();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Weapon::Save( const EcsComponent& _component, Json& _json )
    {
        const Weapon& weapon = static_cast<const Weapon&>( _component );
        Serializable::SaveFixed( _json, "damage", weapon.mDamage );
        Serializable::SaveFixed( _json, "fire_rate", weapon.mRateOfFire );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Weapon::Load( EcsComponent& _component, const Json& _json )
    {
        Weapon& weapon = static_cast<Weapon&>( _component );
        Serializable::LoadFixed( _json, "damage", weapon.mDamage );
        Serializable::LoadFixed( _json, "fire_rate", weapon.mRateOfFire );
    }
}