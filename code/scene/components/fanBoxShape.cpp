#include "scene/components/fanBoxShape.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void BoxShape::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::CUBE_SHAPE16;
		_info.mGroup      = EngineGroups::ScenePhysics;
		_info.onGui       = &BoxShape::OnGui;
		_info.destroy     = &BoxShape::Destroy;
		_info.load        = &BoxShape::Load;
		_info.save        = &BoxShape::Save;
		_info.mEditorPath = "/";
		_info.mName       = "box_shape";
	}

	//========================================================================================================
	//========================================================================================================
	void BoxShape::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		BoxShape& boxShape = static_cast<BoxShape&>( _component );
		boxShape.mBoxShape = new btBoxShape( btVector3( 0.5f, 0.5f, 0.5f ) );
		boxShape.mBoxShape->setUserPointer( nullptr );
	}

	//========================================================================================================
	//========================================================================================================
	void BoxShape::Destroy( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		BoxShape& boxShape = static_cast<BoxShape&>( _component );
		assert( boxShape.mBoxShape != nullptr );
		delete boxShape.mBoxShape;
		boxShape.mBoxShape = nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	void BoxShape::Save( const EcsComponent& _component, Json& _json )
	{
		const BoxShape& boxShape = static_cast<const BoxShape&>( _component );

		Serializable::SaveVec3( _json, "scaling", boxShape.GetScaling() );
	}

	//========================================================================================================
	//========================================================================================================
	void BoxShape::Load( EcsComponent& _component, const Json& _json )
	{
		BoxShape& boxShape = static_cast<BoxShape&>( _component );

		btVector3 scaling;
		Serializable::LoadVec3( _json, "scaling", scaling );

		boxShape.SetScaling( scaling );
	}

	//========================================================================================================
	//========================================================================================================
	void BoxShape::SetScaling( const btVector3 _scaling )
	{
		mBoxShape->setLocalScaling( _scaling );
	}

	//========================================================================================================
	//========================================================================================================
	btVector3 BoxShape::GetScaling() const
	{
		return mBoxShape->getLocalScaling();
	}

    //========================================================================================================
    //========================================================================================================
    void BoxShape::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        BoxShape& boxShape = static_cast<BoxShape&>( _component );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            btVector3 extent = boxShape.GetScaling();
            if( ImGui::DragFloat3( "scaling", &extent[0], 0.05f, 0.f ) )
            {
                boxShape.SetScaling( extent );
            }
        }
        ImGui::PopItemWidth();
    }
}