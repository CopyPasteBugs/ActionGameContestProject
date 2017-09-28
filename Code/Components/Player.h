#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryMath/Cry_Camera.h>

#include <ICryMannequin.h>

#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>

class CThirdPersonCamera;
class CPlayerCharacterController;
class IUnifiedCharacterAnimation;
class CFootstepQueue;

////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
class CPlayerComponent final : public IEntityComponent
{
	enum class EInputFlagType
	{
		Hold = 0,
		Toggle
	};

	typedef uint8 TInputFlags;

	enum class EInputFlag
		: TInputFlags
	{
		MoveLeft = 1 << 0,
		MoveRight = 1 << 1,
		MoveForward = 1 << 2,
		MoveBack = 1 << 3
	};
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{A1BEFE71-5DC3-47E3-AC3A-05F773B39F8E}"_cry_guid;
		return id;
	}
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("Player");
		desc.SetDescription("A main game player");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
	}

public:
	CPlayerComponent() = default;
	virtual ~CPlayerComponent() {}

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void OnShutDown() override;
	// ~IEntityComponent

	void InitializeAnimation();
	void UpdateCharacterContoller(float frameTime);
	void UpdateAnimation(float frameTime);
	void Revive();
	void FindAttachments(ICharacterInstance* pCharacter);
	bool GetTerrainHeight(Vec3 pos, Vec3& terrainHitPos);
	void RaycastFootstepDown();

	const int32 characterSlot = 1;
protected:
	void HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type = EInputFlagType::Hold);

protected:
	//Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAnimationComponent = nullptr;
	IUnifiedCharacterAnimation* m_pAnimationComponent = nullptr;
	FragmentID m_forwardFragmentId;
	FragmentID m_idleFragmentId;
	FragmentID m_walkFragmentId;
	TagID m_rotateTagId;

	CPlayerCharacterController* m_pCharacterController = nullptr;
	CThirdPersonCamera* m_pCameraComponent = nullptr;
	IEntity* m_pEntityHelperCamera = nullptr;
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;

	TInputFlags m_inputFlags;
	Vec2 m_mouseDeltaRotation;
	float m_mouseWheel = 0.0f;

	IAttachment *pFootStepLeft = nullptr;
	IAttachment *pFootStepRight = nullptr;
	int FootStepLeftState = 0;
	int FootStepRightState = 0;

	CFootstepQueue* m_pFootstepQueue = nullptr;

};
