#include "fanUIAlign.hpp"
#include "fanUITransform.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIAlign::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &UIAlign::Load;
        _info.save = &UIAlign::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIAlign::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        UIAlign& align = static_cast<UIAlign&>( _component );
        align.mCorner    = AlignCorner::TopLeft;
        align.mDirection = AlignDirection::HorizontalVertical;
        align.mUnitType  = UnitType::Ratio;
        align.mOffset    = glm::vec2( 0.5f, 0.5f );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIAlign::Save( const EcsComponent& _component, Json& _json )
    {
        const UIAlign& align = static_cast<const UIAlign&>( _component );
        Serializable::SaveInt( _json, "corner", align.mCorner );
        Serializable::SaveInt( _json, "direction", align.mDirection );
        Serializable::SaveInt( _json, "unitType", align.mUnitType );
        Serializable::SaveVec2( _json, "ratio", align.mOffset );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIAlign::Load( EcsComponent& _component, const Json& _json )
    {
        UIAlign& align = static_cast<UIAlign&>( _component );

        int corner, direction, unitType;

        Serializable::LoadInt( _json, "corner", corner );
        Serializable::LoadInt( _json, "direction", direction );
        Serializable::LoadInt( _json, "unitType", unitType );
        Serializable::LoadVec2( _json, "ratio", align.mOffset );

        align.mCorner    = AlignCorner( corner );
        align.mDirection = AlignDirection( direction );
        align.mUnitType  = UnitType( unitType );
    }
}