#include "shpch.h"
#include "StringUtils.h"

#include <filesystem>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <regex>

namespace Siho::Utils {

	int SkipBOM(std::istream& in)
	{
		char test[4] = { 0 };
		in.seekg(0, std::ios::beg);
		in.read(test, 3);
		if (strcmp(test, "\xEF\xBB\xBF") == 0)
		{
			in.seekg(3, std::ios::beg);
			return 3;
		}
		in.seekg(0, std::ios::beg);
		return 0;
	}

	// Returns an empty string when failing.
	std::string ReadFileAndSkipBOM(const std::filesystem::path& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			auto fileSize = in.tellg();
			const int skippedChars = SkipBOM(in);

			fileSize -= skippedChars - 1;
			result.resize(fileSize);
			in.read(result.data() + 1, fileSize);
			// Add a dummy tab to beginning of file.
			result[0] = '\t';
		}
		in.close();
		return result;
	}

	std::vector<std::string> SplitStringAndKeepDelims(std::string str)
	{
		const static std::regex re(R"((^\W|^\w+)|(\w+)|[:()])", std::regex_constants::optimize);

		std::regex_iterator<std::string::iterator> rit(str.begin(), str.end(), re);
		std::regex_iterator<std::string::iterator> rend;
		std::vector<std::string> result;

		while (rit != rend)
		{
			result.emplace_back(rit->str());
			++rit;
		}
		return result;
	}

}