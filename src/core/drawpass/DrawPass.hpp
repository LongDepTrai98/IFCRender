#pragma once
#ifndef _DRAWPASS_HPP_
#define _DRAWPASS_HPP_
#include <memory>
namespace threepp
{
	class GLRenderTarget; 
	class Scene; 
}
namespace dragon
{
	class DrawPass
	{
	public: 
		DrawPass(const int& width, const int& height);
	public: 
		void setSize(const int& width, const int& height);
		threepp::GLRenderTarget* getRenderTarget();
		threepp::Scene* getScene(); 
	protected: 
		std::shared_ptr<threepp::GLRenderTarget> m_RenderTarget{ nullptr }; 
		std::unique_ptr<threepp::Scene> m_Scene{ nullptr }; 
	};
}
#endif // !_DRAWPASS_HPP_
