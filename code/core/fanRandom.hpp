#include <random>

namespace fan
{
	//==============================================================================================================================================================
	// random numbers generator
	//==============================================================================================================================================================
	class Random
	{
	public:
		static float Float();		// range [0.f,  1.f]
		static float FloatClip();	// range [-1.f, 1.f]

	private:
		static std::default_random_engine			 m_generator;
		static std::uniform_real_distribution<float> m_distributionZeroToOne;
		static std::uniform_real_distribution<float> m_distributionMinusOneToOne;
	};
}