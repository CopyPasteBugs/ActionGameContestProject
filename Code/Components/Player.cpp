#include "StdAfx.h"
#include "Player.h"

#include <CryRenderer/IRenderAuxGeom.h>

#include "ThirdPersonCamera.h"
#include "PlayerCharacterController.h"

void CPlayerComponent::Initialize()
{
	//m_pEntity->SetUpdatePolicy(EEntityUpdatePolicy::ENTITY_UPDATE_ALWAYS);

	m_pCharacterController = m_pEntity->GetOrCreateComponent<CPlayerCharacterController>();
	
	// Create the camera component, will automatically update the viewport every frame
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.vPosition = Vec3(0.0f);
	spawnParams.qRotation = IDENTITY;
	spawnParams.vScale = Vec3Constants<float>::fVec3_One;

	if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		m_pCameraComponent = pEntity->CreateComponent<CThirdPersonCamera>();
		m_pCameraComponent->SetTarget(GetEntity());
	}
	
	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	// Register an action, and the callback that will be sent when it's triggered
	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveLeft, activationMode);  });
	// Bind the 'A' key the "moveleft" action
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, EKeyId::eKI_A);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveRight, activationMode);  });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, EKeyId::eKI_D);

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveForward, activationMode);  });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, EKeyId::eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveBack, activationMode);  });
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, EKeyId::eKI_S);

	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { m_mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { m_mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

	// mouse wheel
	m_pInputComponent->RegisterAction("player", "mouse_wheelDown", [this](int activationMode, float value) { m_mouseWheel = value; });
	m_pInputComponent->BindAction("player", "mouse_wheelDown", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelDown);

	m_pInputComponent->RegisterAction("player", "mouse_wheelUp", [this](int activationMode, float value) { m_mouseWheel = value; });
	m_pInputComponent->BindAction("player", "mouse_wheelUp", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelUp);

	Revive();
}

uint64 CPlayerComponent::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_UPDATE);
}

void CPlayerComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
		// Revive the entity when gameplay starts
		Revive();
	}
	break;
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		const float moveSpeed = 5.0f;
		Vec3 velocity = ZERO;

		// Check input to calculate local space velocity
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveLeft)
		{
			velocity.x -= moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveRight)
		{
			velocity.x += moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveForward)
		{
			velocity.y += moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveBack)
		{
			velocity.y -= moveSpeed * pCtx->fFrameTime;
		}

		// Camera component params update
		if (m_pCameraComponent)
		{
			if (fabs(m_mouseWheel) > 0)
			{
				m_pCameraComponent->Distance += m_mouseWheel * pCtx->fFrameTime;
				float clampDistance = CLAMP(m_pCameraComponent->Distance, 2, 10);
				m_pCameraComponent->SetDistance(clampDistance);
				m_mouseWheel = 0.0f;
			}


			const float rotSpeed = 0.1f;
			m_pCameraComponent->YawPitchRoll.x += ((m_mouseDeltaRotation.x * rotSpeed) * pCtx->fFrameTime);
			m_mouseDeltaRotation.x = 0.0f;
		}
		
		// Character comoponent params update
		if (m_pCharacterController)
		{
			IEntity* forwardHelper = m_pCameraComponent ? m_pCameraComponent->forwardCamera : nullptr;

			if (forwardHelper)
			{
				const Matrix34 transfoms = forwardHelper->GetWorldTM();
				Vec3 move = transfoms.TransformVector(velocity);
				m_pCharacterController->AddVelocity(move);
			}
		}

		IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
		debug->Begin("Player", true);
		debug->AddLine(GetEntity()->GetWorldPos(), GetEntity()->GetWorldPos() + GetEntity()->GetForwardDir()*2.0f, ColorF(1.0f, 0.0f, 0.0f), 1.0f);

		UpdateCharacterContoller(pCtx->fFrameTime);
	}
	break;
	}
}

void CPlayerComponent::OnShutDown()
{
	if (m_pCameraComponent)
	{
		m_pCameraComponent->SetTarget(nullptr);
	}
}

void CPlayerComponent::UpdateCharacterContoller(float frameTime)
{

	if (m_pCharacterController->GetVelocity().GetLength() > 0.2f)
	{
		Vec3 forward = m_pCharacterController->GetVelocity().GetNormalized();
		forward.z = 0;

		Quat newRotation = Quat::CreateRotationVDir(forward);
		
		Quat oldRotation = m_pEntity->GetRotation();
		Quat finalQ = Quat::CreateNlerp(oldRotation, newRotation, 0.2f);
		finalQ.Normalize();

		// Send updated transform to the entity, only orientation changes
		m_pEntity->SetPosRotScale(m_pEntity->GetWorldPos(), finalQ, GetEntity()->GetScale());
	}
}

void CPlayerComponent::Revive()
{
	GetEntity()->Hide(false);
	GetEntity()->SetWorldTM(Matrix34::Create(Vec3(1, 1, 1), IDENTITY, GetEntity()->GetWorldPos()));
	// Set player transformation, but skip this in the Editor
	if (!gEnv->IsEditor())
	{
		Vec3 playerScale = Vec3(1.f);
		Quat playerRotation = IDENTITY;

		// Position the player in the center of the map
		const float heightOffset = 20.f;
		float terrainCenter = gEnv->p3DEngine->GetTerrainSize() / 2.f;
		float height = gEnv->p3DEngine->GetTerrainZ(static_cast<int>(terrainCenter), static_cast<int>(terrainCenter));
		Vec3 playerPosition = Vec3(terrainCenter, terrainCenter, height + heightOffset);

		m_pEntity->SetWorldTM(Matrix34::Create(playerScale, playerRotation, playerPosition));
	}

	// Unhide the entity in case hidden by the Editor
	GetEntity()->Hide(false);

	// Reset input now that the player respawned
	m_inputFlags = 0;
	m_mouseDeltaRotation = ZERO;
}

void CPlayerComponent::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
{
	switch (type)
	{
	case EInputFlagType::Hold:
	{
		if (activationMode == eIS_Released)
		{
			m_inputFlags &= ~flags;
		}
		else
		{
			m_inputFlags |= flags;
		}
	}
	break;
	case EInputFlagType::Toggle:
	{
		if (activationMode == eIS_Released)
		{
			// Toggle the bit(s)
			m_inputFlags ^= flags;
		}
	}
	break;
	}
}