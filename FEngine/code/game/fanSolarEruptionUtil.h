#pragma once

//================================================================================================================================
// represent a horizontal section of circle delimited by two vectors
//================================================================================================================================
struct OcclusionSegment {
	btVector3 directionLeft;
	btVector3 directionRight;

	bool IsLeftOf( const OcclusionSegment& _other ) const {
		return IsLeftOf( directionRight, _other.directionRight );
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
struct OrientedAxis {
	enum OpenSide { RIGHT = 1, LEFT = 2, BOTH = RIGHT | LEFT };

	btVector3 direction;
	OpenSide  openSide;
};
