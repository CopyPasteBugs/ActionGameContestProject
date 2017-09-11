#pragma once
#include "UnifiedCharacterController.h"

class CEnemyCharacterController final : public IUnifiedCharacterController
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{58015B0D-4D35-4F68-A93E-7EA458E3F916}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CEnemyCharacterController>& desc)
	{
		desc.AddBase<IUnifiedCharacterController>();
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("EnemyCharacterController");
		desc.SetDescription("Does stuff");
		desc.SetIcon("icons:ObjectTypes/Core.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });
		desc.AddMember(&CEnemyCharacterController::m_name, 'name', "Name", "Name", "Name for the component", "");
	}

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void OnShutDown() override;
	// IEntityComponent

protected:

	Schematyc::CSharedString m_name = "Enemy";
};