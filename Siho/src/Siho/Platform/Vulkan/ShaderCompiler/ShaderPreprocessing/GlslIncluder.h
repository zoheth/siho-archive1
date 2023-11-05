#pragma once

#include <unordered_set>
#include <unordered_map>
#include <shaderc/shaderc.hpp>
#include <libshaderc_util/file_finder.h>


namespace Siho {
	struct IncludeData;
	struct HeaderCache;

	class GlslIncluder : public shaderc::CompileOptions::IncluderInterface
	{
	public:
		explicit GlslIncluder(const shaderc_util::FileFinder* file_finder);

		~GlslIncluder() override;

		shaderc_include_result* GetInclude(
			const char* requestedPath,
			shaderc_include_type type,
			const char* requestingPath,
			size_t includeDepth) override;

		void ReleaseInclude(shaderc_include_result* data) override;

		std::unordered_set<IncludeData>&& GetIncludeData() { return std::move(m_IncludeData); }
		std::unordered_set<std::string>&& GetParseSpecialMacros() { return std::move(m_ParseSpecialMacros); }

	private:
		const shaderc_util::FileFinder* m_FileFinder;
		std::unordered_set<IncludeData> m_IncludeData;
		std::unordered_set<std::string> m_ParseSpecialMacros;
		std::unordered_map<std::string, HeaderCache> m_HeaderCache;
	};
}