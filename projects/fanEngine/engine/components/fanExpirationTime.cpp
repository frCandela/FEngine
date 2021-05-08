#include "engine/components/fanExpirationTime.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ExpirationTime::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &ExpirationTime::Load;
		_info.save        = &ExpirationTime::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void ExpirationTime::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ExpirationTime& expiration = static_cast<ExpirationTime&>( _component );
		expiration.mDuration = 10.f;
	}

	//========================================================================================================
	//========================================================================================================
	void ExpirationTime::Save( const EcsComponent& _component, Json& _json )
	{
		const ExpirationTime& expiration = static_cast<const ExpirationTime&>( _component );

		Serializable::SaveFloat( _json, "duration", expiration.mDuration );
	}

	//========================================================================================================
	//========================================================================================================
	void ExpirationTime::Load( EcsComponent& _component, const Json& _json )
	{
		ExpirationTime& expiration = static_cast<ExpirationTime&>( _component );
		Serializable::LoadFloat( _json, "duration", expiration.mDuration );
	}
}