#include "StdAfx.h"
#include "Player.h"

#include <CryRenderer/IRenderAuxGeom.h>
#include <array>

#include "ThirdPersonCamera.h"
#include "PlayerCharacterController.h"
#include "UnifiedCharacterAnimation.h"
#include "FootstepQueue.h"

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
		m_pCameraComponent->SetTargetOffset(Vec3(0.0f, 0.0f, 1.5f));
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

	m_pInputComponent->RegisterAction("player", "ragdollizeTest", [this](int activationMode, float value) { m_pCharacterController->Ragdollize(); });
	m_pInputComponent->BindAction("player", "ragdollizeTest", eAID_KeyboardMouse, EKeyId::eKI_R);

	InitializeAnimation();
	Revive();
	
	if (m_pAnimationComponent)
		FindAttachments(m_pAnimationComponent->GetCharacter());

	m_pFootstepQueue = m_pEntity->GetOrCreateComponent<CFootstepQueue>();

}

uint64 CPlayerComponent::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_ANIM_EVENT) | BIT64(ENTITY_EVENT_ENTERAREA) | BIT64(ENTITY_EVENT_LEAVEAREA);
}

void CPlayerComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_ENTERAREA:
	{
		CryLogAlways("Player got EnterArea event");
		break;
	}
	case ENTITY_EVENT_LEAVEAREA:
	{
		CryLogAlways("Player got LeaveArea event");
		break;
	}
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
			m_pCameraComponent->UpdateDistanceToTarget(m_mouseWheel, pCtx->fFrameTime);
			m_mouseWheel = 0;

			m_pCameraComponent->UpdateYaw(m_mouseDeltaRotation.x, pCtx->fFrameTime);
			m_mouseDeltaRotation.x = 0.0f;
		}
		
		// Character component params update
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
		UpdateAnimation(pCtx->fFrameTime);
		RaycastFootstepDown();
	}
	break;
	case ENTITY_EVENT_ANIM_EVENT:
	{
		const AnimEventInstance *pAnimEvent = reinterpret_cast<const AnimEventInstance *>(event.nParam[0]);
		ICharacterInstance *pCharacter = reinterpret_cast<ICharacterInstance *>(event.nParam[1]);

		if (pAnimEvent) 
		{
			IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
			debug->Begin("AnimEvent", true);
			debug->AddText(10, 100, 3, ColorF(1.0f), 0.1f, "AnimEvent");

		}
	}
	}
}

void CPlayerComponent::OnShutDown()
{
	if (m_pCameraComponent)
	{
		m_pCameraComponent->SetTarget(nullptr);
	}
}

void CPlayerComponent::InitializeAnimation()
{
	// Create the advanced animation component, responsible for updating Mannequin and animating the player
	//m_pAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();
	m_pAnimationComponent = m_pEntity->GetOrCreateComponent<IUnifiedCharacterAnimation>();
	if (!m_pAnimationComponent) return;

	m_pAnimationComponent->SetEntitySlotId(characterSlot);

	// Set the player geometry, this also triggers physics proxy creation
	m_pAnimationComponent->SetMannequinAnimationDatabaseFile("Animations/Mannequin/ADB/Panda.adb");
	m_pAnimationComponent->SetCharacterFile("Objects/Characters/Panda/Panda.cdf");

	m_pAnimationComponent->SetControllerDefinitionFile("Animations/Mannequin/ADB/PandaControllerDefinition.xml");
	m_pAnimationComponent->SetDefaultScopeContextName("ThirdPersonCharacter");
	//m_pAnimationComponent->ActivateContext("ThirdPersonCharacter");
	// Queue the idle fragment to start playing immediately on next update
	m_pAnimationComponent->SetDefaultFragmentName("Walk");


	// Disable movement coming from the animation (root joint offset), we control this entirely via physics
	m_pAnimationComponent->SetAnimationDrivenMotion(false);

	// Load the character and Mannequin data from file
	m_pAnimationComponent->LoadFromDisk();

	// Acquire fragment and tag identifiers to avoid doing so each update
	m_forwardFragmentId = m_pAnimationComponent->GetFragmentId("Walk");

	//// Query only one fragment (with 1DBS within) for while
	//m_pAnimationComponent->QueueFragmentWithId(m_forwardFragmentId);
}

void CPlayerComponent::UpdateCharacterContoller(float frameTime)
{

	if (m_pCharacterController->GetVelocity().GetLength() > 0.2f)
	{
		Vec3 forward = m_pCharacterController->GetLastMinVelocity().GetNormalized();
		forward.z = 0;

		Quat newRotation = Quat::CreateRotationVDir(forward);
		
		Quat oldRotation = m_pEntity->GetRotation();
		Quat finalQ = Quat::CreateNlerp(oldRotation, newRotation, 0.2f);
		finalQ.Normalize();

		// Send updated transform to the entity, only orientation changes
		m_pEntity->SetPosRotScale(m_pEntity->GetWorldPos(), finalQ, GetEntity()->GetScale());
	}
}

void CPlayerComponent::UpdateAnimation(float frameTime)
{
	if (!m_pAnimationComponent) return;

	float speed = m_pCharacterController->GetVelocity().GetLength2D();
	float normalizedSpeed = speed / m_pCharacterController->GetUnifiedMovement().m_maxGroundVelocity;
	
	float forcedClamped = CLAMP(normalizedSpeed, 0.0f, 1.0f);
	m_pAnimationComponent->SetMotionParameter(eMotionParamID_TravelSpeed, forcedClamped);

	m_pAnimationComponent->ManualUpdate(frameTime);
}

