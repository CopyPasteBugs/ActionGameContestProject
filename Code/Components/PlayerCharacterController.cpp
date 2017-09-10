#include "StdAfx.h"
#include "PlayerCharacterController.h"

void CPlayerCharacterController::Initialize()
{
	IUnifiedCharacterController::Initialize();

}

uint64 CPlayerCharacterController::GetEventMask() const
{
	return IUnifiedCharacterController::GetEventMask();
}

void CPlayerCharacterController::ProcessEvent(SEntityEvent & event)
{
	return IUnifiedCharacterController::ProcessEvent(event);
}

void CPlayerCharacterController::OnShutDown()
{
	IUnifiedCharacterController::OnShutDown();


}
