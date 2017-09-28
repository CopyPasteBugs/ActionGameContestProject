#pragma once

struct SSpawnedFootstep
{
	IEntity* entity;
	bool active;
	float elapsed;
};

class CFootstepQueue : public IEntityComponent
{
public:
	const int NFOOTSTEP = -1;
	const int PE_SLOT = 1;

	static CryGUID& IID()
	{
		static CryGUID id = "{6C540828-4E02-46D0-B2A8-6AF9CA7E7BAE}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CFootstepQueue>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("FootstepQueue");
		desc.SetDescription("");
		desc.SetIcon("icons:General/Core.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });

	}

	CFootstepQueue() = default;
	virtual ~CFootstepQueue();

	// IEntityComponent
	virtual void Initialize();
	virtual void ProcessEvent(SEntityEvent& event);
	virtual uint64 GetEventMask() const;
	void ClearEventMask();

public:
	void QueueNewFootstep(Vec3 pos);
	int GetFirstFreeFootstep();

	std::vector<SSpawnedFootstep> m_SpawnVec;
private:
	uint64 eventMask = BIT64(ENTITY_EVENT_UPDATE);
};