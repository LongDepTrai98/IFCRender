#ifndef _MATH_HELPER_HPP_
#define _MATH_HELPER_HPP_
#include <array>
namespace dragon
{
	class MathHelper
	{
	public: 
		static void convertDoubleArr2FloatArr(const std::array<double, 16>& dbArray, std::array<float, 16>& flArray)
		{
			for (int i = 0; i < 16; ++i)
			{
				flArray[i] = static_cast<float>(dbArray[i]); 
			}
		}
		static void convertFloatArr2DoubleArr(const std::array<float, 16>& flArray, std::array<double, 16>& dbArray)
		{
			for (int i = 0; i < 16; ++i)
			{
				dbArray[i] = static_cast<double>(flArray[i]);
			}
		}
	private: 
		MathHelper() = default; 
	};
}
#endif // !_MATH_HELPER_HPP_
