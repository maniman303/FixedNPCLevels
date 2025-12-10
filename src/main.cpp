#include "NpcProcessor.h"
#include "ExceptionManager.h"
#include "OverridesProcessor.h"

bool InitModCore(const F4SE::QueryInterface* a_f4se)
{
	if (a_f4se->IsEditor()) {
		REX::ERROR("loaded in editor");
		return false;
	}

	const auto ver = a_f4se->RuntimeVersion();
	if (ver < F4SE::RUNTIME_1_11_159) {
		auto verString = ver.string();
		REX::ERROR(std::format("unsupported runtime v{}", verString));
		return false;
	}

	return true;
}

void OnMessage(F4SE::MessagingInterface::Message* message)
{
	if (message->type == F4SE::MessagingInterface::kGameDataReady)
	{
		OverridesProcessor::ProcessAll();

		ExceptionManager::Load();

		NpcProcessor::ProcessAll();

		ExceptionManager::Clear();

		REX::INFO("Finished pre-game processing.");
	}
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se);

	InitModCore(a_f4se);

	F4SE::GetMessagingInterface()->RegisterListener(OnMessage);

	REX::INFO("Finished initialization.");

	return true;
}
