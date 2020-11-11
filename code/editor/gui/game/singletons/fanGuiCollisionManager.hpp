#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void CollisionManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Rigidbody16;
		_info.mGroup = EngineGroups::Game;
		_info.mName  = "collision manager";
	}
}