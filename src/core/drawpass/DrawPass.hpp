#pragma once
#ifndef _DRAWPASS_HPP_
#define _DRAWPASS_HPP_
#include <memory>
namespace threepp
{
	class GLRenderTarget;
	class Scene;
	class GLRenderer;
	class Camera;
	class RawShaderMaterial;
	class BufferGeometry;
	class Mesh;
	class Texture;
	class Vector2;
	class WindowSize;
}
namespace dragon
{
	class DrawPass
	{
	public:
		DrawPass(const int& width, const int& height);
		virtual ~DrawPass();
	public:
		void setSize(const int& width, const int& height);
		threepp::Scene* getScene();
		virtual void clear() = 0;
	public:
		virtual void render(threepp::GLRenderer* renderer, threepp::Camera* camera) = 0;
	protected:
		std::unique_ptr<threepp::Scene> m_Scene{ nullptr };
	};
}
#endif // !_DRAWPASS_HPP_