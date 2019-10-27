#include "fanGlobalIncludes.h"
#include "game/fanSolarEruption.h"

#include "game/fanPlanet.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "renderer/fanRenderer.h"
#include "scene/components/fanTransform.h"
#include "core/time/fanProfiler.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( SolarEruption )
	REGISTER_TYPE_INFO( SolarEruption )

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Start() {
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnAttach() {
		Actor::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnDetach() {
		Actor::OnDetach();
	}

	struct OcclusionSegment {
		btVector3 directionLeft;
		btVector3 directionRight;

		bool IsLeftOf( const OcclusionSegment& _other ) const {
			return IsLeftOf(directionRight, _other.directionRight );
		}

		static bool IsLeftOf( const btVector3& _dir, const btVector3& _otherDir ) {
			float angle = _otherDir.SignedAngle( _dir, btVector3::Up() );
			return btDegrees( angle ) < 180.f;
		}

		bool IsInsideOf( const OcclusionSegment& _other ) const {
			return	OcclusionSegment::IsLeftOf ( _other.directionLeft, directionLeft ) &&
					OcclusionSegment::IsLeftOf ( directionRight, _other.directionRight );
		}

		bool IsOverlapping( const OcclusionSegment& _other, OcclusionSegment& _outFusion ) const {
			const OcclusionSegment & rightMost = this->IsLeftOf( _other ) ? _other : *this;
			const OcclusionSegment & leftMost = &rightMost == this ? _other : *this;

			if ( OcclusionSegment::IsLeftOf ( rightMost.directionLeft, leftMost.directionRight ) ) {
				_outFusion.directionLeft = leftMost.directionLeft;
				_outFusion.directionRight = rightMost.directionRight;
				return true;
			} else {
				return false;
			}			
		}

	};

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Update(const float /*_delta*/) {
		SCOPED_PROFILE( solar_erup);

		std::vector<Planet *> planets = m_gameobject->GetScene()->FindComponentsOfType<Planet>();

		const btVector3 thisPos = m_gameobject->GetTransform()->GetPosition();

		std::vector< OcclusionSegment > segments;
		static float size = 10.f;

		ImGui::DragFloat("size", &size );

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


		static int numChoose = int( segments.size() - 1 );
		if ( numChoose > segments.size() ) {
			numChoose = int( segments.size() );
		}
		ImGui::DragInt( "numChoose", &numChoose, 1, 0, int( segments.size() - 1 ) );

		// Remove overlapping segments
		bool foundOverlapping = true;
		while ( foundOverlapping && segments.size() >= 2) {
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

		// Remove overlapping segments
		for ( int segmentIndex = 0; segmentIndex < segments.size(); segmentIndex++ ) {
			OcclusionSegment & segment = segments[segmentIndex];
			Debug::Render().DebugTriangle( thisPos, segment.directionRight, segment.directionLeft, Color( 1, 0, 0, 0.5f ) );
		}
	}
	   
	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnGui() {
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Load( Json & _json) {
		Actor::Load(_json);
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Save( Json & _json ) const {
		Actor::Save( _json );		
		return true;
	}
}