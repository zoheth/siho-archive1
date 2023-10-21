#include "shpch.h"
#include "GlslIncluder.h"

#include "Siho/Utilities/StringUtils.h"
#include "ShaderPreprocessor.h"
#include "Siho/Core/Hash.h"

#include <filesystem>

namespace Siho {

	GlslIncluder::GlslIncluder(const shaderc_util::FileFinder* file_finder)
		: m_FileFinder(file_finder)
	{
	}
	
	GlslIncluder::~GlslIncluder() = default;


	shaderc_include_result* GlslIncluder::GetInclude(const char* requestedPath, shaderc_include_type type, const char* requestingPath, size_t includeDepth)
	{
		const std::filesystem::path requestedFullPath = (type == shaderc_include_type_relative)
			? m_FileFinder->FindRelativeReadableFilepath(requestingPath, requestedPath)
			: m_FileFinder->FindReadableFilepath(requestedPath);

		auto& [source, sourceHash, stages, isGuarded] = m_HeaderCache[requestedFullPath.string()];

		if (source.empty())
		{
			source = Utils::ReadFileAndSkipBOM(requestedFullPath);
			if (source.empty())
			{
				SH_CORE_ERROR("[Renderer] Failed to read shader include file: {} in {}.", requestedFullPath, requestingPath);
			}
			sourceHash = Hash::GenerateFNVHash(source.c_str());

			// UNDONE 1021
			// stages = ShaderPreprocessor::
		}
	}

}