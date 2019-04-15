#include "EditorEntity_CameraTrackPoint.h"
#include "RenderInfoEditor.h"
#include "../engine/Primitive.h"
#include "EditorGlobal.h"
#include "EditorEntity_CameraTrack.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_CameraTrackPoint::EditorEntity_CameraTrackPoint() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_CameraTrackPoint::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = false;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE;
	
	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->color4 = math::normalizeRgb( 255, 200, 0 );
	pRenderInfo->emissiveIntensity = 0.8f;
	pRenderInfo->diffuseIntensity = 0.2f;
	pRenderInfo->hDiffuseTexture = primitive::texture::createWhite();
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	int scale = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.15f);
	CHECK( scale > 0 );
	SetGridScale( scale, scale, scale );
}
//-------------------------------------------------------------------
//	OnToolUse
//-------------------------------------------------------------------
void EditorEntity_CameraTrackPoint::OnToolUse()
{
	EditorEntity_CameraTrack* pTrack = m_hOwner->Convert<EditorEntity_CameraTrack>();
	Handle<Entity> hOtherPoint = pTrack->GetTrackPoint0()->GetId() != GetId() ? pTrack->GetTrackPoint0() : pTrack->GetTrackPoint1();
	EditorEntity_CameraTrackPoint* pOtherPoint = hOtherPoint->Convert<EditorEntity_CameraTrackPoint>();

	math::Vec3 posThis = GetPos();
	math::Vec3 posOther = pOtherPoint->GetPos();
	math::Vec3 posTrackAdjusted = (posThis + posOther) * 0.5f;
	
	pTrack->SetGridPos( posTrackAdjusted * (float)editorGlobal::grid::UNIT_SIZE_ONE );
}
//-------------------------------------------------------------------
//	SetOwner
//-------------------------------------------------------------------
void EditorEntity_CameraTrackPoint::SetOwner(Handle<Entity> hOwner)
{
	if ( hOwner->GetClassRelationship_Is<EditorEntity_CameraTrack>() )
	{
		m_hOwner = hOwner;
	}
	else
		ASSERT( "Wrong type" );
}
//-------------------------------------------------------------------
//	GetOwner
//-------------------------------------------------------------------
Handle<Entity> EditorEntity_CameraTrackPoint::GetOwner()
{
	return m_hOwner;
}
//-------------------------------------------------------------------
EditorEntity_CameraTrack* EditorEntity_CameraTrackPoint::GetOwnerPtr()
{
	CHECK( m_hOwner->GetClassRelationship_Is<EditorEntity_CameraTrack>() );
	return (EditorEntity_CameraTrack*)m_hOwner.GetPtr();
}
//-------------------------------------------------------------------
const EditorEntity_CameraTrack* EditorEntity_CameraTrackPoint::GetOwnerPtr()const
{
	CHECK( m_hOwner->GetClassRelationship_Is<EditorEntity_CameraTrack>() );
	return (EditorEntity_CameraTrack*)m_hOwner.GetPtr();
}