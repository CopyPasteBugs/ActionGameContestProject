#include "StdAfx.h"
#include "UnifiedCharacterAnimation.h"

inline bool Serialize(Serialization::IArchive& archive, SDefaultScopeSettings& defaultSettings, const char* szName, const char* szLabel)
{
	archive(Serialization::MannequinControllerDefinitionPath(defaultSettings.m_controllerDefinitionPath), "ControllerDefPath", "Controller Definition");
	archive.doc("Path to the Mannequin controller definition");

	std::shared_ptr<Serialization::SMannequinControllerDefResourceParams> pParams;

	// Load controller definition for the context and fragment selectors
	if (archive.isEdit())
	{
		pParams = std::make_shared<Serialization::SMannequinControllerDefResourceParams>();

		IAnimationDatabaseManager &animationDatabaseManager = gEnv->pGameFramework->GetMannequinInterface().GetAnimationDatabaseManager();
		if (defaultSettings.m_controllerDefinitionPath.size() > 0)
		{
			pParams->pControllerDef = animationDatabaseManager.LoadControllerDef(defaultSettings.m_controllerDefinitionPath);
		}
	}

	archive(Serialization::MannequinScopeContextName(defaultSettings.m_contextName, pParams), "DefaultScope", "Default Scope Context Name");
	archive.doc("The Mannequin scope context to activate by default");

	archive(Serialization::MannequinFragmentName(defaultSettings.m_fragmentName, pParams), "DefaultFragment", "Default Fragment Name");
	archive.doc("The fragment to play by default");

	return true;
}

IUnifiedCharacterAnimation::~IUnifiedCharacterAnimation()
{
	SAFE_RELEASE(m_pActionController);
}

void IUnifiedCharacterAnimation::Initialize()
{
	LoadFromDisk();

	ResetCharacter();
}

void IUnifiedCharacterAnimation::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		//if (m_pActionController != nullptr)
		//{
		//	m_pActionController->Update(pCtx->fFrameTime);
		//}

		//Matrix34 characterTransform = GetWorldTransformMatrix();

		//// Set turn rate as the difference between previous and new entity rotation
		//m_turnAngle = Ang3::CreateRadZ(characterTransform.GetColumn1(), m_prevForwardDir) / pCtx->fFrameTime;
		//m_prevForwardDir = characterTransform.GetColumn1();

		//if (m_pCachedCharacter != nullptr)
		//{
			//if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
			//{
			//	pe_status_dynamics dynStatus;
			//	if (pPhysicalEntity->GetStatus(&dynStatus))
			//	{
			//		float travelAngle = Ang3::CreateRadZ(characterTransform.GetColumn1(), dynStatus.v.GetNormalized());
			//		float travelSpeed = dynStatus.v.GetLength2D();

			//		// Set the travel speed based on the physics velocity magnitude
			//		// Keep in mind that the maximum number for motion parameters is 10.
			//		// If your velocity can reach a magnitude higher than this, divide by the maximum theoretical account and work with a 0 - 1 ratio.
			//		if (!m_overriddenMotionParams.test(eMotionParamID_TravelSpeed))
			//		{
			//			m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelSpeed, travelSpeed, 0.f);
			//		}

			//		// Update the turn speed in CryAnimation, note that the maximum motion parameter (10) applies here too.
			//		if (!m_overriddenMotionParams.test(eMotionParamID_TurnAngle))
			//		{
			//			m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TurnAngle, m_turnAngle, 0.f);
			//		}

			//		if (!m_overriddenMotionParams.test(eMotionParamID_TravelAngle))
			//		{
			//			m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelAngle, travelAngle, 0.f);
			//		}
			//	}
			//}

			//if (m_pPoseAligner != nullptr && m_pPoseAligner->Initialize(*m_pEntity, m_pCachedCharacter))
			//{
			//	m_pPoseAligner->SetBlendWeight(1.f);
			//	m_pPoseAligner->Update(m_pCachedCharacter, QuatT(characterTransform), pCtx->fFrameTime);
			//}
		//}

		//m_overriddenMotionParams.reset();
	}
	else if (event.event == ENTITY_EVENT_ANIM_EVENT)
	{
		//if (m_pActionController != nullptr)
		//{
		//	const AnimEventInstance *pAnimEvent = reinterpret_cast<const AnimEventInstance*>(event.nParam[0]);
		//	ICharacterInstance *pCharacter = reinterpret_cast<ICharacterInstance*>(event.nParam[1]);

		//	m_pActionController->OnAnimationEvent(pCharacter, *pAnimEvent);
		//}
	}
	else if (event.event == ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED)
	{
		LoadFromDisk();
		ResetCharacter();
	}
}

