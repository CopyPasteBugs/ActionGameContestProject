#include "StdAfx.h"
#include "ProximityTrigger.h"

CProximitryTrigger::~CProximitryTrigger()
{
}

void CProximitryTrigger::Initialize()
{
	
	pDistanceChecker = (IEntityTriggerComponent*)m_pEntity->CreateProxy(EEntityProxy::ENTITY_PROXY_TRIGGER);
	pDistanceChecker->SetTriggerBounds(GetLocalAABB());
	//pDistanceChecker->ForwardEventsTo(GetEntity()->GetId());

	//LEAVE
	//ENTER
}

void CProximitryTrigger::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_ENTERAREA:
	{	
		CryLogAlways("Enter area");
		break;
	}
	case ENTITY_EVENT_LEAVEAREA:
	{
		CryLogAlways("Leave area");
		break;
	}
	case ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED:
	{
		// update aabb vec from current size
		m_min = Vec3(-m_size * 0.5f);
		m_max = Vec3(m_size * 0.5f);
		break;
	}
	}
}

uint64 CProximitryTrigger::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_ENTERAREA) | BIT64(ENTITY_EVENT_LEAVEAREA) | BIT64(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED);
}
#ifndef RELEASE
void CProximitryTrigger::Render(const IEntity & entity, const IEntityComponent & component, SEntityPreviewContext & context) const
{
	if (context.bSelected)
	{
		Matrix34 transform = GetEntity()->GetWorldTM();
		AABB aabb = AABB(transform.TransformPoint(m_min), transform.TransformPoint(m_max));
		//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(slotTransform.GetTranslation(), context.debugDrawInfo.color, slotTransform.TransformPoint(points[0]), context.debugDrawInfo.color);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(aabb, true, context.debugDrawInfo.color, EBoundingBoxDrawStyle::eBBD_Faceted);
		
	}
}
#endif

const AABB CProximitryTrigger::GetWorldAABB()
{
	Matrix34 transform = GetEntity()->GetWorldTM();
	AABB aabb = AABB(transform.TransformPoint(m_min), transform.TransformPoint(m_max));
	return aabb;
}

const AABB CProximitryTrigger::GetLocalAABB()
{
	AABB aabb = AABB(m_min, m_max);
	return aabb;
}