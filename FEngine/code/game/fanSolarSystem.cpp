#include "fanGlobalIncludes.h"
#include "game/fanSolarSystem.h"

#include "core/fanTime.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanMaterial.h"
#include "game/fanPlanet.h"
#include "renderer/fanRenderer.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( SolarSystem );
	REGISTER_TYPE_INFO( SolarSystem )

	//================================================================================================================================
	//================================================================================================================================
	void SolarSystem::Start() {
		Debug::Log( "bwabwa" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarSystem::Update( const float /*_delta*/ ) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarSystem::OnGui() {
		Component::OnGui();

		Transform * transform = GetGameobject()->GetComponent<Transform>();





		static std::default_random_engine m_generator;
		static std::uniform_real_distribution<float> m_distribution( 0.f, 1.f );
		m_generator.seed( m_seed );		

		ImGui::SliderInt( "seed", &m_seed, 0, 100 );
		if ( ImGui::CollapsingHeader( "orbits" ) ) {
			ImGui::Indent();
			ImGui::SliderFloat( "initial radius", &m_initialRadius, 0.f, 10.f );
			ImGui::SliderFloat( "maxRadius", &m_maxRadius, 0.f, 100.f );
			ImGui::SliderFloat( "offset", &m_offset, 0.f, 10.f );
			ImGui::SliderFloat( "random scale", &m_randomScale, 0.f, 10.f );
			ImGui::Unindent();
		}
		if ( ImGui::CollapsingHeader( "planets" ) ) {
			ImGui::Indent();
			ImGui::SliderFloat( "initial scale", &m_initialPlanetScale, 0.f, 10.f );
			ImGui::SliderFloat( "scale multiplier", &m_scaleMult, 0.f, 1.f );
			ImGui::Unindent();
		}
		if ( ImGui::CollapsingHeader( "speed" ) ) {
			ImGui::Indent();
			ImGui::SliderFloat( "min speed", &m_minSpeed, 0.f, 1.f );
			ImGui::SliderFloat( "max speed", &m_maxSpeed, 0.f, 1.f );
			ImGui::Unindent();
		}

		static std::vector< float > m_scaleList;
		static std::vector< float > m_maxScaleList;
		static std::vector< float > m_radiusList;
		static std::vector< float > m_satelliteRadiusList;
		static std::vector< float > m_satelliteScaleList;

		m_radiusList.clear();
		m_scaleList.clear();
		m_maxScaleList.clear();
		m_satelliteRadiusList.clear();
		m_satelliteScaleList.clear();

		float currentRadius = m_initialRadius;
		float radiusDelta = 1.f;
		// Radius
		for ( int radiusIndex = 0; currentRadius < m_maxRadius; radiusIndex++ ) {
			
			m_radiusList.push_back( currentRadius );
			Debug::Render().DebugCircle( transform->GetPosition(), m_radiusList[radiusIndex], transform->Up(), 32, Color::Cyan );

			float random = m_randomScale * (radiusIndex + 1.f ) * ( radiusIndex + 1.f ) * m_distribution( m_generator );
			currentRadius += radiusDelta + random + m_offset;
		}

		// Scale
		const float sunRadius = transform->GetScale()[0];
		for (int scaleIndex = 0; scaleIndex < m_radiusList.size() ; scaleIndex++) {
			float radius = m_radiusList[scaleIndex];
			float prevDistance = scaleIndex == 0 ? radius - sunRadius : radius - m_radiusList[scaleIndex - 1];
			float nextDistance = scaleIndex == m_radiusList.size()  - 1 ? prevDistance : m_radiusList[scaleIndex + 1] - radius;
			float maxScale = 0.5f * std::min( prevDistance , nextDistance );

			float scale = maxScale * m_scaleMult;

			// Scale
			m_scaleList.push_back( scale );
			m_maxScaleList.push_back( maxScale );
			Debug::Render().DebugCircle( m_radiusList[scaleIndex] * transform->Left(), m_scaleList[scaleIndex], transform->Up(), 16, Color::Cyan );
		}

		// Satellite
		for ( int scaleIndex = 0; scaleIndex < m_radiusList.size(); scaleIndex++ ) {	

			float maxSatelliteScale = 0.5f * ( m_maxScaleList[scaleIndex] - m_scaleList[scaleIndex] );
			float maxRadius = m_scaleList[scaleIndex] + maxSatelliteScale;

			m_satelliteRadiusList.push_back( maxRadius );
			m_satelliteScaleList.push_back( 0.3f * maxSatelliteScale );

			Debug::Render().DebugCircle( ( m_radiusList[scaleIndex] + maxRadius ) * transform->Left(), maxSatelliteScale, transform->Up(), 16, Color::Grey );
			Debug::Render().DebugCircle( (m_radiusList[scaleIndex] - maxRadius ) * transform->Left(), maxSatelliteScale, transform->Up(), 16, Color::Grey );
			Debug::Render().DebugCircle( ( m_radiusList[scaleIndex] - maxRadius ) * transform->Left(), 0.5f * maxSatelliteScale, transform->Up(), 16, Color::Cyan );
		}

		// Moon

		if ( ImGui::Button( "Populate" ))
		{
			Scene * scene = GetGameobject()->GetScene();

			// Remove all childs
			const std::vector<Gameobject*>& childs = GetGameobject()->GetChilds();
			for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
				scene->DeleteGameobject( childs[childIndex] );
			}

			// Generates planets
			for ( size_t radiusIndex = 0; radiusIndex < m_radiusList.size(); radiusIndex++ ) {
				const float radius = m_radiusList[radiusIndex];
				const float scale = m_scaleList[radiusIndex];

				Gameobject * newPlanet = scene->CreateGameobject("planet" + std::to_string(radiusIndex), GetGameobject() );
				Model * model = newPlanet->AddComponent<Model>();
				model->SetPath("content/models/planet.fbx");

// 				Material * material = newPlanet->AddComponent<Material>();
// 				material->SetTexture()

				Planet * planet = newPlanet->AddComponent<Planet>();
				planet->SetRadius( radius );
				planet->SetPhase( 2 * PI * m_distribution( m_generator ) );

				float direction = m_distribution( m_generator ) > 0.5f ? 1.f: -1.f;
				float planetSpeed = m_minSpeed + (m_maxSpeed - m_minSpeed) * m_distribution( m_generator );
				planet->SetSpeed( direction * planetSpeed );

				Transform * planetTransform = newPlanet->GetTransform();
				planetTransform->SetScale( btVector3( scale, scale, scale ));

				if ( m_satelliteRadiusList[radiusIndex] > 0 ) {
					Gameobject * newSatellite = scene->CreateGameobject( "satellite" + std::to_string( radiusIndex ), newPlanet );
					Model * modelSatellite = newSatellite->AddComponent<Model>();
					modelSatellite->SetPath( "content/models/planet.fbx" );

					Planet * satellitePlanet = newSatellite->AddComponent<Planet>();
					satellitePlanet->SetRadius( m_satelliteRadiusList[radiusIndex] );
					satellitePlanet->SetPhase( 2 * PI * m_distribution( m_generator ) );
					satellitePlanet->SetSpeed( - 5.f * planetSpeed );

					Transform * satelliteTransform = newSatellite->GetTransform();
					const float satelliteScale = m_satelliteScaleList[radiusIndex];
					satelliteTransform->SetScale( btVector3( satelliteScale, satelliteScale, satelliteScale ) );
				}
			}
		}
	
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarSystem::Load( std::istream& _in ) {
		Actor::Load( _in );

		if ( !ReadSegmentHeader( _in, "max_radius:" ) ) { return false; }
		if ( !ReadFloat( _in, m_maxRadius ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "initial_radius:" ) ) { return false; }
		if ( !ReadFloat( _in, m_initialRadius ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "offset:" ) ) { return false; }
		if ( !ReadFloat( _in, m_offset ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "randomScale:" ) ) { return false; }
		if ( !ReadFloat( _in, m_randomScale ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "initialPlanetScale:" ) ) { return false; }
		if ( !ReadFloat( _in, m_initialPlanetScale ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "scaleMult:" ) ) { return false; }
		if ( !ReadFloat( _in, m_scaleMult ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "minSpeed:" ) ) { return false; }
		if ( !ReadFloat( _in, m_minSpeed ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "maxSpeed:" ) ) { return false; }
		if ( !ReadFloat( _in, m_maxSpeed ) ) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarSystem::Save( std::ostream& _out, const int _indentLevel ) const {
		Actor::Save( _out, _indentLevel );
		const std::string indentation = GetIndentation( _indentLevel );

		_out << indentation << "max_radius:         " << m_maxRadius << std::endl;
		_out << indentation << "initial_radius:     " << m_initialRadius << std::endl;
		_out << indentation << "offset:             " << m_offset << std::endl;
		_out << indentation << "randomScale:        " << m_randomScale << std::endl;
		_out << indentation << "initialPlanetScale: " << m_initialPlanetScale << std::endl;
		_out << indentation << "scaleMult:          " << m_scaleMult << std::endl;
		_out << indentation << "minSpeed:           " << m_minSpeed << std::endl;
		_out << indentation << "maxSpeed:           " << m_maxSpeed << std::endl;
		return true;
	}
}