uint64 IUnifiedCharacterAnimation::GetEventMask() const
{
	uint64 bitFlags = BIT64(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED);

	//if (m_pPoseAligner != nullptr)
	//{
	//	bitFlags |= BIT64(ENTITY_EVENT_UPDATE);
	//}

	if (m_pActionController != nullptr)
	{
		bitFlags |= BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_ANIM_EVENT);
	}

	return bitFlags;
}

void IUnifiedCharacterAnimation::ActivateContext(const Schematyc::CSharedString & contextName)
{
	if (m_pCachedCharacter == nullptr)
	{
		return;
	}

	const TagID scopeContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find(contextName.c_str());
	if (scopeContextId == TAG_ID_INVALID)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to find scope context %s in controller definition.", contextName.c_str());
		return;
	}

	// Setting Scope contexts can happen at any time, and what entity or character instance we have bound to a particular scope context
	// can change during the lifetime of an action controller.
	m_pActionController->SetScopeContext(scopeContextId, *m_pEntity, m_pCachedCharacter, m_pDatabase);
}

void IUnifiedCharacterAnimation::QueueFragment(const Schematyc::CSharedString & fragmentName)
{
	if (m_pAnimationContext == nullptr)
	{
		return;
	}

	const TagID fragmentId = m_pAnimationContext->controllerDef.m_fragmentIDs.Find(fragmentName.c_str());
	if (fragmentId == TAG_ID_INVALID)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to find Mannequin fragment %s in controller definition %s", fragmentName.c_str(), m_pAnimationContext->controllerDef.m_filename.c_str());
		return;
	}

	return QueueFragmentWithId(fragmentId);
}

void IUnifiedCharacterAnimation::QueueFragmentWithId(const FragmentID & fragmentId)
{
	if (m_pAnimationContext == nullptr)
	{
		return;
	}

	if (m_pActiveAction)
	{
		m_pActiveAction->Stop();
	}

	const int priority = 0;
	m_pActiveAction = new TAction<SAnimationContext>(priority, fragmentId);
	m_pActionController->Queue(*m_pActiveAction);
}

void IUnifiedCharacterAnimation::SetTag(const Schematyc::CSharedString & tagName, bool bSet)
{
	SetTagWithId(GetTagId(tagName.c_str()), bSet);
}

void IUnifiedCharacterAnimation::SetMotionParameter(EMotionParamID motionParam, float value)
{
	CRY_ASSERT(m_pCachedCharacter != nullptr);
	if (m_pCachedCharacter != nullptr)
	{
		m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(motionParam, value, 0.f);
		m_overriddenMotionParams.set(motionParam);
	}
}

TagID IUnifiedCharacterAnimation::GetTagId(const char * szTagName) const
{
	return m_pControllerDefinition->m_tags.Find(szTagName);
}

FragmentID IUnifiedCharacterAnimation::GetFragmentId(const char * szFragmentName) const
{
	return m_pControllerDefinition->m_fragmentIDs.Find(szFragmentName);
}

void IUnifiedCharacterAnimation::SetTagWithId(TagID id, bool bSet)
{
	m_pAnimationContext->state.Set(id, bSet);
}

