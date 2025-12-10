#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "pch.h"

namespace Files
{
	std::vector<std::filesystem::directory_entry> GetPluginFiles(std::string path);
}
