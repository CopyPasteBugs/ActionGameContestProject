#include "StdAfx.h"
#include "UnifiedCharacterController.h"

IUnifiedCharacterController::~IUnifiedCharacterController()
{
	PhysicalizeWithNone();
}

void IUnifiedCharacterController::Initialize()
{
	Physicalize();
}

void IUnifiedCharacterController::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics();
		CRY_ASSERT_MESSAGE(pPhysicalEntity != nullptr, "Physical entity removed without call to IEntity::UpdateComponentEventMask!");

		// Update stats
		pe_status_living livingStatus;
		if (pPhysicalEntity->GetStatus(&livingStatus) != 0)
		{
			m_bOnGround = !livingStatus.bFlying;

			// Store the ground normal in case it is needed
			// Note that users have to check if we're on ground before using, is considered invalid in air.
			m_groundNormal = livingStatus.groundSlope;
		}

		// Get the player's velocity from physics
		pe_status_dynamics playerDynamics;
		if (pPhysicalEntity->GetStatus(&playerDynamics) != 0)
		{
			m_velocity = playerDynamics.v;
		}
	}
	else if (event.event == ENTITY_EVENT_COLLISION)
	{
		// Collision info can be retrieved using the event pointer
		EventPhysCollision* physCollision = reinterpret_cast<EventPhysCollision*>(event.nParam[0]);

		const char* surfaceTypeName = "";
		EntityId otherEntityId = INVALID_ENTITYID;

		ISurfaceTypeManager* pSurfaceTypeManager = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeManager();
		if (ISurfaceType* pSurfaceType = pSurfaceTypeManager->GetSurfaceType(physCollision->idmat[1]))
		{
			surfaceTypeName = pSurfaceType->GetName();
		}

		if (IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[1]))
		{
			otherEntityId = pOtherEntity->GetId();
		}


	}
	else if (event.event == ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED)
	{
		// Start validating inputs

		// Slide value will have no effect if larger than the fall angle, since we'll fall instead
		if (m_unifiedMovement.m_minSlideAngle > m_unifiedMovement.m_minFallAngle)
		{
			m_unifiedMovement.m_minSlideAngle = m_unifiedMovement.m_minFallAngle;
		}


		m_pEntity->UpdateComponentEventMask(this);

		Physicalize();
	}
}

uint64 IUnifiedCharacterController::GetEventMask() const
{
	uint64 eventMask = BIT64(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED);

	// Only update when we have a physical entity
	if (m_pEntity->GetPhysicalEntity() != nullptr)
	{
		eventMask |= BIT64(ENTITY_EVENT_UPDATE);
	}

	if (m_unifiedPhysics.m_bSendCollisionSignal)
	{
		eventMask |= BIT64(ENTITY_EVENT_COLLISION);
	}

	return eventMask;
}
#ifndef RELEASE
void IUnifiedCharacterController::Render(const IEntity & entity, const IEntityComponent & component, SEntityPreviewContext & context) const
{
	if (context.bSelected)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
		{
			pe_params_part partParams;

			// The living entity main part (cylinder / capsule) is always at index 0
			partParams.ipart = 0;
			if (pPhysicalEntity->GetParams(&partParams))
			{
				Matrix34 entityTransform = m_pEntity->GetWorldTM();

				geom_world_data geomWorldData;
				geomWorldData.R = Matrix33(Quat(entityTransform) * partParams.q);
				geomWorldData.scale = entityTransform.GetUniformScale() * partParams.scale;
				geomWorldData.offset = entityTransform.GetTranslation() + entityTransform.TransformVector(partParams.pos);

				gEnv->pSystem->GetIPhysRenderer()->DrawGeometry(partParams.pPhysGeom->pGeom, &geomWorldData, -1, 0, ZERO, context.debugDrawInfo.color);
			}
		}
	}
}
#endif

void IUnifiedCharacterController::Physicalize()
{
	// Physicalize the player as type Living.
	// This physical entity type is specifically implemented for players
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_LIVING;
	physParams.nSlot = GetOrMakeEntitySlotId();
	physParams.mass = m_unifiedPhysics.m_mass;

	pe_player_dimensions playerDimensions;

	// Prefer usage of a cylinder
	playerDimensions.bUseCapsule = m_unifiedPhysics.m_bCapsule ? 1 : 0;

	// Specify the size of our capsule, physics treats the input as the half-size, so we multiply our value by 0.5.
	// This ensures that 1 unit = 1m for designers.
	playerDimensions.sizeCollider = Vec3(m_unifiedPhysics.m_radius * 0.5f, 1.f, m_unifiedPhysics.m_height * 0.5f);
	// Capsule height needs to be adjusted to match 1 unit ~= 1m.
	if (playerDimensions.bUseCapsule)
	{
		playerDimensions.sizeCollider.z *= 0.5f;
	}

	// Keep pivot at the player's feet (defined in player geometry) 
	playerDimensions.heightPivot = m_unifiedPhysics.m_offset;
	// Offset collider upwards if the user requested it
	playerDimensions.heightCollider = m_pTransform != nullptr ? m_pTransform->GetTranslation().z : 0.f;
	playerDimensions.groundContactEps = 0.004f;

	physParams.pPlayerDimensions = &playerDimensions;

	pe_player_dynamics playerDynamics;
	playerDynamics.mass = physParams.mass;
	playerDynamics.kAirControl = m_unifiedMovement.m_airControlRatio;
	playerDynamics.kAirResistance = m_unifiedMovement.m_airResistance;
	playerDynamics.kInertia = m_unifiedMovement.m_inertia;
	playerDynamics.kInertiaAccel = m_unifiedMovement.m_inertiaAcceleration;

	playerDynamics.maxClimbAngle = m_unifiedMovement.m_maxClimbAngle.ToDegrees();
	playerDynamics.maxJumpAngle = m_unifiedMovement.m_maxJumpAngle.ToDegrees();
	playerDynamics.minFallAngle = m_unifiedMovement.m_minFallAngle.ToDegrees();
	playerDynamics.minSlideAngle = m_unifiedMovement.m_minSlideAngle.ToDegrees();

	playerDynamics.maxVelGround = m_unifiedMovement.m_maxGroundVelocity;

	physParams.pPlayerDynamics = &playerDynamics;

	m_pEntity->Physicalize(physParams);

	m_pEntity->UpdateComponentEventMask(this);
}

void IUnifiedCharacterController::PhysicalizeWithNone()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_NONE;
	m_pEntity->Physicalize(physParams);
}

void IUnifiedCharacterController::Ragdollize()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_ARTICULATED;
	physParams.mass = m_unifiedPhysics.m_mass;
	physParams.nSlot = GetEntitySlotId();
	physParams.bCopyJointVelocities = true;

	m_pEntity->Physicalize(physParams);
}
