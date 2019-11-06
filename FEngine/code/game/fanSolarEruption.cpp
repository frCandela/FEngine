#include "fanGlobalIncludes.h"
#include "game/fanSolarEruption.h"

#include "game/fanPlanet.h"
#include "core/time/fanProfiler.h"
#include "renderer/fanRenderer.h"
#include "renderer/fanMesh.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
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

		m_model = m_gameobject->GetComponent<Model>();
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnAttach() {
		Actor::OnAttach();

		m_mesh = new Mesh();
		m_mesh->SetHostVisible(true);
		m_mesh->SetOptimizeVertices( false );
		m_mesh->SetAutoUpdateHull( false );
		m_mesh->SetPath("solar_eruption");
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnDetach() {
		Actor::OnDetach();
		delete m_mesh;
		if( m_model != nullptr ) {
			m_model->SetMesh( nullptr );		
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
				const float scale = m_eruptionTime / m_warmingTime; // between 0-1

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
				const float scale = m_eruptionTime / m_waitingTime; // between 0-1

				// Particles
				m_particleSystem->SetParticlesPerSecond( int( scale * ( m_waitingMaxParticles - m_warmingMaxParticles ) ) );

				if ( m_eruptionTime > m_waitingTime ) {
					m_eruptionTime -= m_waitingTime;
					m_state = EXPODING;
				}
			} break;
			case EXPODING: {
				const float scale = std::clamp ( m_eruptionTime / m_explosionFadeinTime, 0.f, 1.f );

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
				const float scale = m_eruptionTime / m_backToNormalTime;

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
		

	bool IsLeftOf( const btVector3& _dir, const btVector3& _otherDir )
	{
		float angle = _otherDir.SignedAngle( _dir, btVector3::Up() );
		return btDegrees( angle ) < 180.f;
	}

	//================================================================================================================================
	// Helper : Generates a triangle that represents a segment of a circle of radius m_radius
	//================================================================================================================================
	void SolarEruption::AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1)
	{
		const glm::vec3 normal( 0.f, 1.f, 0.f );
		const glm::vec3 color( 1.f, 1.f, 1.f );
		const glm::vec3 center( 0.f, 0.f, 0.f );
		const glm::vec2 centerUV( 0.5f, 0.5f );

		glm::vec3 p1( _v0[0], 0.f, _v0[2] );
		glm::vec3 p2( _v1[0], 0.f, _v1[2] );
		glm::vec2 uv1( _v0[0], _v0[2] );
		glm::vec2 uv2( _v1[0], _v1[2] );

		uv1 = 0.5f * uv1 / m_radius + glm::vec2( 0.5f, 0.5f );
		uv2 = 0.5f * uv2 / m_radius + glm::vec2( 0.5f, 0.5f );

		_vertices.push_back( { center,	normal, color,centerUV	} );
		_vertices.push_back( { p1,		normal, color, uv1		} );
		_vertices.push_back( { p2,		normal, color, uv2		} );


		if ( m_debugDraw ) {

			const Color debugColor( 1.f, 1.f, 0.f, 0.3f );
			Debug::Render().DebugTriangle( btVector3::Zero(), _v0, _v1, debugColor );
			Debug::Render().DebugLine( btVector3::Zero(), _v0, Color::Green );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::GenerateMesh() {
		std::vector<Planet *> planets = m_gameobject->GetScene()->FindComponentsOfType<Planet>();

		std::vector< OrientedSegment > segments;

		// Generates occlusion rays for each planet
		for (int planetIndex = 0; planetIndex < planets.size() ; planetIndex++)	{
			Planet * planet = planets[planetIndex];
			const Transform * planetTransform = planet->GetGameobject()->GetTransform();
			const btVector3 planetPos = planetTransform->GetPosition();
			const btVector3 dir = planetPos - btVector3::Zero();
			const btVector3 left = btVector3::Up().cross( dir ).normalized();

			btVector3 planetLeft = planetPos + planetTransform->GetScale().getX() * left;
			btVector3 planetRight = planetPos - planetTransform->GetScale().getX() * left;

			segments.push_back( { planetRight, OrientedSegment::RIGHT, planetRight.norm() } );
			segments.push_back( { planetLeft, OrientedSegment::LEFT, planetRight.norm() } );
		}

		// Sort the segments in ascending order ( counter clockwise )
		std::sort( std::begin( segments ), std::end( segments ),
			[] ( OrientedSegment& _s1, OrientedSegment& _s2 )
		{
			return btVector3::Left().SignedAngle( _s1.direction, btVector3::Up() ) < btVector3::Left().SignedAngle( _s2.direction, btVector3::Up() );
		} );

		// Finds the starting point of mesh generation loop
		int startIndex = 0;
		{
			// Finds the index of a RIGHT axis that has a minimal number of nested levels of planets	(depth)	
			int startIndexDepth = 10000;
			int depth = 0;	//			
			for ( int axisIndex = 0; axisIndex < segments.size(); axisIndex++ )
			{
				OrientedSegment& axis = segments[axisIndex];
				depth += axis.openSide == OrientedSegment::RIGHT ? 1 : -1;
				if ( axis.openSide == OrientedSegment::RIGHT && depth < startIndexDepth )
				{
					startIndex = axisIndex;
					startIndexDepth = depth;
				}
			}
		}

		// generates the mesh
		std::vector<Vertex>	vertices;
		vertices.reserve( 3 * planets.size() );
		const float minGapRadians = btRadians( m_subAngle );
		std::set<float> norms;	// Stores the nested opening segments norms
		for ( int axisIndex = 0; axisIndex < segments.size(); axisIndex++ )
		{
			const int index = (axisIndex + startIndex) % segments.size();
			const int indexNext = ( index + 1 ) % segments.size();
			OrientedSegment& axis = segments[ index ];
			OrientedSegment& axisNext = segments[ indexNext ];
			
			float length = 0.f;
			if ( axis.openSide == OrientedSegment::RIGHT )
			{
				// Easy case of an opening segment
				norms.insert( axis.norm );
				length = *norms.begin(); // Gets the smallest norm
			}
			else if ( axis.openSide == OrientedSegment::LEFT ) {
				norms.erase( axis.norm );
				if ( ! norms.empty() )
				{
					length = *norms.begin(); // Easy case of a closing segment
				}
				else { 
					if ( axisNext.openSide == OrientedSegment::RIGHT ){ 
						// Empty space with no planets -> fills the space with triangles
						float angle = axis.direction.SignedAngle( axisNext.direction, btVector3::Up() );
						if ( angle > minGapRadians ) // gap is too large -> subdivise it
						{

							const int numSubdivistions = int( angle / minGapRadians ) + 1;
							const float subdivisionAngle = angle / numSubdivistions;
							btTransform rotate( btQuaternion( btVector3::Up(), subdivisionAngle ) );
							btVector3 subAxisNext = m_radius * axis.direction / axis.norm;
							for ( int subAxisIndex = 0; subAxisIndex < numSubdivistions; subAxisIndex++ )
							{
								btVector3 subAxis = subAxisNext;
								subAxisNext = rotate * subAxisNext;
								AddSunTriangle(vertices, subAxis , subAxisNext );	
							}
						}
						else // gap size is small enougth
						{
							length = m_radius;
						}
					}
					else
					{
						// I think this is a degenerate case that doesn't need to be considered #provemewrong
						////Debug::Warning( "VIDE JURIDIQUE" );
					} 
				}
			}

			// Generate a light triangle based on the processed segment length
			if( length > 0.f ) {
				const btVector3 v0 = length * axis.direction / axis.norm;
				const btVector3 v1 = length * axisNext.direction / axisNext.norm;
				AddSunTriangle( vertices, v0, v1 );
			}
		}

		// Load mesh
		m_mesh->LoadFromVertices( vertices );
		Model * model = m_gameobject->GetComponent<Model>();
		if( model ) {
			model->SetMesh( m_mesh );
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