#pragma once
#ifndef _WIN_32_BACKEND_
#define _WIND_32_BACKEND_

#include <mbgl/util/size.hpp>
#include <mbgl/gfx/backend.hpp>

namespace mbgl {
	namespace gfx {
		class RendererBackend;
	} // namespace gfx
} // namespace mbgl
namespace dragon
{
    class MapRenderCanvas; 
}
class Win32View; 
namespace editor
{
	class Win32Backend
	{
    public:
        explicit Win32Backend() = default;
        Win32Backend(const Win32Backend&) = delete;
        Win32Backend& operator=(const Win32Backend&) = delete;
        virtual ~Win32Backend() = default;

        static std::unique_ptr<Win32Backend> Create(dragon::MapRenderCanvas* window, bool capFrameRate) {
            return mbgl::gfx::Backend::Create<Win32Backend, dragon::MapRenderCanvas*, bool>(window, capFrameRate);
        }

        virtual mbgl::gfx::RendererBackend& getRendererBackend() = 0;
        virtual mbgl::Size getSize() const = 0;
        virtual void setSize(mbgl::Size) = 0;
	};
}
#endif // !_WIN_32_BACKEND_



