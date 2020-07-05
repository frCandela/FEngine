#include "game/components/fanSolarSystem.hpp"
#include "game/components/fanPlanet.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SolarSystem::Start()
	{}

	//================================================================================================================================
	//================================================================================================================================
	void SolarSystem::Update( const float /*_delta*/ )
	{


	}

	struct PlanetData
	{
		float radius;
		float phase;
	};

	struct OrbitData
	{
		float speed;
		float radius;
		float maxScale;
		std::vector<PlanetData> planets;
	};

	//================================================================================================================================
	//================================================================================================================================
	void SolarSystem::OnGui()
	{
// 		EcsComponent::OnGui();
// 
// 		Transform& transform = m_gameobject->GetTransform();
// 
// 		static std::default_random_engine m_generator;
// 		static std::uniform_real_distribution<float> m_distribution( 0.f, 1.f );
// 		m_generator.seed( m_seed );
// 
// 		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
// 		{
// 
// 			ImGui::SliderInt( "seed", &m_seed, 0, 100 );
// 			ImGui::DragInt( "max planets per orbit", &m_maxPlanetsPerOrbit );
// 			ImGui::Spacing();
// 
// 			ImGui::SliderFloat( "initial radius", &m_initialRadius, 0.f, 10.f );
// 			ImGui::SliderFloat( "maxRadius", &m_maxRadius, 0.f, 100.f );
// 			ImGui::DragFloat2( "radius factors", &m_radiusFactors[ 0 ], 0.01f, 0.001f, 10.f );
// 			ImGui::DragFloat2( "random factors", &m_radiusRFactors[ 0 ], 0.01f, 0.001f, 10.f );
// 			ImGui::Spacing();
// 
// 			ImGui::SliderFloat( "scale multiplier", &m_scaleMult, 0.f, 1.f );
// 			if ( ImGui::DragFloat2( "scale min max", &m_scaleMinMax[ 0 ], 0.01f, 0.f, 1.f ) ) { if ( m_scaleMinMax[ 0 ] > m_scaleMinMax[ 1 ] ) { m_scaleMinMax[ 0 ] = m_scaleMinMax[ 1 ]; } }
// 			ImGui::DragFloat2( "speed factors", &m_speedFactors[ 0 ], 0.001f, 0.001f, 2.f );
// 
// 		} ImGui::PopItemWidth();
// 
// 		std::vector< OrbitData > m_orbits;
// 		m_orbits.clear();
// 
// 
// 
// 		float currentRadius = m_initialRadius;
// 
// 		// orbits
// 		while ( currentRadius < m_maxRadius )
// 		{
// 			OrbitData currentOrbit;
// 			currentOrbit.radius = currentRadius;
// 			currentRadius +=
// 				m_radiusFactors.x() + m_radiusFactors.y() * currentRadius +
// 				m_distribution( m_generator ) * ( m_radiusRFactors.x() + m_radiusRFactors.y() * currentRadius );
// 			m_orbits.push_back( currentOrbit );
// 		}
// 
// 		// planet max scale
// 		const float sunRadius = transform.GetScale()[ 0 ];
// 		for ( int orbitIndex = 0; orbitIndex < m_orbits.size(); orbitIndex++ )
// 		{
// 			OrbitData& orbit = m_orbits[ orbitIndex ];
// 			float prevDistance = orbitIndex == 0 ? orbit.radius - sunRadius : orbit.radius - m_orbits[ orbitIndex - 1 ].radius;
// 			float nextDistance = orbitIndex == m_orbits.size() - 1 ? prevDistance : m_orbits[ orbitIndex + 1 ].radius - orbit.radius;
// 			orbit.maxScale = 0.5f * std::min( prevDistance, nextDistance );
// 			orbit.maxScale *= m_scaleMult;
// 
// 		}
// 
// 		// num planets
// 		for ( int orbitIndex = 0; orbitIndex < m_orbits.size(); orbitIndex++ )
// 		{
// 			OrbitData& orbit = m_orbits[ orbitIndex ];
// 			int num = 1 + int( ( m_distribution( m_generator ) - 0.001f ) * m_maxPlanetsPerOrbit );
// 			orbit.planets.resize( num );
// 
// 			// phase
// 			const float maxPhaseIncrement = 2.f * PI / num;
// 			float prevPhase = 2.f * PI * m_distribution( m_generator );
// 			for ( int planetIndex = 0; planetIndex < num; planetIndex++ )
// 			{
// 				PlanetData& planet = orbit.planets[ planetIndex ];
// 				planet.phase = prevPhase;
// 				prevPhase += maxPhaseIncrement * ( 0.2f * m_distribution( m_generator ) + 0.8f );
// 			}
// 
// 			// scale
// 			for ( int planetIndex = 0; planetIndex < num; planetIndex++ )
// 			{
// 				PlanetData& planet = orbit.planets[ planetIndex ];
// 				planet.radius = orbit.maxScale;
// 			}
// 
// 			// Speed
// 			const float direction = orbitIndex % 2 ? 1.f : -1.f;
// 			const float speed = direction * ( m_speedFactors[ 0 ] + m_speedFactors[ 1 ] * orbit.radius );
// 			orbit.speed = speed;
// 		}
// 
// 		// Draw orbits
// 		for ( int orbitIndex = 0; orbitIndex < m_orbits.size(); orbitIndex++ )
// 		{
// 			OrbitData orbit = m_orbits[ orbitIndex ];
// 
// 			_world.GetSingleton<RenderDebug>().DebugCircle( transform.GetPosition(), orbit.radius, transform.Up(), 32, Color::Cyan );
// 
// 			float const time = -orbit.speed * GameTime::ElapsedSinceStartup();
// 			for ( int planetIndex = 0; planetIndex < orbit.planets.size(); planetIndex++ )
// 			{
// 				PlanetData& planet = orbit.planets[ planetIndex ];
// 				btVector3 position( std::cosf( time + planet.phase ), 0, std::sinf( time + planet.phase ) );
// 
// 				_world.GetSingleton<RenderDebug>().DebugCircle( orbit.radius * position, std::fabs( orbit.maxScale ), transform.Up(), 16, Color::Cyan );
// 			}
// 
// 		}
// 
// 
// 
// 		// Moon
// 		if ( ImGui::Button( "Populate" ) )
// 		{
// 			Scene& scene = m_gameobject->GetScene();
// 
// 			// Remove all childs
// 			const std::vector<Gameobject*>& childs = m_gameobject->GetChilds();
// 			for ( int childIndex = 0; childIndex < childs.size(); childIndex++ )
// 			{
// 				scene.DeleteGameobject( childs[ childIndex ] );
// 			}
// 
// 			// Generates planets
// 			for ( size_t orbitIndex = 0; orbitIndex < m_orbits.size(); orbitIndex++ )
// 			{
// 				const OrbitData& orbit = m_orbits[ orbitIndex ];
// 
// 				for ( int planetIndex = 0; planetIndex < orbit.planets.size(); planetIndex++ )
// 				{
// 					const PlanetData& planetData = orbit.planets[ planetIndex ];
// 
// 					Gameobject* newPlanet = scene.CreateGameobject( "planet" + std::to_string( orbitIndex ) + "-" + std::to_string( planetIndex ), m_gameobject );
// 					MeshRenderer* meshRenderer = newPlanet->AddComponent<MeshRenderer>();
// 					meshRenderer->SetPath( RenderGlobal::s_meshSphere );
// 
// 					Material* material = newPlanet->AddComponent<Material>();
// 					material->SetTexturePath( RenderGlobal::s_textureWhite );
// 					material->SetColor( Color::Brown );
// 
// 					Planet* planet = newPlanet->AddComponent<Planet>();
// 					planet->SetRadius( orbit.radius );
// 					planet->SetPhase( planetData.phase );
// 					planet->SetSpeed( orbit.speed );
// 
// 					SphereShape* shape = newPlanet->AddComponent<SphereShape>();
// 					shape->SetRadius( planetData.radius );
// 
// 					Rigidbody* rb = newPlanet->AddComponent<Rigidbody>();
// 					rb->EnableDesactivation( false );
// 					rb->SetKinematic();
// 
// 					Transform& planetTransform = newPlanet->GetTransform();
// 					planetTransform.SetScale( btVector3( planetData.radius, planetData.radius, planetData.radius ) );
// 				}
// 			}
// 		}
	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	bool SolarSystem::Load( const Json& _json )
// 	{
// 

// 		Serializable::LoadInt( _json, "seed", m_seed );
// 		Serializable::LoadInt( _json, "maxPlanetsPerOrbit", m_maxPlanetsPerOrbit );
// 		Serializable::LoadFloat( _json, "max_radius", m_maxRadius );
// 		Serializable::LoadFloat( _json, "initial_radius", m_initialRadius );
// 		Serializable::LoadFloat( _json, "scaleMult", m_scaleMult );
// 		Serializable::LoadVec2( _json, "radiusFactors", m_radiusFactors );
// 		Serializable::LoadVec2( _json, "radiusRFactors", m_radiusRFactors );
// 		Serializable::LoadVec2( _json, "scaleMinMax", m_scaleMinMax );
// 		Serializable::LoadVec2( _json, "speedFactors", m_speedFactors );
// 
// 		return true;
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	bool SolarSystem::Save( Json& _json ) const
// 	{
// 		Serializable::SaveInt( _json, "seed", m_seed );
// 		Serializable::SaveInt( _json, "maxPlanetsPerOrbit", m_maxPlanetsPerOrbit );
// 		Serializable::SaveFloat( _json, "max_radius", m_maxRadius );
// 		Serializable::SaveFloat( _json, "initial_radius", m_initialRadius );
// 		Serializable::SaveFloat( _json, "scaleMult", m_scaleMult );
// 		Serializable::SaveVec2( _json, "radiusFactors", m_radiusFactors );
// 		Serializable::SaveVec2( _json, "radiusRFactors", m_radiusRFactors );
// 		Serializable::SaveVec2( _json, "scaleMinMax", m_scaleMinMax );
// 		Serializable::SaveVec2( _json, "speedFactors", m_speedFactors );
// 
// 
// 		return true;
// 	}
}