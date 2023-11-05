#pragma once

#include <filesystem>
#include <sstream>
#include <map>
#include <unordered_set>

#include "Siho/Utilities/StringUtils.h"
#include "Siho/Platform/Vulkan/VulkanShaderUtils.h"

enum VkShaderStageFlagBits;

namespace Siho {
	namespace PreprocessUtils {
		// From https://wandbox.org/permlink/iXC7DWaU8Tk8jrf3 and is modified.
		enum class State : char { SlashOC, StarIC, SingleLineComment, MultiLineComment, NotAComment };

		template <typename InputIt, typename OutputIt>
		void CopyWithoutComments(InputIt first, InputIt last, OutputIt out)
		{
			State state = State::NotAComment;

			while (first != last)
			{
				switch (state)
				{
				case State::SlashOC:
					if (*first == '/') state = State::SingleLineComment;
					else if (*first == '*') state = State::MultiLineComment;
					else
					{
						state = State::NotAComment;
						*out++ = '/';
						*out++ = *first;
					}
					break;
				case State::StarIC:
					if (*first == '/') state = State::NotAComment;
					else state = State::MultiLineComment;
					break;
				case State::NotAComment:
					if (*first == '/') state = State::SlashOC;
					else *out++ = *first;
					break;
				case State::SingleLineComment:
					if (*first == '\n')
					{
						state = State::NotAComment;
						*out++ = '\n';
					}
					break;
				case State::MultiLineComment:
					if (*first == '*') state = State::StarIC;
					else if (*first == '\n') *out++ = '\n';
					break;
				}
				++first;
			}
		}
	}
	struct IncludeData
	{
		std::filesystem::path IncludedFilePath{};
		size_t IncludeDepth{ 0 };
		bool IsRelative{ false };
		bool IsGuarded{ false };
		uint32_t HashValue{ 0 };

		VkShaderStageFlagBits ShaderStage {};

		inline bool operator==(const IncludeData& rhs) const noexcept
		{
			return IncludedFilePath == rhs.IncludedFilePath && HashValue == rhs.HashValue;
		}
	};

	struct HeaderCache
	{
		std::string Source;
		uint32_t SourceHash;
		VkShaderStageFlagBits Stages;
		bool IsGuarded;
	};
}

namespace Siho {

	class ShaderPreprocessor
	{
	public:
	/**
	 * Preprocesses a given GLSL shader source to identify and separate out individual shader stages and special macros.
	 *
	 * This function first removes comments from the provided shader source. Then, it parses the preprocessed shader source
	 * to identify shader stages (like vertex, fragment, and compute) marked by "#pragma stage" directives and other special
	 * macros starting with "__SH_". Each identified stage is separated and stored in a map keyed by the corresponding Vulkan
	 * shader stage flag. Additionally, special macros are added to a set for further use or analysis.
	 *
	 * Note: The function assumes that the shader source uses "#pragma stage" to denote different shader stages and the shader
	 * stages are presented in order of their declaration. It also assumes that the "#version" directive appears at the start
	 * of each stage.
	 *
	 * @param source        The raw GLSL shader source code string.
	 * @param specialMacros An unordered set to store discovered special macros (those starting with "__SH_").
	 *
	 * @return A map from Vulkan shader stage flags to the corresponding preprocessed shader stage source strings.
	 */
		static std::map<VkShaderStageFlagBits, std::string> PreprocessShader(const std::string& source, std::unordered_set<std::string>& specialMacros);
	};

