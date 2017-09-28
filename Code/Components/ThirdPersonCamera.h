#pragma once
#include "UnifiedCamera.h"

class CThirdPersonCamera final : public IUnifiedCamera
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{66230D23-C35E-47CF-8737-CCBBFEB8AFD0}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CThirdPersonCamera>& desc)
	{
		desc.AddBase<IUnifiedCamera>();
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("ThirdPersonCamera");
		desc.SetDescription("Does stuff");
		desc.SetIcon("icons:ObjectTypes/Camera.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });
		desc.AddMember(&CThirdPersonCamera::m_name, 'name', "Name", "Name", "Name for the component", "");
	}

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void OnShutDown() override;
	// IEntityComponent
public:
	void SetYawPitchRoll(Vec3 orient_) { YawPitchRoll = orient_; };
	void SetDistance(float distance_) { Distance = distance_; };
	void SetTarget(IEntity* target_) { Target = target_; };
	void SetTargetOffset(Vec3 offset) { m_TargetOffset = offset; };
	Vec3 GetPlaneForward();
	void UpdateDistanceToTarget(float& wheelMove, float frameTime);
	void UpdateYaw(float mouseDeltaRotationX, float frameTime);

	Vec3 YawPitchRoll = Vec3(DEG2RAD(0.0f), DEG2RAD(-15.0f), 0.0); // target observe angle 
	float Distance = 5.0f;
	
	float m_maxDistance = 5;
	float m_minDistance = 1.0f;

	Matrix33 RotationMatrix;
	Quat Rotation;
	IEntity* Target = nullptr;
	IEntity* blendCamera = nullptr;
	Vec3 m_TargetOffset = Vec3(0.0f);

	IEntity* forwardCamera = nullptr;
	Matrix33 planeRotationMatrix;
	Quat planeRotation;

protected:

	Schematyc::CSharedString m_name = "ThirdPersonCamera";
};