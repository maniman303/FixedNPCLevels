#include <json/json.h>
#include <unordered_set>
#include "ExceptionManager.h"
#include "Files.h"
#include "FormUtil.h"

std::unordered_set<uint32_t> exceptionCache;

void ExceptionManager::Clear()
{
    exceptionCache.clear();
}

bool ExceptionManager::Contains(RE::TESForm* form)
{
    if (form == NULL)
    {
        return false;
    }

    return exceptionCache.contains(form->GetFormID());
}

int LoadFromFile(std::filesystem::directory_entry entry)
{
    auto& path = entry.path();

    Json::Value exceptionJson;
    std::ifstream exceptionFile;
    
    try
    {
        exceptionFile.open(path);
        exceptionFile >> exceptionJson;
        exceptionFile.close();
    }
    catch (...)
    {
        REX::ERROR(std::format("Invalid json '{0}'.", path.string()));
        return 0;
    }

    auto formIds = FormUtil::GetFormIdsFromJson(exceptionJson);
    int count = 0;

    for (auto formId : formIds)
    {
        if (formId == 0)
        {
            continue;
        }

        count++;
        exceptionCache.insert(formId);
    }

    return count;
}

void ExceptionManager::Load()
{
    exceptionCache.clear();

    auto files = Files::GetPluginFiles("Exceptions");

    REX::INFO(std::format("Retrieved {0} exception files to process.", files.size()));

    int count = 0;

    for (auto& file : files)
    {
        count +=  LoadFromFile(file);
    }

    REX::INFO(std::format("Exempted {0} NPCs.", count));
}