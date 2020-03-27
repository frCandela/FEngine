#include "scene/components/fanExpirationTime.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( ExpirationTime, "expiration_time" );

	//================================================================================================================================
	//================================================================================================================================
	void ExpirationTime::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::STOP16;
		_info.onGui = &ExpirationTime::OnGui;
		_info.init = &ExpirationTime::Init;
		_info.load  = &ExpirationTime::Load;
		_info.save  = &ExpirationTime::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ExpirationTime::Init( Component& _component )
	{
		ExpirationTime& expiration = static_cast<ExpirationTime&>( _component );
		expiration.duration = 10.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ExpirationTime::OnGui( Component& _component )
	{
		ExpirationTime& expiration = static_cast<ExpirationTime&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "duration##ExpirationTime", &expiration.duration, 0.1f, 0.f, 10.f );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ExpirationTime::Save( const Component& _component, Json& _json )
	{
		const ExpirationTime& expiration = static_cast<const ExpirationTime&>( _component );

		Serializable::SaveFloat( _json, "duration", expiration.duration );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ExpirationTime::Load( Component& _component, const Json& _json )
	{
		ExpirationTime& expiration = static_cast<ExpirationTime&>( _component );
		Serializable::LoadFloat( _json, "duration", expiration.duration );
	}
}