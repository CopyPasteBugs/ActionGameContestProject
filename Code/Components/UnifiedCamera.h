#pragma once

class IUnifiedCamera :
	public IEntityComponent
#ifndef RELEASE
	, public IEntityComponentPreviewer
#endif
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{06B9B4FD-F57D-4A04-A4DC-7A8A83B923E1}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<IUnifiedCamera>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("UnifiedCamera");
		desc.SetDescription("Represents a camera that can be activated to render to screen");
		desc.SetIcon("icons:General/Camera.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&IUnifiedCamera::m_bActivateOnCreate, 'actv', "Active", "Active", "Whether or not this camera should be activated on component creation", true);
		desc.AddMember(&IUnifiedCamera::m_nearPlane, 'near', "NearPlane", "Near Plane", nullptr, 0.25f);
		desc.AddMember(&IUnifiedCamera::m_fieldOfView, 'fov', "FieldOfView", "Field of View", nullptr, 75.0_degrees);
	}

	IUnifiedCamera() = default;
	virtual ~IUnifiedCamera();

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

public:

	virtual void Activate();
	bool IsActive() const { return s_pActiveCamera == this; }
	
	virtual void EnableAutomaticActivation(bool bActivate) { m_bActivateOnCreate = bActivate; }
	bool IsAutomaticallyActivated() const { return m_bActivateOnCreate; }

	virtual void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
	float GetNearPlane() const { return m_nearPlane; }

	virtual void SetFieldOfView(CryTransform::CAngle angle) { m_fieldOfView = angle; }
	CryTransform::CAngle GetFieldOfView() const { return m_fieldOfView; }

	virtual CCamera& GetCamera() { return m_camera; }
	const CCamera&   GetCamera() const { return m_camera; }

protected:
	bool m_bActivateOnCreate = true;
	Schematyc::Range<0, 32768> m_nearPlane = 0.25f;
	CryTransform::CClampedAngle<20, 360> m_fieldOfView = 75.0_degrees;

	CCamera m_camera;

	static IUnifiedCamera* s_pActiveCamera;

};