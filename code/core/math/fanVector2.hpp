#pragma once

#include "bullet/LinearMath/btVector3.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

namespace fan
{
	//================================================================================================================================
	// bullet physics has no 2D vector, @todo delete this and replace with glm::vec2 where needed
	//================================================================================================================================
	struct btVector2
	{
	public:
		btVector2( const btVector3 _vec3 ) { m_vector = _vec3; }
		btVector2() : m_vector( 0, 0, 0 ) {}
		btVector2( const btScalar _x, const btScalar _y ) : m_vector( _x, _y, 0 ) {}

		static btVector2 Zero() { return btVector2( 0.f, 0.f ); }
		btScalar x() const { return m_vector.x(); }
		btScalar y() const { return m_vector.y(); }
		void setX( const btScalar _x ) { m_vector.setX( _x ); }
		void setY( const btScalar _y ) { m_vector.setY( _y ); }

		btScalar& operator[] ( int _index ) { return m_vector[ _index ]; }
		btScalar operator[] ( int _index ) const { return m_vector[ _index ]; }
		btVector2& operator+=( const btVector2& _v )
		{
			m_vector += _v.m_vector;
			return *this;
		}
		btVector2& operator-=( const btVector2& _v )
		{
			m_vector -= _v.m_vector;
			return *this;
		}
		btVector2& operator*=( const btScalar& _value )
		{
			m_vector *= _value;
			return *this;
		}
		btVector2& operator/=( const btScalar& _value )
		{
			m_vector /= _value;
			return *this;
		}
		btVector2& operator*=( const btVector2& _value )
		{
			m_vector *= _value.m_vector;
			return *this;
		}
		btVector2& operator/=( const btVector2& _value )
		{
			m_vector = m_vector / _value.m_vector;
			return *this;
		}
		bool operator!=( const btVector2& _value ) const
		{
			return m_vector[ 0 ] != _value.m_vector[ 0 ] || m_vector[ 1 ] != _value.m_vector[ 1 ];
		}
		bool operator==( const btVector2& _value ) const
		{
			return m_vector[ 0 ] == _value.m_vector[ 0 ] && m_vector[ 1 ] == _value.m_vector[ 1 ];
		}

	private:
		btVector3 m_vector;
	};

	//================================================================================================================================
	btVector2 operator+( const btVector2& _v1, const btVector2& _v2 );
	btVector2 operator-( const btVector2& _v1, const btVector2& _v2 );
	btVector2 operator*( const btVector2& _v1, const btScalar& _value );
	btVector2 operator/( const btVector2& _v1, const btScalar& _value );
	btVector2 operator*( const btScalar& _value, const btVector2& _v1 );
	btVector2 operator/( const btScalar& _value, const btVector2& _v1 );
	btVector2 operator*( const btVector2& _v0, const btVector2& _v1 );
	btVector2 operator/( const btVector2& _v0, const btVector2& _v1 );

	inline btVector2 ToBullet( const glm::vec2 _vec )	{ return btVector2( _vec.x, _vec.y ); }
	inline glm::vec2 ToGLM( const btVector2 _vec )		{ return glm::vec2( _vec[0], _vec[1] ); }
}