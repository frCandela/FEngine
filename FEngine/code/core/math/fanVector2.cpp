#include "core/math/fanVector2.h"

btVector2 operator+(const btVector2& _v1, const btVector2& _v2) {
	btVector2 tmp(_v1);
	tmp += _v2;
	return tmp;
}
btVector2 operator-(const btVector2& _v1, const btVector2& _v2) {
	btVector2 tmp(_v1);
	tmp -= _v2;
	return tmp;
}
btVector2 operator*(const btVector2& _v1, const btScalar& _value) {
	btVector2 tmp(_v1);
	tmp *= _value;
	return tmp;
}
btVector2 operator/(const btVector2& _v1, const btScalar& _value) {
	btVector2 tmp(_v1);
	tmp /= _value;
	return tmp;
}
btVector2 operator*(const btScalar& _value, const btVector2& _v1) {
	btVector2 tmp(_v1);
	tmp *= _value;
	return tmp;
}
btVector2 operator/(const btScalar& _value, const btVector2& _v1) {
	btVector2 tmp(_v1);
	tmp /= _value;
	return tmp;
}
btVector2 operator*(const btVector2& _v0, const btVector2& _v1) {
	btVector2 tmp(_v0);
	tmp *= _v1;
	return tmp;
}
btVector2 operator/(const btVector2& _v0, const btVector2& _v1) {
	btVector2 tmp(_v0);
	tmp /= _v1;
	return tmp;
}
