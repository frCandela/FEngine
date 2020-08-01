#pragma once

#include <cstdint>
#include <bitset>

namespace fan
{
	static constexpr uint32_t ecsSignatureLength = 64; 
	using EcsSignature = std::bitset<ecsSignatureLength>;
	using EcsHandle = uint32_t;

	// theses groups are used to set the color of the singleton/components icons
	enum class EngineGroups
	{
		None = 0, 
		Network,
		Scene,
		SceneUI, 
		ScenePhysics, 
		SceneRender,
		Game,
		GameNetwork,
		Editor,		
		Count
	};

	constexpr const char* GetEngineGroupName( const EngineGroups _group )
	{
		switch( _group )
		{
		case EngineGroups::None:			return "None";
		case EngineGroups::Network:			return "Network";
		case EngineGroups::Scene:			return "Scene";
		case EngineGroups::SceneUI:			return "SceneUI";
		case EngineGroups::ScenePhysics:	return "ScenePhysics";
		case EngineGroups::SceneRender:		return "SceneRender";
		case EngineGroups::Game:			return "Game";
		case EngineGroups::GameNetwork:		return "GameNetwork";
		case EngineGroups::Editor:			return "Editor";
		default:							return "Error";
		}
	}
}