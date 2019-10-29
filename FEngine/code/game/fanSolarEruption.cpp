#include "fanGlobalIncludes.h"
#include "game/fanSolarEruption.h"

#include "game/fanSolarEruptionUtil.h"
#include "game/fanPlanet.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanModel.h"
#include "renderer/fanRenderer.h"
#include "scene/components/fanTransform.h"
#include "core/time/fanProfiler.h"
#include "renderer/fanMesh.h"
#include "scene/components/fanMaterial.h"
#include "scene/actors/fanParticleSystem.h"
#include "editor/fanModals.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( SolarEruption )
	REGISTER_TYPE_INFO( SolarEruption )

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Start() {
		m_material = m_gameobject->GetComponent<Material>();
		if ( m_material == nullptr ) {
			Debug::Warning("no material found on Solar Eruption");
			SetEnabled(false);
		}

		m_particleSystem = m_gameobject->GetComponent<ParticleSystem>();
		if ( m_particleSystem == nullptr ) {
			Debug::Warning( "no particle system found on Solar Eruption" );
			SetEnabled( false );
		} else {
			m_particleSystem->SetEnabled(false);
		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnAttach() {
		Actor::OnAttach();

		m_mesh = new Mesh();
		m_mesh->SetPath("solar_eruption");
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnDetach() {
		Actor::OnDetach();
		delete m_mesh;

		Model * model = m_gameobject->GetComponent<Model>();
		if ( model != nullptr ) {
			model->SetMesh( nullptr );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Update(const float _delta) {
		SCOPED_PROFILE( solar_erup );
		UpdateStateMachine( _delta );
		GenerateMesh();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::UpdateStateMachine( const float _delta ) {
		SCOPED_PROFILE( state );

		if ( m_state != NONE ) {
			m_eruptionTime += _delta;

			switch ( m_state ) {
			case COLLAPSING: {
				float scale = m_eruptionTime / m_warmingTime; // between 0-1

				ImGui::DragFloat( "scale", &scale );

				// Color
				Color color = m_baseColor;
				color[3] *= 1.f - scale;
				m_material->SetColor( color );

				// Particles
				m_particleSystem->SetParticlesPerSecond( int( scale * m_warmingMaxParticles ) );

				if ( m_eruptionTime > m_warmingTime ) {
					m_eruptionTime -= m_warmingTime;
					m_state = WAITING;
				}
			} break;
			case WAITING: {
				float scale = m_eruptionTime / m_waitingTime; // between 0-1

				// Particles
				m_particleSystem->SetParticlesPerSecond( int( scale * ( m_waitingMaxParticles - m_warmingMaxParticles ) ) );

				if ( m_eruptionTime > m_waitingTime ) {
					m_eruptionTime -= m_waitingTime;
					m_state = EXPODING;
				}
			} break;
			case EXPODING: {
				float scale = std::clamp ( m_eruptionTime / m_explosionFadeinTime, 0.f, 1.f );
				ImGui::DragFloat( "scale", &scale );
				ImGui::DragFloat( "eruptionTime", &m_eruptionTime );

				// Particles
				m_particleSystem->SetParticlesPerSecond( int( m_explosionMaxParticles ) );
				m_particleSystem->SetSpeedParticles( m_explosionParticlesSpeed );

				//scale
				m_gameobject->GetTransform()->SetScale( btVector3( scale, 1.f, scale ) );

				// Color
				m_material->SetColor( m_explositonColor );

				if ( m_eruptionTime > m_explosionTime ) {
					m_eruptionTime -= m_explosionTime;
					m_state = BACK_TO_NORMAL;
				}
			} break;
			case BACK_TO_NORMAL: {
				float scale = m_eruptionTime / m_backToNormalTime;

				m_material->SetColor( ( 1.f - scale ) * m_explositonColor.ToGLM() + scale * m_baseColor.ToGLM() );
				m_particleSystem->SetParticlesPerSecond( int( ( 1.f - scale ) * m_explosionMaxParticles ) );

				if ( m_eruptionTime > m_backToNormalTime ) {
					m_eruptionTime -= m_backToNormalTime;
					m_state = NONE;

					m_particleSystem->SetEnabled( false );
					m_material->SetColor( m_baseColor );
				}
			} break;
			default: {
				m_state = NONE;
			} break;
			}
		}
	}
		
	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::GenerateMesh() {
		std::vector<Planet *> planets = m_gameobject->GetScene()->FindComponentsOfType<Planet>();

		const btVector3 thisPos = m_gameobject->GetTransform()->GetPosition();

		std::vector< OcclusionSegment > segments;

		// Generates occlusion rays for each planet
		for (int planetIndex = 0; planetIndex < planets.size() ; planetIndex++)	{
			Planet * planet = planets[planetIndex];
			const Transform * planetTransform = planet->GetGameobject()->GetTransform();
			const btVector3 planetPos = planetTransform->GetPosition();
			const btVector3 dir = planetPos - thisPos;
			const btVector3 left = btVector3::Up().cross( dir ).normalized();

			btVector3 planetLeft = planetPos + planetTransform->GetScale().getX() * left;
			btVector3 planetRight = planetPos - planetTransform->GetScale().getX() * left;

			segments.push_back( { planetLeft, planetRight } );	
		}

		// Sort the segments in ascending order ( counter clockwise )
		std::sort( std::begin( segments ), std::end( segments ),
			[] ( OcclusionSegment& _s1, OcclusionSegment _s2 ) {
				return btVector3::Left().SignedAngle( _s1.directionRight, btVector3::Up()) < btVector3::Left().SignedAngle( _s2.directionRight, btVector3::Up());
			}
		);

		// Remove overlapping segments
		bool foundOverlapping = true;
		while ( foundOverlapping && segments.size() >= 2 ) {
			foundOverlapping = false;
			for ( int segmentIndex = 0; segmentIndex < segments.size(); segmentIndex++ ) {
				int nextSegmentIndex = ( segmentIndex + 1 ) % int(segments.size());
				OcclusionSegment & segment = segments[ segmentIndex ];
				OcclusionSegment & nextSegment = segments[nextSegmentIndex];
		
				Color nextColor = Color( 0, 1, 0, 0.5f );
				if ( nextSegment.IsInsideOf( segment ) ) {
					foundOverlapping = true;
					segments.erase( segments.begin() + nextSegmentIndex );
					break;
				} 
				
				OcclusionSegment fusion;
				if ( nextSegment.IsOverlapping( segment, fusion ) ) {
					foundOverlapping = true;
					segments.erase( segments.begin() + nextSegmentIndex );
					segment = fusion;
					break;
				}
			}
		}

		// Draw occlusion segments
		if ( m_debugDraw ) {
			for ( int segmentIndex = 0; segmentIndex < segments.size(); segmentIndex++ ) {
				OcclusionSegment & segment = segments[segmentIndex];
				Debug::Render().DebugTriangle( thisPos, m_radius *segment.directionRight.normalized(), m_radius * segment.directionLeft.normalized(), Color( 0, 1, 0, 0.5f ) );

			}
		}

		// Generates oriented axis list
		std::vector<OrientedAxis> orientedAxises;
		orientedAxises.reserve(2 * segments.size());
		for ( int segmentIndex = 0; segmentIndex < segments.size(); segmentIndex++ ) {
			OcclusionSegment & segment = segments[segmentIndex];
			orientedAxises.push_back( { m_radius * segment.directionRight.normalized() ,OrientedAxis::RIGHT } );
			orientedAxises.push_back( { m_radius * segment.directionLeft.normalized()  ,OrientedAxis::LEFT } );
		}

		// Fill 45 degrees blanks
		float minGapRadians = btRadians( m_subAngle );
		bool hasFoundBlank = true;
		while ( hasFoundBlank ) {
			hasFoundBlank = false;		
			for ( int axisIndex = 0; axisIndex < orientedAxises.size(); axisIndex++ ) {
				int nextAxisIndex = ( axisIndex + 1 ) % int( orientedAxises.size() );
				OrientedAxis & axis = orientedAxises[axisIndex];
				OrientedAxis & nextAxis = orientedAxises[nextAxisIndex];				

				float angle = axis.direction.SignedAngle( nextAxis.direction, btVector3::Up() );
				if ( angle > minGapRadians ) {

					const int numSubdivistions = int( angle / minGapRadians ) + 1;
					const float subdivisionAngle = angle / numSubdivistions;
					btTransform rotate( btQuaternion( btVector3::Up(), subdivisionAngle ) );
					btVector3 prevDirection = axis.direction;
					for (int subAxisIndex = 0; subAxisIndex < numSubdivistions - 1; subAxisIndex++)	{
						prevDirection = rotate * prevDirection;
						orientedAxises.insert( orientedAxises.begin() + axisIndex + 1 + subAxisIndex, { prevDirection, OrientedAxis::BOTH } );
					}

					hasFoundBlank = true;
					break;
				}
			}
		}	


		std::vector<Vertex>	vertices;
		vertices.reserve( orientedAxises.size() );
		glm::vec3 up( 0.f, 1.f, 0.f );
		glm::vec3 color( 1.f, 1.f, 1.f );

		// Draw axises
		for ( int axisIndex = 0; axisIndex < orientedAxises.size(); axisIndex++ ) {
			int nextAxisIndex = ( axisIndex + 1 ) % int( orientedAxises.size() );
			OrientedAxis & rightAxis = orientedAxises[axisIndex];
			OrientedAxis & leftAxis = orientedAxises[nextAxisIndex];

			if ( m_debugDraw ) {
				Debug::Render().DebugLine( thisPos, rightAxis.direction, Color::Red );
				btVector3 left = 0.1f * btVector3::Up().cross( rightAxis.direction ).normalized();
				if ( rightAxis.openSide & OrientedAxis::LEFT ) {
					Debug::Render().DebugLine( rightAxis.direction, rightAxis.direction + left, Color::Green );
				}
				if ( rightAxis.openSide & OrientedAxis::RIGHT ) {
					Debug::Render().DebugLine( rightAxis.direction, rightAxis.direction - left, Color::Green );
				}
			}

			if ( rightAxis.openSide & OrientedAxis::LEFT &&  leftAxis.openSide & OrientedAxis::RIGHT ) {
				glm::vec3 p1( rightAxis.direction[0], 0.f, rightAxis.direction[2] );
				glm::vec3 p2( leftAxis.direction[0], 0.f, leftAxis.direction[2] );
				glm::vec2 uv1( rightAxis.direction[0], rightAxis.direction[2] );
				glm::vec2 uv2( leftAxis.direction[0], leftAxis.direction[2] );

				uv1 = 0.5f * uv1 / m_radius + glm::vec2( 0.5f, 0.5f );
				uv2 = 0.5f * uv2 / m_radius + glm::vec2( 0.5f, 0.5f );

				vertices.push_back( { glm::vec3( 0.f,0.f,0.f ), up, color, glm::vec2( 0.5f, 0.5f) } );
				vertices.push_back( { p1, up, color, uv1 });
				vertices.push_back( { p2, up, color, uv2 });

				if ( m_debugDraw ) {
					Debug::Render().DebugTriangle( thisPos, rightAxis.direction, leftAxis.direction, Color( 1, 0, 0, 0.5f ) ); 
				}
			}
		}

		m_mesh->LoadFromVertices( vertices );

		Model * model = m_gameobject->GetComponent<Model>();
		if ( model != nullptr ) {
			model->SetMesh(m_mesh);
		}
	}
	   
	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnGui() {
		
		ImGui::PushItemWidth( 100.f ); {
			// Mesh generation
			ImGui::Checkbox( "debug draw", &m_debugDraw );
			ImGui::DragFloat( "radius", &m_radius, 0.1f );
			ImGui::DragFloat( "sub-angle", &m_subAngle, 1.f, 1.f, 180.f );
			ImGui::Spacing();

			// State machine		
			ImGui::DragFloat( "eruption time", &m_eruptionTime );
			ImGui::DragFloat( "warming time", &m_warmingTime );
			ImGui::DragFloat( "warming max particles", &m_warmingMaxParticles );
			ImGui::DragFloat( "waiting time", &m_waitingTime );
			ImGui::DragFloat( "waiting max particles", &m_waitingMaxParticles );
			ImGui::DragFloat( "explosion time", &m_explosionTime );
			ImGui::DragFloat( "explosion fade-in time", &m_explosionFadeinTime );
			ImGui::DragFloat( "explosion max particles", &m_explosionMaxParticles );
			ImGui::DragFloat( "explosion particles speed", &m_explosionParticlesSpeed );
			ImGui::DragFloat( "back to normal time", &m_backToNormalTime );
			ImGui::Spacing();
		} ImGui::PopItemWidth();

		// Colors
		ImGui::Spacing();
		if ( ImGui::ColorEdit4( "base ", m_baseColor.Data(), gui::colorEditFlags ) ) {
			m_material->SetColor( m_baseColor );
		}
		ImGui::ColorEdit4( "explositon ", m_explositonColor.Data(), gui::colorEditFlags );

		// Test
		ImGui::Spacing();
		if ( ImGui::Button( "Eruption" ) ) {
			m_state = COLLAPSING;
			m_eruptionTime = 0.f;
			m_particleSystem->SetEnabled( true );
			m_material->SetColor( m_baseColor );
			m_particleSystem->SetSpeedParticles( 1.f );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Load( Json & _json) {
		Actor::Load(_json);
		LoadFloat( _json, "radius", m_radius );
		LoadFloat( _json, "sub_angle", m_subAngle );

		// State machine		
		LoadFloat( _json, "eruption time", m_eruptionTime );
		LoadFloat( _json, "warming time", m_warmingTime );
		LoadFloat( _json, "warming max particles", m_warmingMaxParticles );
		LoadFloat( _json, "waiting time", m_waitingTime );
		LoadFloat( _json, "waiting max particles", m_waitingMaxParticles );
		LoadFloat( _json, "explosion time", m_explosionTime );
		LoadFloat( _json, "explosion fade-in time", m_explosionFadeinTime );
		LoadFloat( _json, "explosion max particles", m_explosionMaxParticles );
		LoadFloat( _json, "explosion particles speed", m_explosionParticlesSpeed );
		
		// Colors
		LoadColor( _json, "base ", m_baseColor );
		LoadColor( _json, "explositon ", m_explositonColor );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Save( Json & _json ) const {
		Actor::Save( _json );		
		SaveFloat( _json, "radius", m_radius );
		SaveFloat( _json, "sub_angle", m_subAngle );

		// State machine		
		SaveFloat( _json, "eruption time", m_eruptionTime );
		SaveFloat( _json, "warming time", m_warmingTime );
		SaveFloat( _json, "warming max particles", m_warmingMaxParticles );
		SaveFloat( _json, "waiting time", m_waitingTime );
		SaveFloat( _json, "waiting max particles", m_waitingMaxParticles );
		SaveFloat( _json, "explosion time", m_explosionTime );
		SaveFloat( _json, "explosion fade-in time", m_explosionFadeinTime );
		SaveFloat( _json, "explosion max particles", m_explosionMaxParticles );
		SaveFloat( _json, "explosion particles speed", m_explosionParticlesSpeed );

		// Colors
		SaveColor( _json, "base ", m_baseColor );
		SaveColor( _json, "explositon ", m_explositonColor );
		return true;
	}
}