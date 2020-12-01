#include "project_empty/game/components/fanTestComponent.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{

	//========================================================================================================
	//========================================================================================================
	void TestComponent::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &TestComponent::Load;
		_info.save        = &TestComponent::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void TestComponent::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        TestComponent& testComponent = static_cast<TestComponent&>( _component );
        testComponent.mValue = 42;
	}

	//========================================================================================================
	//========================================================================================================
	void TestComponent::Save( const EcsComponent& _component, Json& _json )
	{
		const TestComponent& testComponent = static_cast<const TestComponent&>( _component );
		Serializable::SaveInt( _json, "test_value", testComponent.mValue );
	}

	//========================================================================================================
	//========================================================================================================
	void TestComponent::Load( EcsComponent& _component, const Json& _json )
	{
        TestComponent& testComponent = static_cast<TestComponent&>( _component );
		Serializable::LoadInt( _json, "test_value", testComponent.mValue );
	}
}