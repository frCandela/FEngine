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

	struct OrientedAxis {
		enum OpenSide { RIGHT=1, LEFT=2, BOTH = RIGHT | LEFT };
		
		btVector3 direction;
		OpenSide  openSide;
	};

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Update(const float /*_delta*/) {
		SCOPED_PROFILE( solar_erup);

		std::vector<Planet *> planets = m_gameobject->GetScene()->FindComponentsOfType<Planet>();

		const btVector3 thisPos = m_gameobject->GetTransform()->GetPosition();

		std::vector< OcclusionSegment > segments;
		static float size = 10.f;
		static float subAngle = 45.f;
		ImGui::DragFloat("size", &size );
		ImGui::DragFloat( "subAngle", &subAngle,1.f,1.f,180.f );

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
		for ( int segmentIndex = 0; segmentIndex < segments.size(); segmentIndex++ ) {
			OcclusionSegment & segment = segments[segmentIndex];
			Debug::Render().DebugTriangle( thisPos, size *segment.directionRight.normalized(), size * segment.directionLeft.normalized() , Color( 0, 1, 0, 0.5f ) );
		}

		// Generates oriented axis list
		std::vector<OrientedAxis> orientedAxises;
		orientedAxises.reserve(2 * segments.size());
		for ( int segmentIndex = 0; segmentIndex < segments.size(); segmentIndex++ ) {
			OcclusionSegment & segment = segments[segmentIndex];
			orientedAxises.push_back( { size * segment.directionRight.normalized() ,OrientedAxis::RIGHT } );
			orientedAxises.push_back( { size * segment.directionLeft.normalized()  ,OrientedAxis::LEFT } );
		}

		// Fill 45 degrees blanks
		float minGapRadians = btRadians( subAngle );
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


		// Draw axises
		for ( int axisIndex = 0; axisIndex < orientedAxises.size(); axisIndex++ ) {
			int nextAxisIndex = ( axisIndex + 1 ) % int( orientedAxises.size() );
			OrientedAxis & rightAxis = orientedAxises[axisIndex];
			OrientedAxis & leftAxis = orientedAxises[nextAxisIndex];

			Debug::Render().DebugLine( thisPos, rightAxis.direction, Color::Red );
			btVector3 left = 0.1f * btVector3::Up().cross( rightAxis.direction ).normalized();
			if ( rightAxis.openSide & OrientedAxis::LEFT ) {
				Debug::Render().DebugLine( rightAxis.direction, rightAxis.direction + left, Color::Green );
			}
			if ( rightAxis.openSide & OrientedAxis::RIGHT ) {
				Debug::Render().DebugLine( rightAxis.direction, rightAxis.direction - left, Color::Green );
			}

			if ( rightAxis.openSide & OrientedAxis::LEFT &&  leftAxis.openSide & OrientedAxis::RIGHT ) {
				Debug::Render().DebugTriangle( thisPos, rightAxis.direction, leftAxis.direction, Color( 1, 0, 0, 0.5f ) );
			}
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