#include "scene/components/fanBoxShape.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( BoxShape, "box_shape" );

	//================================================================================================================================
	//================================================================================================================================
	BoxShape::BoxShape() : boxShape( btVector3( 0.5f, 0.5f, 0.5f ) ) {}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::CUBE_SHAPE16;
		_info.onGui = &BoxShape::OnGui;
		_info.init = &BoxShape::Init;
		_info.load  = &BoxShape::Load;
		_info.save  = &BoxShape::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::Init( EcsWorld& _world, Component& _component )
	{
		BoxShape& boxShape = static_cast<BoxShape&>( _component );
		boxShape.boxShape = btBoxShape( btVector3( 0.5f, 0.5f, 0.5f ) );
		boxShape.boxShape.setUserPointer( nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::OnGui( Component& _component )
	{
		BoxShape& boxShape = static_cast<BoxShape&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			btVector3 extent = boxShape.GetScaling();
			if( ImGui::DragFloat3( "scaling", &extent[0], 0.05f, 0.f ) )
			{
				boxShape.SetScaling( extent );
			}
			//RendererDebug::Get().DebugCube( m_gameobject->GetTransform().GetBtTransform(), 0.5f * extent, Color::Green );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::Save( const Component& _component, Json& _json )
	{
		const BoxShape& boxShape = static_cast<const BoxShape&>( _component );

		Serializable::SaveVec3( _json, "scaling", boxShape.GetScaling() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::Load( Component& _component, const Json& _json )
	{
		BoxShape& boxShape = static_cast<BoxShape&>( _component );

		btVector3 scaling;
		Serializable::LoadVec3( _json, "scaling", scaling );

		boxShape.SetScaling( scaling );
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::SetScaling( const btVector3 _scaling )
	{
		boxShape.setLocalScaling( _scaling );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 BoxShape::GetScaling() const
	{
		return boxShape.getLocalScaling();
	}
}