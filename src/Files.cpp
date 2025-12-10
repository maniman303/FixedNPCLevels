#include "Files.h"

namespace Files
{
	std::filesystem::path GetRootPath()
	{
		auto workingDir = std::filesystem::current_path();
		workingDir.append("Data");
		workingDir.append("F4SE");

		return workingDir;
	}

	std::filesystem::path GetPluginPath()
	{
		auto rootDir = Files::GetRootPath();
		rootDir.append("Plugins");
		rootDir.append("FixedNPCLevels");

		return rootDir;
	}

	std::vector<std::filesystem::directory_entry> GetPluginFiles(std::string path)
	{
		std::vector<std::filesystem::directory_entry> result;

		auto pluginPath = Files::GetPluginPath().append(path);

		if (!std::filesystem::exists(pluginPath) || pluginPath.empty()) {
			REX::WARN("Specified folder does not exist.");
			return result;
		}

		for (auto& entry : std::filesystem::directory_iterator(pluginPath)) {
			if (entry.is_directory()) {
				continue;
			}

			auto pathToFile = entry.path();

			if (StringHelper::ToLower(pathToFile.extension().string()) != ".json") {
				continue;
			}

			if (StringHelper::StartsWith(StringHelper::ToLower(pathToFile.filename().string()), "_")) {
				continue;
			}

			result.push_back(entry);
		}

		return result;
	}
}
