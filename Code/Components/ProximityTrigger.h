#pragma once


class CProximitryTrigger : 
	public IEntityComponent
#ifndef RELEASE
	,public IEntityComponentPreviewer
#endif
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{E28EBAAA-9394-42A3-8201-2AC7E30485F1}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CProximitryTrigger>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("ProximitryTrigger");
		desc.SetDescription("ProximitryTrigger");
		desc.SetIcon("icons:General/Core.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly });

		desc.AddMember(&CProximitryTrigger::m_size, 'size', "Size", "Size", nullptr, 0.0f);
	}

	CProximitryTrigger() = default;
	virtual ~CProximitryTrigger();

	// IEntityComponent
	virtual void Initialize();
	virtual void ProcessEvent(SEntityEvent& event);
	virtual uint64 GetEventMask() const;

#ifndef RELEASE
	virtual IEntityComponentPreviewer* GetPreviewer() final { return this; }
	// ~IEntityComponent

	// IEntityComponentPreviewer
	virtual void SerializeProperties(Serialization::IArchive& archive) final {}

	virtual void Render(const IEntity& entity, const IEntityComponent& component, SEntityPreviewContext &context) const final;
	// ~IEntityComponentPreviewer
#endif

protected:

	const AABB GetWorldAABB();
	const AABB GetLocalAABB();

	IEntityTriggerComponent* pDistanceChecker;
	float m_size = 2.0f;
	Vec3 m_min = Vec3(-m_size * 0.5f);
	Vec3 m_max = Vec3(m_size * 0.5f);
};