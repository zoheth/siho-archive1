#pragma once

#include "RendererContext.h"
#include "RenderCommandQueue.h"
#include "RendererConfig.h"
#include "Siho/Core/RenderThread.h"

namespace Siho {

	class ShaderLibrary;

	class Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		static Ref<RendererContext> GetContext()
		{
			return Application::Get().GetWindow().GetRenderContext();
		}

		static void Init();
		static void Shutdown();

		static Ref<ShaderLibrary> GetShaderLibrary();

		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) {
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();

				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();
				};
			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		template<typename FuncT>
		static void SubmitResourceFree(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) {
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();

				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();
				};

			Submit([renderCmd, func]()
				{
					const uint32_t index = Renderer::RT_GetCurrentFrameIndex();
					auto storageBuffer = GetRenderResourceReleaseQueue(index).Allocate(renderCmd, sizeof(func));
					new (storageBuffer) FuncT(std::forward<FuncT>((FuncT&&)func));
				});
		}

		static void WaitAndRender(RenderThread* renderThread);
		static void SwapQueues();

		static void RenderThreadFunc(RenderThread* renderThread);

		static void BeginFrame();
		static void EndFrame();

		static RendererConfig& GetConfig();

		static uint32_t GetCurrentFrameIndex();
		static uint32_t RT_GetCurrentFrameIndex();

		static RenderCommandQueue& GetRenderResourceReleaseQueue(uint32_t index);

		// Add known macro from shader
		static const std::unordered_map<std::string, std::string>& GetGlobalShaderMacros();


	private:
		static RenderCommandQueue& GetRenderCommandQueue();
	};
}