#include "core/math/shapes/fanAABB.hpp"
#include "core/math/shapes/fanPlane.hpp"

namespace fan
{
	AABB::AABB() : 
		  m_low ( -0.5f * btVector3::One() )
		, m_high(  0.5f * btVector3::One() ) 
	{}

	//================================================================================================================================
	//================================================================================================================================
	AABB::AABB( const btVector3 _low, const btVector3 _high ) :
		m_low( _low ),
		m_high( _high )
	{
		assert( _low[ 0 ] <= _high[ 0 ] && _low[ 1 ] <= _high[ 1 ] && _low[ 2 ] <= _high[ 2 ] );
	}

	//================================================================================================================================
	// Computes the AABB of a a transformed points cloud
	//================================================================================================================================
	AABB::AABB( const std::vector<btVector3> _pointCloud, const glm::mat4 _modelMatrix )
	{
		if( _pointCloud.empty() )
		{
			m_low = -0.5f * btVector3::One();
			m_high = 0.5f * btVector3::One();
			return;
		}

		glm::vec3 high( std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() );
		glm::vec3 low( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );

		for ( int index = 0; index < (int)_pointCloud.size(); index++ )
		{
			const glm::vec4 vertex = _modelMatrix * glm::vec4( _pointCloud[ index ][ 0 ], _pointCloud[ index ][ 1 ], _pointCloud[ index ][ 2 ], 1.f );
			if ( vertex.x < low.x ) { low.x = vertex.x; }
			if ( vertex.y < low.y ) { low.y = vertex.y; }
			if ( vertex.z < low.z ) { low.z = vertex.z; }
			if ( vertex.x > high.x ) { high.x = vertex.x; }
			if ( vertex.y > high.y ) { high.y = vertex.y; }
			if ( vertex.z > high.z ) { high.z = vertex.z; }
		}
		m_low = ToBullet( low );
		m_high = ToBullet( high );
		assert( m_low[ 0 ] <= m_high[ 0 ] && m_low[ 1 ] <= m_high[ 1 ] && m_low[ 2 ] <= m_high[ 2 ] );
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< btVector3 > AABB::GetCorners() const
	{
		return {
			// Top face
			m_high
			,btVector3( m_low[ 0 ],m_high[ 1 ],m_high[ 2 ] )
			,btVector3( m_low[ 0 ],m_high[ 1 ],m_low[ 2 ] )
			,btVector3( m_high[ 0 ],m_high[ 1 ],m_low[ 2 ] )
			//Bot face
			,btVector3( m_high[ 0 ],m_low[ 1 ],m_high[ 2 ] )
			,btVector3( m_low[ 0 ],m_low[ 1 ],m_high[ 2 ] )
			,m_low
			,btVector3( m_high[ 0 ],m_low[ 1 ],m_low[ 2 ] )
		};
	}

	//================================================================================================================================
	// Fast Ray - Box Intersection
	// by Andrew Woo
	// from "Graphics Gems", Academic Press, 1990
	// Original code : https://github.com/erich666/GraphicsGems/blob/master/gems/RayBox.c
	//================================================================================================================================
	bool AABB::RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const
	{
		const int RIGHT = 0;
		const int LEFT = 1;
		const int MIDDLE = 2;

		bool inside = true;
		btVector3 quadrant( 0, 0, 0 );
		int i;
		int whichPlane;
		btVector3 maxT;
		btVector3 candidatePlane;

		// Find candidate planes; this loop can be avoided if
		// rays cast all from the eye(assume perpsective view) 
		for ( i = 0; i < 3; i++ )
		{
			if ( _origin[ i ] < m_low[ i ] )
			{
				quadrant[ i ] = LEFT;
				candidatePlane[ i ] = m_low[ i ];
				inside = false;
			}
			else if ( _origin[ i ] > m_high[ i ] )
			{
				quadrant[ i ] = RIGHT;
				candidatePlane[ i ] = m_high[ i ];
				inside = false;
			}
			else
			{
				quadrant[ i ] = MIDDLE;
			}
		}

		// Ray origin inside bounding box 
		if ( inside )
		{
			_outIntersection = _origin;
			return true;
		}


		// Calculate T distances to candidate planes 
		for ( i = 0; i < 3; i++ )
		{
			if ( quadrant[ i ] != MIDDLE && _direction[ i ] != 0. )
				maxT[ i ] = ( candidatePlane[ i ] - _origin[ i ] ) / _direction[ i ];
			else
				maxT[ i ] = -1.f;
		}

		// Get largest of the maxT's for final choice of intersection 
		whichPlane = 0;
		for ( i = 1; i < 3; i++ )
		{
			if ( maxT[ whichPlane ] < maxT[ i ] )
				whichPlane = i;
		}

		// Check final candidate actually inside box 
		if ( maxT[ whichPlane ] < 0.f )
		{
			return false;
		}

		for ( i = 0; i < 3; i++ )
		{
			if ( whichPlane != i )
			{
				_outIntersection[ i ] = _origin[ i ] + maxT[ whichPlane ] * _direction[ i ];
				if ( _outIntersection[ i ] < m_low[ i ] || _outIntersection[ i ] > m_high[ i ] )
					return false;
			}
			else
			{
				_outIntersection[ i ] = candidatePlane[ i ];
			}
		}

		return true;
	}
}
