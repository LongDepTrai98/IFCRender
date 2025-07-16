#ifndef _MATH_HELPER_HPP_
#define _MATH_HELPER_HPP_
#include <array>
#include <string>
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
		static std::string colorToHash(const float& r, const float& g, const float& b, const float& w)
		{
			std::ostringstream ss;
			ss << std::setw(3) << std::setfill('0') << static_cast<int>(r * 255)
				<< std::setw(3) << std::setfill('0') << static_cast<int>(g * 255)
				<< std::setw(3) << std::setfill('0') << static_cast<int>(b * 255)
				<< std::setw(3) << std::setfill('0') << static_cast<int>(w * 255);
			return ss.str();
		}
		template<typename T> 
		static void calD()
		{
			
		}
	private: 
		MathHelper() = default; 
	};
}
#endif // !_MATH_HELPER_HPP_
