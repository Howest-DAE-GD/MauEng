#ifndef MAUREN_VULKANSURFACECONTEXT_H
#define MAUREN_VULKANSURFACECONTEXT_H

#include "RendererPCH.h"
#include "VulkanInstanceContext.h"
namespace MauRen
{
	class VulkanSurfaceContext final
	{
	public:
		VulkanSurfaceContext() = default;
		~VulkanSurfaceContext() = default;

		void Initialize(VulkanInstanceContext* pVulkanInstanceContext, SDL_Window* pWindow)
		{
			m_pVulkanInstanceContext = pVulkanInstanceContext;

			if (not SDL_Vulkan_CreateSurface(pWindow, m_pVulkanInstanceContext->GetInstance(), nullptr, &m_WindowSurface))
			{
				throw std::runtime_error("Failed to create Vulkan window surface!");
			}
			//if (glfwCreateWindowSurface(m_pVulkanInstanceContext->GetInstance(), pWindow, nullptr, &m_WindowSurface) != VK_SUCCESS)
			//{
			//	throw std::runtime_error("Failed to create window surface!");
			//}
		}

		void Destroy()
		{
			if (m_WindowSurface == VK_NULL_HANDLE)
			{
				return;
			}
			vkDestroySurfaceKHR(m_pVulkanInstanceContext->GetInstance(), m_WindowSurface, nullptr);
		}

		VulkanSurfaceContext(VulkanSurfaceContext const&) = delete;
		VulkanSurfaceContext(VulkanSurfaceContext&&) = delete;
		VulkanSurfaceContext& operator=(VulkanSurfaceContext const&) = delete;
		VulkanSurfaceContext& operator=(VulkanSurfaceContext&&) = delete;


		[[nodiscard]] VkSurfaceKHR GetWindowSurface() const noexcept { return m_WindowSurface; }

	private:
		VulkanInstanceContext* m_pVulkanInstanceContext;
		VkSurfaceKHR m_WindowSurface;
	};
}

#endif // MAUREN_VULKANSURFACECONTEXT_H