void CPlayerComponent::Revive()
{
	GetEntity()->Hide(false);
	GetEntity()->SetWorldTM(Matrix34::Create(Vec3(1, 1, 1), IDENTITY, GetEntity()->GetWorldPos()));
	
	// Update flags for slot
	uint32 flags =  ENTITY_SLOT_CAST_SHADOW;
	m_pEntity->SetSlotFlags(characterSlot, m_pEntity->GetSlotFlags(characterSlot) | flags);
	
	// Set player transformation, but skip this in the Editor
	//if (!gEnv->IsEditor())
	//{
	//	Vec3 playerScale = Vec3(1.f);
	//	Quat playerRotation = IDENTITY;

	//	// Position the player in the center of the map
	//	const float heightOffset = 20.f;
	//	float terrainCenter = gEnv->p3DEngine->GetTerrainSize() / 2.f;
	//	float height = gEnv->p3DEngine->GetTerrainZ(static_cast<int>(terrainCenter), static_cast<int>(terrainCenter));
	//	Vec3 playerPosition = Vec3(terrainCenter, terrainCenter, height + heightOffset);

	//	m_pEntity->SetWorldTM(Matrix34::Create(playerScale, playerRotation, playerPosition));
	//}

	// Unhide the entity in case hidden by the Editor
	GetEntity()->Hide(false);

	// Reset input now that the player respawned
	m_inputFlags = 0;
	m_mouseDeltaRotation = ZERO;
}

void CPlayerComponent::FindAttachments(ICharacterInstance* pCharacter)
{
	pFootStepLeft = pCharacter->GetIAttachmentManager()->GetInterfaceByName("FL");
	pFootStepRight = pCharacter->GetIAttachmentManager()->GetInterfaceByName("FR");
}

bool CPlayerComponent::GetTerrainHeight(Vec3 pos, Vec3& terrainHitPos)
{
	bool ret = false;
	ray_hit rayHit;
	unsigned int flags = rwi_stop_at_pierceable | rwi_colltype_any | rwi_ignore_back_faces;
	int hit = 0;
	Vec3 rayOrigin = pos;
	Vec3 downDirWithRange = Vec3(0, 0, -1) * 20.0f;
	
	hit = gEnv->pPhysicalWorld->RayWorldIntersection(rayOrigin, downDirWithRange, ent_terrain, flags, &rayHit, 1);
	if (hit & rayHit.bTerrain)
	{
		terrainHitPos = rayHit.pt;
		ret = true;
	}

	return ret;
}

void CPlayerComponent::RaycastFootstepDown()
{
	ICharacterInstance *pCharacter = m_pAnimationComponent->GetCharacter();
	if (pCharacter)
	{
		ISkeletonPose *pSkelPose = pCharacter ? pCharacter->GetISkeletonPose() : NULL;

		if (pSkelPose)
		{
			IDefaultSkeleton& rIDefaultSkeleton = pCharacter->GetIDefaultSkeleton();
			int16 footLeftID = rIDefaultSkeleton.GetJointIDByName("Foot.L");
			int16 footRightID = rIDefaultSkeleton.GetJointIDByName("Foot.R");
			QuatT footLeftInModelSpace = pSkelPose->GetAbsJointByID(footLeftID);
			QuatT footRightInModelSpace = pSkelPose->GetAbsJointByID(footRightID);

			Vec3 footLeftInWS = m_pEntity->GetWorldTM().TransformPoint(footLeftInModelSpace.t);
			Vec3 footRightInWS = m_pEntity->GetWorldTM().TransformPoint(footRightInModelSpace.t);

			IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
			debug->Begin("Footsteps", true);

			Vec3 lh = Vec3(0.0f);
			Vec3 rh = Vec3(0.0f);

			if (GetTerrainHeight(footLeftInWS, lh))
				debug->AddLine(footLeftInWS, lh, ColorF(1, 0, 0, 1), 1);


			if (GetTerrainHeight(footRightInWS, rh))
				debug->AddLine(footRightInWS, rh, ColorF(1, 0, 0, 1), 1);

			float heightL = (lh - footLeftInWS).GetLength();
			float heightR = (rh - footRightInWS).GetLength();

			float thresholdHigh = 0.3f;
			float thresholdLow = 0.15f;

			string s;
			debug->AddText(10, 230, 2, ColorF(1.0f), 1.0f, s.Format("LeftFootstep Height=%f", heightL));
			debug->AddText(10, 250, 2, ColorF(1.0f), 1.0f, s.Format("RightFootstep Height=%f", heightR));
			debug->AddText(10, 270, 2, ColorF(1.0f), 1.0f, s.Format("queuedFootSteps=%d", m_pFootstepQueue->m_SpawnVec.size()));

			float sv = m_pCharacterController->GetVelocity().GetLength2D();
			if ((sv > 1.0f) & (heightL > thresholdHigh))
			{
				if (!FootStepLeftState) FootStepLeftState = 1;
			}

			if (FootStepLeftState)
				if ((sv > 1.0f) & (heightL < thresholdLow))
				{
					debug->AddText(10, 200, 2, ColorF(1.0f), 0.1f, "left footstep!");
					FootStepLeftState = 0;
					if (m_pFootstepQueue)
						m_pFootstepQueue->QueueNewFootstep(lh);

				}

			//R
			if ((sv > 1.0f) & (heightR > thresholdHigh))
			{
				if (!FootStepRightState) FootStepRightState = 1;
			}

			if (FootStepRightState)
				if ((sv > 1.0f) & (heightR < thresholdLow))
				{
					debug->AddText(10, 200, 2, ColorF(1.0f), 0.1f, "right footstep!");
					FootStepRightState = 0;
					if (m_pFootstepQueue)
						m_pFootstepQueue->QueueNewFootstep(rh);

				}
		}
	}
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