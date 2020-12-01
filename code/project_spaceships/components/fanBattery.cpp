#include "project_spaceships/components/fanBattery.hpp"

#include "engine/fanEngineSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Battery::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &Battery::Load;
		_info.save        = &Battery::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void Battery::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Battery& battery = static_cast<Battery&>( _component );
		battery.mMaxEnergy     = 100.f;
		battery.mCurrentEnergy = battery.mMaxEnergy;
	}

	//========================================================================================================
	//========================================================================================================
	void Battery::Save( const EcsComponent& _component, Json& _json )
	{
		const Battery& battery = static_cast<const Battery&>( _component );

		Serializable::SaveFloat( _json, "max_energy", battery.mMaxEnergy );
	}

	//========================================================================================================
	//========================================================================================================
	void Battery::Load( EcsComponent& _component, const Json& _json )
	{
		Battery& battery = static_cast<Battery&>( _component );

		Serializable::LoadFloat( _json, "max_energy", battery.mMaxEnergy );
	}
}