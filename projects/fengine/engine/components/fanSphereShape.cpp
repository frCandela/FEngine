#include "engine/components/fanSphereShape.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SphereShape::SetInfo( EcsComponentInfo& _info )
	{
		_info.destroy     = &SphereShape::Destroy;
		_info.load        = &SphereShape::Load;
		_info.save        = &SphereShape::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void SphereShape::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		SphereShape& sphereShape = static_cast<SphereShape&>( _component );
		sphereShape.mSphereShape = new btSphereShape( 1.f );
		sphereShape.mSphereShape->setUserPointer( nullptr );
	}

	//========================================================================================================
	//========================================================================================================
	void SphereShape::Destroy( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		SphereShape& sphereShape = static_cast<SphereShape&>( _component );
        fanAssert( sphereShape.mSphereShape != nullptr );
		delete sphereShape.mSphereShape;
		sphereShape.mSphereShape = nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	void SphereShape::Save( const EcsComponent& _component, Json& _json )
	{
		const SphereShape& sphereShape = static_cast<const SphereShape&>( _component );
		Serializable::SaveFloat( _json, "radius", sphereShape.GetRadius() );
	}

	//========================================================================================================
	//========================================================================================================
	void SphereShape::Load( EcsComponent& _component, const Json& _json )
	{
		SphereShape& sphereShape = static_cast<SphereShape&>( _component );
		float radius;
		Serializable::LoadFloat( _json, "radius", radius );

		sphereShape.SetRadius( radius );
	}

	//========================================================================================================
	//========================================================================================================
	void  SphereShape::SetRadius( const float _radius )
	{
		mSphereShape->setUnscaledRadius( _radius >= 0 ? _radius : 0.f );
	}

	//========================================================================================================
	//========================================================================================================
	float SphereShape::GetRadius() const
	{
		return mSphereShape->getRadius();
	}
}