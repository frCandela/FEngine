#include "engine/components/ui/fanUIAlign.hpp"
#include "engine/components/ui/fanUITransform.hpp"
#include "engine/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIAlign::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &UIAlign::Load;
		_info.save        = &UIAlign::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UIAlign& align = static_cast<UIAlign&>( _component );
        align.mCorner = AlignCorner::TopLeft;
        align.mDirection = AlignDirection::HorizontalVertical;
        align.mUnitType = UnitType::Ratio;
        align.mOffset = glm::vec2( 0.5f, 0.5f );
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Save( const EcsComponent& _component, Json& _json )
	{
        const UIAlign& align = static_cast<const UIAlign&>( _component );
        Serializable::SaveInt( _json, "corner", align.mCorner );
        Serializable::SaveInt( _json, "direction", align.mDirection );
        Serializable::SaveInt( _json, "unitType", align.mUnitType );
        Serializable::SaveVec2( _json, "ratio", align.mOffset );
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Load( EcsComponent& _component, const Json& _json )
	{
        UIAlign& align = static_cast<UIAlign&>( _component );
        Serializable::LoadInt( _json, "corner", (int&)align.mCorner );
        Serializable::LoadInt( _json, "direction", (int&)align.mDirection );
        Serializable::LoadInt( _json, "unitType", (int&)align.mUnitType );
        Serializable::LoadVec2( _json, "ratio", align.mOffset );
    }
}