	std::map<VkShaderStageFlagBits, std::string> ShaderPreprocessor::PreprocessShader(const std::string& source, std::unordered_set<std::string>& specialMacros)
	{
		std::stringstream sourceStream;
		PreprocessUtils::CopyWithoutComments(source.begin(), source.end(), std::ostream_iterator<char>(sourceStream));
		std::string newSource = sourceStream.str();

		std::map<VkShaderStageFlagBits, std::string> shaderSources;
		std::vector<std::pair<VkShaderStageFlagBits, size_t>> stagePositions;
		SH_CORE_ASSERT(!newSource.empty(), "Shader source is empty!");

		size_t startOfStage = 0;
		size_t pos = newSource.find("#");

		// Check first #version
		const size_t endOfLine = newSource.find_first_of("\r\n", pos) + 1;
		const std::vector<std::string> tokens = Utils::SplitStringAndKeepDelims(newSource.substr(pos, endOfLine - pos));
		SH_CORE_ASSERT(tokens.size() >= 3, "Invalid #version statement!");
		pos = newSource.find('#', pos + 1);

		while (pos != std::string::npos)
		{
			const size_t endOfLine = newSource.find_first_of("\r\n", pos) + 1;
			const std::vector<std::string> tokens = Utils::SplitStringAndKeepDelims(newSource.substr(pos, endOfLine - pos));
			
			size_t index = 1; // Skip #

			if (tokens[index] == "pragma") // Parse stage. example: #pragma stage : vert
			{
				++index;
				if (tokens[index] == "stage")
				{
					++index;
					SH_CORE_ASSERT(tokens[index] == ":", "Stage pragma is invalid!");
					++index;

					const std::string_view stage = tokens[index];
					SH_CORE_ASSERT(stage == "vert" || stage == "frag" || stage == "comp", "Invalid shader type specified");
					auto shaderStage = ShaderUtils::ShaderTypeFromString(stage);

					stagePositions.emplace_back(shaderStage, startOfStage);
				}
			}
			else if (tokens[index] == "ifdef")
			{
				++index;
				if (tokens[index].rfind("__SH_", 0) == 0) // Siho special macros start with "__SH_"
				{
					specialMacros.emplace(tokens[index]);
				}

			}
			else if (tokens[index] == "if" || tokens[index] == "define")
			{
				++index;
				for (size_t i = index; i < tokens.size(); ++i)
				{
					if(tokens[i].rfind("__SH_", 0) == 0) // Siho special macros start with "__SH_"
					{
						specialMacros.emplace(tokens[i]);
					}
				}
			}
			else if (tokens[index] == "version")
			{
				++index;
				startOfStage = pos;
			}
			pos = newSource.find('#', pos + 1);
		}

		SH_CORE_ASSERT(!stagePositions.empty(), "No shader stage specified!");
		auto& [firstStage, firstStart] = stagePositions[0];
		if (stagePositions.size() > 1)
		{
			// Get first stage
			const std::string firstStageSource = newSource.substr(firstStart, stagePositions[1].second - firstStart);
			size_t lineCount = std::count(firstStageSource.begin(), firstStageSource.end(), '\n') + 1;
			shaderSources[firstStage] = firstStageSource;

			// Get stages in the middle
			for (size_t i = 1; i < stagePositions.size() - 1; ++i)
			{
				auto& [stage, start] = stagePositions[i];
				std::string stageSource = newSource.substr(start, stagePositions[i + 1].second - start);
				const size_t secondLinePose = stageSource.find_first_of('\n', 1) + 1;
				stageSource.insert(secondLinePose, fmt::format("#line {}\n", lineCount));
				shaderSources[stage] = stageSource;
				lineCount += std::count(stageSource.begin(), stageSource.end(), '\n') + 1;
			}

			// Get last stage
			auto& [lastStage, lastStart] = stagePositions[stagePositions.size() - 1];
			std::string lastStageSource = newSource.substr(lastStart);
			const size_t secondLinePose = lastStageSource.find_first_of('\n', 1) + 1;
			lastStageSource.insert(secondLinePose, fmt::format("#line {}\n", lineCount + 1));
			shaderSources[lastStage] = lastStageSource;
		}
		else
		{
			shaderSources[firstStage] = newSource.substr(firstStart);
		}

		return shaderSources;
	}

}
