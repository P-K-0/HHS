
#include "actors.h"

#include "externals.h"
#include "settings.h"
#include "cache.h"

namespace Actors {

	void Utility::Init() noexcept
	{
		if (!actor || !actor->race) {
			return;
		}

		TESNPC* npc{ nullptr };

		if (!actor->baseForm || !(npc = DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC))) {
			return;
		}

		isFemale = CALL_MEMBER_FN(npc, GetSex)();

		auto& settings = Settings::Ini::GetSingleton();

		isPlayer = actor == *g_player;
		isEnabled = isPlayer ? settings.Get_bEnablePlayer() : settings.Get_bEnableNPCs();

		auto gender = settings.Get_iGender();

		if (isEnabled && gender != Settings::Gender::BothGender) {

			if (isFemale && gender == Settings::Gender::Male) {
				isEnabled = false;
			}

			if (!isFemale && gender == Settings::Gender::Female) {
				isEnabled = false;
			}
		}

		isRaceCompatible = settings.CheckRace(actor->race);
	}

	Utility::Utility(Actor* act) noexcept
		: actor{ act }
	{
		Init();
	}

	Utility::Utility(TESObjectREFR* refr) noexcept
	{
		actor = DYNAMIC_CAST(refr, TESObjectREFR, Actor);

		Init();
	}

	Utility::Utility(TESForm* frm) noexcept
	{
		actor = DYNAMIC_CAST(frm, TESForm, Actor);

		Init();
	}

	Utility::Utility(std::uint32_t id) noexcept
	{
		auto refr = LookupFormByID(id);

		if (!refr) {
			return;
		}

		actor = DYNAMIC_CAST(refr, TESObjectREFR, Actor);

		Init();
	}

	Utility& Utility::operator=(const Utility& util) noexcept
	{
		actor = util.actor;

		isFemale = util.isFemale;
		isEnabled = util.isEnabled;
		isPlayer = util.isPlayer;
		isRaceCompatible = util.isRaceCompatible;

		return *this;
	}

	Utility& Utility::operator=(Utility&& util) noexcept
	{
		actor = util.actor;

		util.actor = nullptr;

		isFemale = util.isFemale;
		isEnabled = util.isEnabled;
		isPlayer = util.isPlayer;
		isRaceCompatible = util.isRaceCompatible;

		return *this;
	}

	std::uint32_t Utility::GetIDFromScriptKwrd() noexcept
	{
		if (!actor || !actor->equipData) {
			return 0;
		}

		TESObjectARMO* armor{ nullptr };

		for (std::uint32_t slot = MinSlot; slot < MaxSlot; slot++) {

			const auto itemSwap = actor->equipData->slots[slot].item;

			if (!itemSwap || !(armor = DYNAMIC_CAST(itemSwap, TESForm, TESObjectARMO))) {
				continue;
			}
			
			for (std::uint32_t key = 0; key < armor->keywordForm.numKeywords; ++key) {

				BGSKeyword* kwrd = armor->keywordForm.keywords[key];

				if (kwrd && _strcmpi(kwrd->keyword.c_str(), "HHS_Script") == 0) {
					return itemSwap->formID;
				}
			}
		}

		const auto itemSwap = actor->equipData->slots[3].item;

		if (itemSwap) {
			return itemSwap->formID;	
		}

		return 0;
	}

	class BGSMod__Attachment__Mod;

	float Utility::GetHeightFromMod(std::uint32_t slot) noexcept
	{
		ExtraDataList* stackDataList{ nullptr };
		BSExtraData* extraData{ nullptr };
		TESForm* frm{ nullptr };
		float height{ ZeroValue };

		if (!actor || !actor->GetEquippedExtraData(slot, &stackDataList) || !stackDataList) {
			return ZeroValue;
		}

		if (!(extraData = stackDataList->GetByType(ExtraDataType::kExtraData_ObjectInstance))) {
			return ZeroValue;
		}

		BGSObjectInstanceExtra* objectModData = DYNAMIC_CAST(extraData, BSExtraData, BGSObjectInstanceExtra);

		if (!objectModData) {
			return ZeroValue;
		}

		const auto data = objectModData->data;

		if (!data || !data->forms) {
			return ZeroValue;
		}

		for (std::uint32_t i{}; i < data->blockSize / sizeof(BGSObjectInstanceExtra::Data::Form); i++) {

			if (!(frm = LookupFormByID(data->forms[i].formId))) {
				continue;
			}

			BGSMod::Attachment::Mod* objectMod = (BGSMod::Attachment::Mod*)DYNAMIC_CAST(frm, TESForm, BGSMod__Attachment__Mod);

			if (objectMod && (height = Cache::Map::GetSingleton().Find(objectMod->materialSwap.GetModelName())) != ZeroValue) {
				return height;
			}
		}

		return ZeroValue;
	}

	float Utility::GetHeightFromWornItem(std::uint32_t slot, std::uint32_t id, bool equipped) noexcept
	{
		ActorEquipData* equipdata{ nullptr };
		
		float height{ ZeroValue };

		if (!actor || !(equipdata = actor->equipData)) {
			return ZeroValue;
		}

		if (isPlayer && equipped && actor->middleProcess && actor->middleProcess->unk08) {	
			CALL_MEMBER_FN(actor->middleProcess, UpdateEquipment)(actor, 0);
		}

		for (std::uint32_t indexSlot = MinSlot; indexSlot < MaxSlot; indexSlot++) {

			if (!equipped && (slot & (1 << indexSlot)) && id) {
				continue;
			}

			const auto materialSwap = equipdata->slots[indexSlot].modelMatSwap;

			if (!Settings::Ini::GetSingleton().GetSlots()[indexSlot] || !materialSwap) {
				continue;
			}

			height = Cache::Map::GetSingleton().Find(materialSwap->GetModelName());
			
			if (height == ZeroValue) {
				height = GetHeightFromMod(indexSlot);
			}

			if (height != ZeroValue) {
				return height;
			}
		}

		return ZeroValue;
	}

	std::uint64_t Utility::GetHandle() noexcept
	{
		if (!actor || !(*g_gameVM)) {
			return 0;
		}

		auto registry = (*g_gameVM)->m_virtualMachine;

		if (!registry) {
			return 0;
		}

		auto policy = registry->GetHandlePolicy();

		if (policy) {
			return policy->Create(static_cast<UInt32>(actor->formType), (void*)actor);
		}

		return 0;
	}

	bool Utility::GetEquipData(const std::uint32_t& slot, std::uint32_t& id, std::string& filename) noexcept
	{
		ActorEquipData* actEquipData{ nullptr };

		if (!actor || !(actEquipData = actor->equipData)) {
			return false;
		}

		id = 0;
		filename.clear();

		const auto itemSwap = actEquipData->slots[slot].item;
		const auto materialSwap = actEquipData->slots[slot].modelMatSwap;

		if (!itemSwap || !materialSwap) {
			return false;
		}

		id = itemSwap->formID;
		filename = materialSwap->GetModelName();

		return true;
	}

	TESObjectREFR* Utility::GetFurnitureReference() noexcept
	{
#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
		TESObjectREFR* refr{ nullptr };
#else
		NiPointer<TESObjectREFR> refr{};
#endif

		if (!actor || !actor->middleProcess) {
			return nullptr;
		}

		auto data08 = actor->middleProcess->unk08;

		if (!data08) {
			return nullptr;
		}

		if (actor->actorState.flags & (ActorState::Flags::kUnk1 | ActorState::Flags::kUnk2)) {

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
			LookupREFRByHandle(&data08->furnitureHandle2, &refr);
#else
			LookupREFRByHandle(data08->furnitureHandle2, refr);
#endif

			return refr;
		}

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
		LookupREFRByHandle(&data08->furnitureHandle1, &refr);
#else
		LookupREFRByHandle(data08->furnitureHandle1, refr);
#endif

		return refr;
	}

	std::uint32_t GetSlotMaskByID(const std::uint32_t& id) noexcept
	{
		if (id == 0) {
			return InvalidSlot;
		}

		TESForm* ObjectFrm = LookupFormByID(id);

		if (!ObjectFrm || ObjectFrm->formType != kFormType_ARMO) {
			return InvalidSlot;
		}

		TESObjectARMO* armor = DYNAMIC_CAST(ObjectFrm, TESForm, TESObjectARMO);

		if (!armor) {
			return InvalidSlot;
		}

		return armor->bipedObject.GetSlotMask();
	}
}
