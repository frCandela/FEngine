#include "scene/components/fanBoxShape.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void BoxShape::SetInfo( EcsComponentInfo& _info )
	{
		_info.destroy     = &BoxShape::Destroy;
		_info.load        = &BoxShape::Load;
		_info.save        = &BoxShape::Save;
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
        fanAssert( boxShape.mBoxShape != nullptr );
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
}