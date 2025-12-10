#include "NpcProcessor.h"
#include "ExceptionManager.h"

void ProcessPCLevelMultNPC(RE::TESNPC* npc)
{
    npc->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kPCLevelMult);
 
    auto calcLevelMin = npc->actorData.calcLevelMin;
    auto calcLevelMax = npc->actorData.calcLevelMax;

    uint16_t newLevel = (calcLevelMin + calcLevelMax) / 2;
    
    npc->actorData.calcLevelMin = 0;
    npc->actorData.calcLevelMax = 0;

    if (calcLevelMin <= newLevel)
    {
        npc->actorData.level = newLevel;
    }
    else
    {
        npc->actorData.level = calcLevelMin;
        // REX::INFO(std::format("NPC [{0}] has level {1}.", npc->GetFullName(), calcLevelMin));
    }
}

void ProcessFixedNPC(RE::TESNPC* npc)
{
    auto calcLevelMin = npc->actorData.calcLevelMin;
    auto calcLevelMax = npc->actorData.calcLevelMax;

    if (calcLevelMin == 0 && calcLevelMax == 0)
    {
        return;
    }

    npc->actorData.calcLevelMin = 0;
    npc->actorData.calcLevelMax = 0;

    uint16_t newLevel = (calcLevelMin + calcLevelMax) / 2;

    if (npc->actorData.level >= newLevel)
    {
        return;
    }
    
    npc->actorData.level = newLevel;

    // REX::INFO(std::format("NPC [{0}] has level {1}.", npc->GetFullName(), newLevel));
}

void NpcProcessor::ProcessAll()
{
    auto dataHandler = RE::TESDataHandler::GetSingleton();
    if (dataHandler == NULL)
    {
        return;
    }

    for (auto currentNPC : dataHandler->GetFormArray<RE::TESNPC>())
    {
        if (currentNPC == NULL || ExceptionManager::Contains(currentNPC))
        {
            continue;
        }

        if (currentNPC->HasPCLevelMult())
        {
            ProcessPCLevelMultNPC(currentNPC);
        }
        else
        {
            ProcessFixedNPC(currentNPC);
        }
    }
}