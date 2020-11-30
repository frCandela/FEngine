#include "project_spaceships/components/fanDamage.hpp"

#include "engine/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Damage::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &Damage::Load;
		_info.save        = &Damage::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void Damage::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Damage& damage = static_cast<Damage&>( _component );
		damage.mDamage = 1.f;
	}

	//========================================================================================================
	//========================================================================================================
	void Damage::Save( const EcsComponent& _component, Json& _json )
	{
		const Damage& damage = static_cast<const Damage&>( _component );
		Serializable::SaveFloat( _json, "damage", damage.mDamage );
	}

	//========================================================================================================
	//========================================================================================================
	void Damage::Load( EcsComponent& _component, const Json& _json )
	{
		Damage& damage = static_cast<Damage&>( _component );
		Serializable::LoadFloat( _json, "damage", damage.mDamage );
	}
}