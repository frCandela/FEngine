#pragma once

#include "LinearMath/btVector3.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

namespace fan
{
	//========================================================================================================
	// bullet physics has no 2D vector,
	// @todo btVector2 MUST DIE !!! delete this and replace with glm::vec2 where needed
	//========================================================================================================
	struct btVector2
	{
	public:
		btVector2( const btVector3 _vec3 ) { mVector = _vec3; }
		btVector2() : mVector( 0, 0, 0 ) {}
		btVector2( const btScalar _x, const btScalar _y ) : mVector( _x, _y, 0 ) {}

		static btVector2 Zero() { return btVector2( 0.f, 0.f ); }
		btScalar x() const { return mVector.x(); }
		btScalar y() const { return mVector.y(); }
		void setX( const btScalar _x ) { mVector.setX( _x ); }
		void setY( const btScalar _y ) { mVector.setY( _y ); }

		btScalar& operator[] ( int _index ) { return mVector[ _index ]; }
		btScalar operator[] ( int _index ) const { return mVector[ _index ]; }
		btVector2& operator+=( const btVector2& _v )
		{
            mVector += _v.mVector;
			return *this;
		}
		btVector2& operator-=( const btVector2& _v )
		{
            mVector -= _v.mVector;
			return *this;
		}
		btVector2& operator*=( const btScalar& _value )
		{
            mVector *= _value;
			return *this;
		}
		btVector2& operator/=( const btScalar& _value )
		{
            mVector /= _value;
			return *this;
		}
		btVector2& operator*=( const btVector2& _value )
		{
            mVector *= _value.mVector;
			return *this;
		}
		btVector2& operator/=( const btVector2& _value )
		{
            mVector = mVector / _value.mVector;
			return *this;
		}
		bool operator!=( const btVector2& _value ) const
		{
			return mVector[ 0 ] != _value.mVector[ 0 ] || mVector[ 1 ] != _value.mVector[ 1 ];
		}
		bool operator==( const btVector2& _value ) const
		{
			return mVector[ 0 ] == _value.mVector[ 0 ] && mVector[ 1 ] == _value.mVector[ 1 ];
		}

	private:
		btVector3 mVector;
	};

	//========================================================================================================
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