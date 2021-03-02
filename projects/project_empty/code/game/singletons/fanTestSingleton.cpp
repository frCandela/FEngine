#include "game/singletons/fanTestSingleton.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void TestSingleton::SetInfo( EcsSingletonInfo& _info )
	{
		_info.save   = &TestSingleton::Save;
		_info.load   = &TestSingleton::Load;
	}

	//========================================================================================================
	//========================================================================================================
	void TestSingleton::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
        TestSingleton& testSingleton = static_cast<TestSingleton&>( _component );
        testSingleton.mValue = 30.f;
	}

	//========================================================================================================
	//========================================================================================================
	void TestSingleton::Save( const EcsSingleton& _component, Json& _json )
	{
		const TestSingleton& testSingleton = static_cast<const TestSingleton&>( _component );
 		Serializable::SaveFloat( _json, "test_value", testSingleton.mValue );
	}

	//========================================================================================================
	//========================================================================================================
	void TestSingleton::Load( EcsSingleton& _component, const Json& _json )
	{
        TestSingleton& testSingleton = static_cast<TestSingleton&>( _component );
		Serializable::LoadFloat( _json, "test_value", testSingleton.mValue );
	}
}