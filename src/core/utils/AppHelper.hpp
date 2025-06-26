#ifndef _APP_HELPER_HPP_
#define _APP_HELPER_HPP_
#include "wxInclude.hpp"
#include "view/MainViewPort.hpp"
namespace threepp
{
	class Scene; 
}
namespace dragon
{
	class WindowFrame; 
	class AppHelper
	{
	public: 
		/*APP HELPER GET MAIN VIEW PORT SCENE*/
		static MainViewPort* getMainViewPortScene(WindowFrame* main_frame);
	private: 
		AppHelper() = default; 
	};
}
#endif // !_APP_HELPER_HPP_