void IUnifiedCharacterAnimation::LoadFromDisk()
{
	if (m_characterFile.value.size() > 0)
	{
		m_pCachedCharacter = gEnv->pCharacterManager->CreateInstance(m_characterFile.value);

		if (m_pCachedCharacter == nullptr)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load character %s!", m_characterFile.value.c_str());
			return;
		}

		if (m_bGroundAlignment && m_pCachedCharacter != nullptr)
		{
			if (m_pPoseAligner == nullptr)
			{
				CryCreateClassInstance(CPoseAlignerC3::GetCID(), m_pPoseAligner);
			}

			m_pPoseAligner->Clear();
		}
		else
		{
			m_pPoseAligner.reset();
		}
	}
	else
	{
		m_pCachedCharacter = nullptr;
	}

	if (m_controllerDefinitionPath.length() > 0 && m_databasePath.value.size() > 0)
	{
		// Now start loading the Mannequin data
		IMannequin& mannequinInterface = gEnv->pGameFramework->GetMannequinInterface();
		IAnimationDatabaseManager& animationDatabaseManager = mannequinInterface.GetAnimationDatabaseManager();

		// Load the Mannequin controller definition.
		// This is owned by the animation database manager
		m_pControllerDefinition = animationDatabaseManager.LoadControllerDef(m_controllerDefinitionPath.c_str());
		if (m_pControllerDefinition == nullptr)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load controller definition %s!", m_controllerDefinitionPath.c_str());
			return;
		}

		// Load the animation database
		m_pDatabase = animationDatabaseManager.Load(m_databasePath.value);
		if (m_pDatabase == nullptr)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load animation database %s!", m_databasePath.value.c_str());
			return;
		}
	}
}

void IUnifiedCharacterAnimation::ResetCharacter()
{
	m_pActiveAction = nullptr;

	// Release previous controller and context
	SAFE_RELEASE(m_pActionController);
	m_pAnimationContext.reset();

	if (m_pCachedCharacter == nullptr)
	{
		return;
	}

	m_pEntity->SetCharacter(m_pCachedCharacter, GetOrMakeEntitySlotId() | ENTITY_SLOT_ACTUAL, false);
	SetAnimationDrivenMotion(m_bAnimationDrivenMotion);

	if (m_pControllerDefinition != nullptr)
	{
		// Create a new animation context for the controller definition we loaded above
		m_pAnimationContext = stl::make_unique<SAnimationContext>(*m_pControllerDefinition);

		// Now create the controller that will be handling animation playback
		IMannequin& mannequinInterface = gEnv->pGameFramework->GetMannequinInterface();
		m_pActionController = mannequinInterface.CreateActionController(GetEntity(), *m_pAnimationContext);
		CRY_ASSERT(m_pActionController != nullptr);

		if (m_contextName.length() > 0)
		{
			ActivateContext(m_contextName);
		}

		if (m_fragmentName.length() > 0)
		{
			QueueFragment(m_fragmentName);
		}

		m_pEntity->UpdateComponentEventMask(this);
	}
}

void IUnifiedCharacterAnimation::SetAnimationDrivenMotion(bool bSet)
{
	m_bAnimationDrivenMotion = bSet;

	if (m_pCachedCharacter == nullptr)
	{
		return;
	}

	// Disable animation driven motion, note that the function takes the inverted parameter of what you would expect.
	m_pCachedCharacter->GetISkeletonAnim()->SetAnimationDrivenMotion(m_bAnimationDrivenMotion ? 0 : 1);
}

void IUnifiedCharacterAnimation::SetCharacterFile(const char * szPath)
{
	m_characterFile = szPath;
}

void IUnifiedCharacterAnimation::SetMannequinAnimationDatabaseFile(const char * szPath)
{
	m_databasePath = szPath;
}

void IUnifiedCharacterAnimation::SetControllerDefinitionFile(const char * szPath)
{
	m_controllerDefinitionPath = szPath;
}

void IUnifiedCharacterAnimation::SetDefaultScopeContextName(const char * szName)
{
	m_contextName = szName;
}

void IUnifiedCharacterAnimation::SetDefaultFragmentName(const char * szName)
{
	m_fragmentName = szName;
}

void IUnifiedCharacterAnimation::ManualUpdate(float frametime)
{
	if (m_pActionController != nullptr)
	{
		m_pActionController->Update(frametime);
	}
}
