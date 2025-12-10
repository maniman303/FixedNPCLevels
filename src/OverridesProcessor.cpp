#include <json/json.h>
#include "OverridesProcessor.h"
#include "Files.h"
#include "FormUtil.h"

struct OverrideModel {
    uint32_t npcId = 0;
    std::optional<int> health;
    std::optional<int> level;
};

std::optional<OverrideModel> DecodeModel(Json::Value value)
{
    auto npcIdString = value["Npc"];
    auto npcId = FormUtil::GetFormIdFromJson(npcIdString);
    if (npcId == 0)
    {
        return std::optional<OverrideModel>();
    }

    OverrideModel model;
    model.npcId = npcId;

    auto health = value["Health"];
    if (!health.empty() && health.isInt())
    {
        model.health = health.asInt();
    }

    auto level = value["Level"];
    if (!level.empty() && level.isInt())
    {
        model.level = level.asInt();
    }

    std::optional<OverrideModel> res;
    res = model;

    return res;
}

void ProcessModel(OverrideModel model)
{
    auto npc = FormUtil::GetFormAs<RE::TESNPC>(model.npcId);
    if (npc == NULL)
    {
        return;
    }

    if (model.level.has_value())
    {
        auto level = static_cast<uint16_t>(model.level.value());
        npc->actorData.level = level;
        npc->actorData.calcLevelMin = level;
        npc->actorData.calcLevelMax = level;
    }

    if (model.health.has_value())
    {
        auto healthValueId = FormUtil::GetFormId("Fallout4.esm", 0x2D4);
        auto healthValue = RE::TESForm::GetFormByID(healthValueId);

        if (healthValue != NULL)
        {
            for (auto tuple : *npc->properties)
            {
                if (tuple.first == healthValue)
                {
                    tuple.second.f = static_cast<float>(model.health.value());

                    break;
                }
            }
        }
    }
}

int ProcessFile(std::filesystem::directory_entry entry)
{
    auto& path = entry.path();

    Json::Value overrideJson;
    std::ifstream overrideFile;
    
    try
    {
        overrideFile.open(path);
        overrideFile >> overrideJson;
        overrideFile.close();
    }
    catch (...)
    {
        REX::ERROR(std::format("Invalid json '{0}'.", path.string()));
        return 0;
    }

    if (!overrideJson.isArray())
    {
        REX::ERROR(std::format("Json '{0}' is not an array.", path.string()));
        return 0;
    }

    int count = 0;

    for (auto value: overrideJson)
    {
        auto model = DecodeModel(value);
        if (!model.has_value())
        {
            continue;
        }

        ProcessModel(model.value());

        count++;
    }

    return count;
}

void OverridesProcessor::ProcessAll()
{
    auto files = Files::GetPluginFiles("Overrides");

    REX::INFO(std::format("Retrieved {0} override files to process.", files.size()));

    int count = 0;

    for (auto& file : files)
    {
        count += ProcessFile(file);
    }

    REX::INFO(std::format("Processed {0} overrides.", count));
}