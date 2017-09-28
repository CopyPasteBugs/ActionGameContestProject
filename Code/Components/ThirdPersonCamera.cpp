#include "StdAfx.h"
#include "ThirdPersonCamera.h"

void CThirdPersonCamera::Initialize()
{
	IUnifiedCamera::Initialize();

	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.vPosition = GetEntity()->GetWorldPos();
	spawnParams.qRotation = IDENTITY;
	spawnParams.vScale = Vec3Constants<float>::fVec3_One;
	spawnParams.nFlags = ENTITY_FLAG_SPAWNED | ENTITY_FLAG_CLIENT_ONLY;
	
	blendCamera = gEnv->pEntitySystem->SpawnEntity(spawnParams);
	forwardCamera = gEnv->pEntitySystem->SpawnEntity(spawnParams);
}

uint64 CThirdPersonCamera::GetEventMask() const
{
	return IUnifiedCamera::GetEventMask();
}

void CThirdPersonCamera::ProcessEvent(SEntityEvent & event)
{
	IUnifiedCamera::ProcessEvent(event);

	if (event.event == ENTITY_EVENT_UPDATE)
	{
		// early out if no target for camera logic
		if (!Target) return;

		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		// calculate rotation and pos onbase yaw and pitch + distance values
		RotationMatrix = CCamera::CreateOrientationYPR(YawPitchRoll);
		Vec3 cameraForward = GetEntity()->GetWorldTM().GetColumn1();
		Vec3 pos = (Target->GetWorldPos() + m_TargetOffset) - (cameraForward * Distance); //TODO: add target offset variable
		Rotation = Quat(RotationMatrix);

		// set ditry rotation and pos(target trasforms) to blend-helper entity
		blendCamera->SetPosRotScale(pos, Rotation, Vec3(1, 1, 1));

		// plane rotation's (root helper) use only yaw value
		planeRotationMatrix = CCamera::CreateOrientationYPR(Vec3(YawPitchRoll.x, 0, 0));
		planeRotation = Quat(planeRotationMatrix);
		forwardCamera->SetPosRotScale(Target->GetWorldPos(), planeRotation, Vec3(1, 1, 1));

		// now, lerp active camera to helper entity 
		Vec3 bpos = GetEntity()->GetWorldPos();
		Quat brot = GetEntity()->GetWorldRotation();

		const float blendPositionSpeed = 40.0f;
		const float blendRotationSpeed = 20.0f;

		bpos.SetLerp(bpos, pos, blendPositionSpeed * pCtx->fFrameTime);
		brot.SetNlerp(brot, Rotation, blendRotationSpeed * pCtx->fFrameTime);

		GetEntity()->SetPosRotScale(bpos, brot, Vec3(1, 1, 1));
	}
}

void CThirdPersonCamera::OnShutDown()
{
	IUnifiedCamera::OnShutDown();

	if (blendCamera)
		gEnv->pEntitySystem->RemoveEntity(blendCamera->GetId());
	if (forwardCamera)
		gEnv->pEntitySystem->RemoveEntity(forwardCamera->GetId());

}

Vec3 CThirdPersonCamera::GetPlaneForward()
{
	Vec3 ret = FORWARD_DIRECTION;

	if (forwardCamera)
		ret = forwardCamera->GetWorldTM().GetColumn1();

	return ret;
}

void CThirdPersonCamera::UpdateDistanceToTarget(float& wheelMove, float frameTime)
{
	if (fabs(wheelMove) > 0)
	{
		Distance += wheelMove * frameTime;
		float clampDistance = CLAMP(Distance, m_minDistance, m_maxDistance);
		SetDistance(clampDistance);
		wheelMove = 0.0f;
	}
}

void CThirdPersonCamera::UpdateYaw(float mouseDeltaRotationX, float frameTime)
{
	const float rotSpeed = 0.1f;
	YawPitchRoll.x += ((mouseDeltaRotationX * rotSpeed) * frameTime);
}
