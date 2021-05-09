#include "engine/components/fanDirectionalLight.hpp"
#include "core/memory/fanSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &DirectionalLight::Load;
		_info.save        = &DirectionalLight::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		DirectionalLight& light = static_cast<DirectionalLight&>( _component );
		light.mAmbiant  = Color( 0.1f, 0.1f, 0.1f );
		light.mDiffuse  = Color::sWhite;
		light.mSpecular = Color::sDarkGrey;
	}

	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::Save( const EcsComponent& _component, Json& _json )
	{
		const DirectionalLight& light = static_cast<const DirectionalLight&>( _component );
		Serializable::SaveColor( _json, "ambiant", light.mAmbiant );
		Serializable::SaveColor( _json, "diffuse", light.mDiffuse );
		Serializable::SaveColor( _json, "specular", light.mSpecular );
	}
	
	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::Load( EcsComponent& _component, const Json& _json )
	{
		DirectionalLight& light = static_cast<DirectionalLight&>( _component );
		Serializable::LoadColor( _json, "ambiant", light.mAmbiant );
		Serializable::LoadColor( _json, "diffuse", light.mDiffuse );
		Serializable::LoadColor( _json, "specular", light.mSpecular );
	}
}