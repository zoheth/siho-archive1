#pragma once

namespace Siho::Utils {
	std::string ReadFileAndSkipBOM(const std::filesystem::path& filepath);

	// Keeps delimiters except for spaces, used for shaders
	std::vector<std::string> SplitStringAndKeepDelims(std::string str);
}