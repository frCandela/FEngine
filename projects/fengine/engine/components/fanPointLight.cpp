#include "engine/components/fanPointLight.hpp"
#include "LinearMath/btVector3.h"
#include "core/memory/fanSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void PointLight::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &PointLight::Load;
		_info.save        = &PointLight::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void PointLight::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
		pointLight.mAmbiant  = Color::sWhite;
		pointLight.mDiffuse  = Color::sWhite;
		pointLight.mSpecular = Color::sWhite;
		pointLight.mAttenuation[0] = 1.0f;
		pointLight.mAttenuation[1] = 0.5f;
		pointLight.mAttenuation[2] = 0.2f;
	}

    //========================================================================================================
    //========================================================================================================
    float PointLight::GetLightRange( const PointLight& _light )
    {
        const float epsilon = 0.1f;	// Value at which we consider the light value null
        const float q = _light.mAttenuation[2];
        const float s = _light.mAttenuation[0];
        const float L = _light.mAttenuation[1];
        if ( q < epsilon )
        {	// first order linear differential equation
            return ( ( 1.f / epsilon ) - s ) / L;
        }
        else
        { // Second order linear differential equation
            float delta = L * L - 4 * q * ( s - 1 / epsilon );
            float sqrtDelta = std::sqrt( delta );
            return ( -L + sqrtDelta ) / ( 2 * q );
        }
    }

	//========================================================================================================
	//========================================================================================================
	void PointLight::Save( const EcsComponent& _component, Json& _json )
	{
		const PointLight& pointLight = static_cast<const PointLight&>( _component );
		Serializable::SaveColor( _json, "ambiant", pointLight.mAmbiant );
		Serializable::SaveColor( _json, "diffuse", pointLight.mDiffuse );
		Serializable::SaveColor( _json, "specular", pointLight.mSpecular );
        Serializable::SaveVec3( _json,
                                "attenuation",
                                btVector3( pointLight.mAttenuation[0],
                                           pointLight.mAttenuation[1],
                                           pointLight.mAttenuation[2] ) );
	}

	//========================================================================================================
	//========================================================================================================
	void PointLight::Load( EcsComponent& _component, const Json& _json )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
		Serializable::LoadColor( _json, "ambiant", pointLight.mAmbiant );
		Serializable::LoadColor( _json, "diffuse", pointLight.mDiffuse );
		Serializable::LoadColor( _json, "specular", pointLight.mSpecular );

		btVector3 tmp;
		Serializable::LoadVec3( _json, "attenuation", tmp );
		pointLight.mAttenuation[0] = tmp[0];
		pointLight.mAttenuation[1] = tmp[1];
		pointLight.mAttenuation[2] = tmp[2];
	}
}