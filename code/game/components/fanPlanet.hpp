#pragma once

#include "game/fanGamePrecompiled.hpp"
#include "ecs/fanComponent.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Planet : public Component
	{
		DECLARE_COMPONENT( Planet )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void OnGui( Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float timeAccumulator;
		float speed;
		float radius;
		float phase;
	};
	static constexpr size_t sizeof_planet = sizeof( Planet );




	// 	class Planet// : public Actor
// 	{
// 	public:
// 		void Start() /*override*/;
// 		void Stop() /*override*/ {}
// 		void Update( const float _delta ) /*override*/;
// 		void LateUpdate( const float /*_delta*/ ) /*override*/ {}
// 
// // 		void SetSpeed( const float _speed );
// // 		void SetRadius( const float _radius );
// // 		void SetPhase( const float _phase );
// 
// 		void OnGui() /*override*/;
// 		//ImGui::IconType GetIcon() const /*override*/ { return ImGui::IconType::PLANET16; }
// 
// 
// 	protected:
// 		void OnAttach() /*override*/;
// 		void OnDetach() /*override*/;
// 		bool Load( const Json& _json ) /*override*/;
// 		bool Save( Json& _json ) const /*override*/;
// 
// 	private:
// 
// 		//ecsPlanet* const m_planet = nullptr;
// 	};
}