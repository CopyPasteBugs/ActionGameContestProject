#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryMath/Cry_Camera.h>

#include <ICryMannequin.h>

#include <DefaultComponents/Input/InputComponent.h>



class CThirdPersonCamera;
class CPlayerCharacterController;

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

	void UpdateCharacterContoller(float frameTime);
	void Revive();

protected:
	void HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type = EInputFlagType::Hold);

protected:
	CPlayerCharacterController* m_pCharacterController = nullptr;
	CThirdPersonCamera* m_pCameraComponent = nullptr;
	IEntity* m_pEntityHelperCamera = nullptr;
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;

	TInputFlags m_inputFlags;
	Vec2 m_mouseDeltaRotation;
	float m_mouseWheel = 0.0f;
};
