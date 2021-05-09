#include "engine/components/fanPointLight.hpp"
#include "core/math/fanVector3.hpp"
#include "core/memory/fanSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PointLight::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &PointLight::Load;
        _info.save = &PointLight::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PointLight::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        PointLight& pointLight = static_cast<PointLight&>( _component );
        pointLight.mAmbiant  = Color::sWhite;
        pointLight.mDiffuse  = Color::sWhite;
        pointLight.mSpecular = Color::sWhite;
        pointLight.mAttenuation[0] = 1;
        pointLight.mAttenuation[1] = FIXED( 0.5 );
        pointLight.mAttenuation[2] = FIXED( 0.2 );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Fixed PointLight::GetLightRange( const PointLight& _light )
    {
        const Fixed epsilon = FIXED( 0.1 );    // Value at which we consider the light value null
        const Fixed q       = _light.mAttenuation[2];
        const Fixed s       = _light.mAttenuation[0];
        const Fixed L       = _light.mAttenuation[1];
        if( q < epsilon )
        {    // first order linear differential equation
            return ( ( 1 / epsilon ) - s ) / L;
        }
        else
        { // Second order linear differential equation
            Fixed delta     = L * L - 4 * q * ( s - 1 / epsilon );
            Fixed sqrtDelta = Fixed::Sqrt( delta );
            return ( -L + sqrtDelta ) / ( 2 * q );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PointLight::Save( const EcsComponent& _component, Json& _json )
    {
        const PointLight& pointLight = static_cast<const PointLight&>( _component );
        Serializable::SaveColor( _json, "ambiant", pointLight.mAmbiant );
        Serializable::SaveColor( _json, "diffuse", pointLight.mDiffuse );
        Serializable::SaveColor( _json, "specular", pointLight.mSpecular );
        Serializable::SaveVec3( _json, "attenuation", Vector3( pointLight.mAttenuation[0], pointLight.mAttenuation[1], pointLight.mAttenuation[2] ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PointLight::Load( EcsComponent& _component, const Json& _json )
    {
        PointLight& pointLight = static_cast<PointLight&>( _component );
        Serializable::LoadColor( _json, "ambiant", pointLight.mAmbiant );
        Serializable::LoadColor( _json, "diffuse", pointLight.mDiffuse );
        Serializable::LoadColor( _json, "specular", pointLight.mSpecular );

        Vector3 tmp;
        Serializable::LoadVec3( _json, "attenuation", tmp );
        pointLight.mAttenuation[0] = tmp[0];
        pointLight.mAttenuation[1] = tmp[1];
        pointLight.mAttenuation[2] = tmp[2];
    }
}