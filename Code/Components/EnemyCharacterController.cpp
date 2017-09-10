#include "StdAfx.h"
#include "EnemyCharacterController.h"

void CEnemyCharacterController::Initialize()
{
	IUnifiedCharacterController::Initialize();

}

uint64 CEnemyCharacterController::GetEventMask() const
{
	return IUnifiedCharacterController::GetEventMask();
}

void CEnemyCharacterController::ProcessEvent(SEntityEvent & event)
{
	return IUnifiedCharacterController::ProcessEvent(event);
}

void CEnemyCharacterController::OnShutDown()
{
	IUnifiedCharacterController::OnShutDown();


}