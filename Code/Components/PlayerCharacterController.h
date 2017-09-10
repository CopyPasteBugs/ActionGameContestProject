#pragma once
#include "UnifiedCharacterController.h"

class CPlayerCharacterController final : public IUnifiedCharacterController
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{5A016269-F951-4D44-81D5-8F019B92E4E3}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CPlayerCharacterController>& desc)
	{
		desc.AddBase<IUnifiedCharacterController>();
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("PlayerCharacterController");
		desc.SetDescription("Does stuff");
		desc.SetIcon("icons:ObjectTypes/light.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });

		desc.AddMember(&CPlayerCharacterController::m_name, 'name', "Name", "Name", "Name for the component", "");
	}

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void OnShutDown() override;
	// IEntityComponent

protected:

	Schematyc::CSharedString m_name = "Player";
};