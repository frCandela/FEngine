#include "core/fanRandom.hpp"

namespace fan
{
	std::default_random_engine			  Random::m_generator;
	std::uniform_real_distribution<float> Random::m_distributionZeroToOne( 0.f, 1.f );
	std::uniform_real_distribution<float> Random::m_distributionMinusOneToOne( -1.f, 1.f );

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	float Random::Float()  { return m_distributionZeroToOne( m_generator ); }
	float Random::FloatClip() { return m_distributionMinusOneToOne( m_generator ); }
}

