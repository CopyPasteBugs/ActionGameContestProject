#include "StdAfx.h"
#include "UnifiedCamera.h"

#include <array>

IUnifiedCamera* IUnifiedCamera::s_pActiveCamera = nullptr;

IUnifiedCamera::~IUnifiedCamera()
{
	if (s_pActiveCamera == this)
	{
		s_pActiveCamera = nullptr;
	}
}

void IUnifiedCamera::Initialize()
{
	if (m_bActivateOnCreate)
	{
		Activate();
	}
}

void IUnifiedCamera::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		const CCamera& systemCamera = gEnv->pSystem->GetViewCamera();

		const float farPlane = gEnv->p3DEngine->GetMaxViewDistance();

		m_camera.SetFrustum(systemCamera.GetViewSurfaceX(), systemCamera.GetViewSurfaceZ(), m_fieldOfView.ToRadians(), m_nearPlane, farPlane, systemCamera.GetPixelAspectRatio());
		m_camera.SetMatrix(GetWorldTransformMatrix());

		gEnv->pSystem->SetViewCamera(m_camera);
	}
	else if (event.event == ENTITY_EVENT_START_GAME || event.event == ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED)
	{
		if (m_bActivateOnCreate && !IsActive())
		{
			Activate();
		}
	}
}

uint64 IUnifiedCamera::GetEventMask() const
{
	uint64 bitFlags = IsActive() ? BIT64(ENTITY_EVENT_UPDATE) : 0;
	bitFlags |= BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED);

	return bitFlags;
}
#ifndef RELEASE
void IUnifiedCamera::Render(const IEntity & entity, const IEntityComponent & component, SEntityPreviewContext & context) const
{
	if (context.bSelected)
	{
		Matrix34 slotTransform = GetWorldTransformMatrix();

		// Don't use actual far plane as it's usually huge for cameras
		float distance = 10.f;
		float size = distance * tan(m_fieldOfView.ToRadians());
		
		std::array<Vec3, 4> points =
		{ {
				Vec3(size, distance, size),
				Vec3(-size, distance, size),
				Vec3(-size, distance, -size),
				Vec3(size, distance, -size)
			} };

		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(slotTransform.GetTranslation(), context.debugDrawInfo.color, slotTransform.TransformPoint(points[0]), context.debugDrawInfo.color);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(slotTransform.GetTranslation(), context.debugDrawInfo.color, slotTransform.TransformPoint(points[1]), context.debugDrawInfo.color);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(slotTransform.GetTranslation(), context.debugDrawInfo.color, slotTransform.TransformPoint(points[2]), context.debugDrawInfo.color);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(slotTransform.GetTranslation(), context.debugDrawInfo.color, slotTransform.TransformPoint(points[3]), context.debugDrawInfo.color);

		Vec3 p1 = slotTransform.TransformPoint(points[0]);
		Vec3 p2;
		for (int i = 0; i < points.size(); i++)
		{
			int j = (i + 1) % points.size();
			p2 = slotTransform.TransformPoint(points[j]);
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(p1, context.debugDrawInfo.color, p2, context.debugDrawInfo.color);
			p1 = p2;
		}
	}
}
#endif
void IUnifiedCamera::Activate()
{
	if (s_pActiveCamera != nullptr)
	{
		IUnifiedCamera* pPreviousCamera = s_pActiveCamera;
		s_pActiveCamera = nullptr;
		m_pEntity->UpdateComponentEventMask(pPreviousCamera);
	}

	s_pActiveCamera = this;
	m_pEntity->UpdateComponentEventMask(this);
}
