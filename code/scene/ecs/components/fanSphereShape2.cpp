#include "scene/ecs/components/fanSphereShape2.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( SphereShape2, "sphere_shape" );

	//================================================================================================================================
	//================================================================================================================================
	SphereShape2::SphereShape2() : sphereShape( 0.5f ){}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::SPHERE_SHAPE16;
		_info.onGui = &SphereShape2::OnGui;
		_info.init = &SphereShape2::Init;
		_info.load  = &SphereShape2::Load;
		_info.save  = &SphereShape2::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape2::Init( ecComponent& _component )
	{
		SphereShape2& sphereShape = static_cast<SphereShape2&>( _component );
		sphereShape.sphereShape = btSphereShape( 1.f );
		sphereShape.sphereShape.setUserPointer( nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape2::OnGui( ecComponent& _sphereShape )
	{
		SphereShape2& sphereShape = static_cast<SphereShape2&>( _sphereShape );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			float radius = sphereShape.GetRadius();
			if( ImGui::DragFloat( "radius", &radius, 0.1f, 0.f ) )
			{
				sphereShape.SetRadius( radius );
			}
			//RendererDebug::Get().DebugSphere( m_gameobject->GetTransform().GetBtTransform(), radius, 2, Color::Green );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape2::Save( const ecComponent& _sphereShape, Json& _json )
	{
		const SphereShape2& sphereShape = static_cast<const SphereShape2&>( _sphereShape );
		Serializable::SaveFloat( _json, "radius", sphereShape.GetRadius() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape2::Load( ecComponent& _sphereShape, const Json& _json )
	{
		SphereShape2& sphereShape = static_cast<SphereShape2&>( _sphereShape );
		float radius;
		Serializable::LoadFloat( _json, "radius", radius );

		sphereShape.SetRadius( radius );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  SphereShape2::SetRadius( const float _radius )
	{
		sphereShape.setUnscaledRadius( _radius >= 0 ? _radius : 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	float SphereShape2::GetRadius() const
	{
		return sphereShape.getRadius();
	}
}