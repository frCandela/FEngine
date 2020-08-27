#include "core/fanRandom.hpp"

namespace fan
{
	std::default_random_engine			  Random::sGenerator;
	std::uniform_real_distribution<float> Random::sDistributionZeroToOne( 0.f, 1.f );
	std::uniform_real_distribution<float> Random::sDistributionMinusOneToOne( -1.f, 1.f );

	//========================================================================================================
	//========================================================================================================
	float Random::Float()  { return sDistributionZeroToOne( sGenerator ); }
	float Random::FloatClip() { return sDistributionMinusOneToOne( sGenerator ); }
}

