#include "EditorTool.h"
#include "../engine/Vertex.h"
#include "EditorEntity.h"
#include "../engine/CoreInterface.h"
#include "EditorGlobal.h"
#include "EditorEvent.h"
#include "../engine/Debug.h"
#include "RenderInfoEditor.h"

//#define FRAME_TIME g_pCore->GetTimeSinceLastFrame()
#define FRAME_TIME 1.0f

/****************************************************************************************
	EditorTool
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorTool::EditorTool()
{
	m_using = false;
	m_initialized = false;
	m_axisEnum = math::eAxis::X;
}
//-------------------------------------------------------------------
EditorTool::~EditorTool()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void EditorTool::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void EditorTool::Initialize(math::eAxis axisEnum)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	float radius = editorGlobal::tool::AXIS_RADIUS;

	m_axisEnum = axisEnum;
	m_axis = math::AXES[(uint)axisEnum];
	m_rotation = math::matrixOrthogonalUp( m_axis );

	m_scaleBase.x = radius;
	m_scaleBase.y = 1.0f;
	m_scaleBase.z = radius;
	m_scale = m_scaleBase;

	m_renderInfo.hModel = CreateModel( 12 );
	m_renderInfo.occlusionPass.selectedColor4 = editorGlobal::tool::SELECTED_OCCLUDED_COLOR;
	m_renderInfo.occlusionPass.unselectedColor4 = editorGlobal::tool::UNSELECTED_OCCLUDED_COLOR;

	m_renderInfo.normalPass.unselectedAmbient4 = editorGlobal::tool::UNSELECTED_NORMAL_AMBIENT[(uint)axisEnum];
	m_renderInfo.normalPass.unselectedDiffuse4 = editorGlobal::tool::UNSELECTED_NORMAL_DIFFUSE[(uint)axisEnum];

	m_renderInfo.normalPass.selectedAmbient4 = editorGlobal::tool::SELECTED_NORMAL_AMBIENT;
	m_renderInfo.normalPass.selectedDiffuse4 = editorGlobal::tool::SELECTED_NORMAL_DIFFUSE;

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorTool::Update()
{
	if ( m_hEntityAttached.IsValid() )
	{
		m_pos = m_hEntityAttached.GetAs<EditorEntity>().GetPos();

		m_scaleBase.x = math::min( math::average(m_hEntityAttached.GetAs<EditorEntity>().GetScale()) * 0.7f, 0.1f );
		m_scaleBase.z = m_scaleBase.x;
		m_scaleBase.y = editorGlobal::tool::AXIS_SCALE_OUTWARD + m_hEntityAttached.GetAs<EditorEntity>().GetScale() * m_axis;

		CalculateScaleRelativeToCamera(m_hEntityAttached, m_cameraTransform);
	}

	auto transform = m_renderInfo.GetTransform();
	transform.translate = math::matrixTranslate( m_pos );
	transform.scale = math::matrixScale( m_scale );
	transform.rotation = m_rotation;
	m_renderInfo.SetTransform( transform );
}
//-------------------------------------------------------------------
//	PrepareUse
//-------------------------------------------------------------------
void EditorTool::PrepareUse(const math::Vec3& toolCollisionPoint, eEditorToolAction action)
{
	m_using = true;

	switch (action)
	{
		case eEditorToolAction::Resize:
			m_pAction = new EditorToolAction_Resize;
			break;
		case eEditorToolAction::Translate:
			m_pAction = new EditorToolAction_Translate;
			break;
		case eEditorToolAction::Rotate:
			m_pAction = new EditorToolAction_Rotate;
			break;
		case eEditorToolAction::PickAxis:
			m_pAction = new EditorToolAction_PickAxis;
			break;
		default:
			CHECK( "Unrecognized action" );
			m_pAction = nullptr;
	}

	if ( m_pAction )
	{
		EditorToolActionInfo info;
		info.axisEnum = m_axisEnum;
		info.camera = m_cameraTransform;
		info.hEntity = m_hEntityAttached;
		info.toolCollisionPoint = toolCollisionPoint;

		m_pAction->PrepareUse( info );
	}
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
bool EditorTool::Use(const math::Vec2& dist)
{
	return m_pAction->Use( m_hEntityAttached, dist, m_cameraTransform );
}
//-------------------------------------------------------------------
//	EndUse
//-------------------------------------------------------------------
void EditorTool::EndUse()
{
	m_using = false;
	m_pAction = nullptr;

	SmartPtr<Event_EntityToolEndUse> event = new Event_EntityToolEndUse;
	event->hEntity = m_hEntityAttached;

	g_pEvent->Process( event );
}
//-------------------------------------------------------------------
//	Attach
//-------------------------------------------------------------------
void EditorTool::Attach(Handle<Entity> hEntity)
{
	m_hEntityAttached = hEntity;
}
//-------------------------------------------------------------------
//	GetCameraTransform
//-------------------------------------------------------------------
void EditorTool::SetCameraTransform(const math::CameraTransform& camera)
{
	m_cameraTransform = camera;
}
//-------------------------------------------------------------------
//	GetClosestCollision
//-------------------------------------------------------------------
float EditorTool::GetClosestCollision(const math::Vec3& rayOrigin, const math::Vec3& rayDir)
{
	math::Vec3 cylinderStart = m_pos - m_axis * m_scale.y;
	math::Vec3 cylinderEnd = m_pos + m_axis * m_scale.y;

	auto intersect = math::intersectRayCylinder( rayOrigin, rayDir, cylinderStart, cylinderEnd, m_scale.x );
	if ( !intersect.collision )
		return -1.0f;

	return intersect.t0;
}
//-------------------------------------------------------------------
//	CalculateScaleRelativeToCamera
//-------------------------------------------------------------------
void EditorTool::CalculateScaleRelativeToCamera(Handle<Entity> hEntity, const math::CameraTransform& camera)
{
	float scaleDistance = editorGlobal::tool::CAMERA_SCALE;
	float distCamToEntity = math::dist( hEntity.GetAs<EditorEntity>().GetPos(), camera.pos );
	float distBasedScale = distCamToEntity / scaleDistance;

	if ( distCamToEntity < scaleDistance )
		distBasedScale *= distBasedScale * distBasedScale;

	m_scale = m_scaleBase * (1.0f + distBasedScale * 1.5f);
	m_scale.y = m_scaleBase.y + distBasedScale;
}
//-------------------------------------------------------------------
//	CreateModel
//-------------------------------------------------------------------
Handle<Model> EditorTool::CreateModel(uint sliceCount)
{
	String name = "Cylinder_" + string::intToStr(sliceCount);

	if ( g_pResource->GetModelMngr()->HasModelByName( name ) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3N3::GetLayout() );

	const uint vertexCount = sliceCount * 2 + 2;
	const uint indexCount = sliceCount * 12;
	Vertex_P3N3* vertices = new Vertex_P3N3[vertexCount];
	uint* indices = new uint[indexCount];
	float y = 1.0f;
	math::Vec4 rotationPoint( 1.0f, 0.0f, 0.0f, 0.0f );
	float rotationDegs = 360.0f / (float)sliceCount;

	{
		uint index = 0;

		// Top
		vertices[0].pos = math::Vec3(0.0f, y, 0.0f);
		vertices[0].normal = math::Vec3(0.0f, 1.0f, 0.0f);
		for ( uint slice = 0; slice < sliceCount; slice++ )
		{
			index = slice + 1;
			math::Mtx44 rotation = math::matrixRotateDegs( 0.0f, 1.0f, 0.0f, rotationDegs * (float)slice );

			vertices[index].pos = (rotationPoint * rotation).Xyz();
			vertices[index].normal = math::normalize(vertices[index].pos);
			vertices[index].pos.y = y;
		}

		// Bottom
		for ( uint slice = 0; slice < sliceCount; slice++ )
		{
			index = sliceCount + slice + 1;
			vertices[index] = vertices[slice + 1];
			vertices[index].pos.y = -y;
		}
		index = vertexCount - 1;
		vertices[index].pos = math::Vec3(0.0f, -y, 0.0f);
		vertices[index].normal = math::Vec3(0.0f, -1.0f, 0.0f);
	}

	// Index
	{
		uint index = 0;

		// Top
		for ( uint slice = 0; slice < sliceCount; slice++ )
		{
			index = slice * 3;

			indices[index++] = 0;
			indices[index++] = slice + 1;
			indices[index++] = (slice + 1) % sliceCount + 1;
		}

		// Middle
		for ( uint slice = 0; slice < sliceCount; slice++ )
		{
			index = (sliceCount * 3) + slice * 6;
			uint sliceTopBase = slice + 1;
			uint sliceTopNext = sliceTopBase % sliceCount + 1;
			uint sliceBottomBase = sliceTopBase + sliceCount;
			uint sliceBottomNext = (sliceCount + 1) + (slice + 1) % sliceCount;

			indices[index++] = sliceTopBase;
			indices[index++] = sliceBottomBase;
			indices[index++] = sliceTopNext;

			indices[index++] = sliceTopNext;
			indices[index++] = sliceBottomBase;
			indices[index++] = sliceBottomNext;
		}

		// Top
		index = indexCount - sliceCount * 3;
		for ( uint slice = 0; slice < sliceCount; slice++ )
		{
			indices[index++] = vertexCount - 1;
			indices[index++] = (sliceCount + 1) + (slice + 1) % sliceCount;
			indices[index++] = (sliceCount + 1) + slice;
		}
	}

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::Triangle, vertices, vertexCount, indices, indexCount );

	safeArrayRelease( indices );
	safeArrayRelease( vertices );

	CHECK( result.IsValid() );
	return result;
}

/****************************************************************************************
	EditorToolAction_Base
*****************************************************************************************/

//-------------------------------------------------------------------
//	PrepareUse
//-------------------------------------------------------------------
void EditorToolAction_Base::PrepareUse(const EditorToolActionInfo& info)
{
	math::Vec3 entityToCollision = info.toolCollisionPoint - info.hEntity.GetAs<EditorEntity>().GetPos();
	m_axisEnum = info.axisEnum;
	m_axis = math::AXES[(uint)info.axisEnum];
	m_axisSign = entityToCollision * m_axis > 0.0f ? math::POSITIVE : math::NEGATIVE;
	m_totalMovement = 0.0f;

	OnPrepareUse( info );

	SmartPtr<Event_EntityToolUse> event = new Event_EntityToolUse;
	event->axisEnum = m_axisEnum;
	event->axisSign = m_axisSign;
	event->hEntity = info.hEntity;
	event->entityToolAction = GetActionEnum();

	g_pEvent->Process( event );
}
//-------------------------------------------------------------------
//	CalculateAmount
//-------------------------------------------------------------------
float EditorToolAction_Base::CalculateAmount(const math::Vec2& dist, const math::CameraTransform& camera)
{
	float result = 0.0f;
	
	if ( m_axisEnum == math::eAxis::Y )
		result = -dist.y;
	else
	{
		float dirDotCamRight = camera.right * m_axis;
		float dirDotCamUp = camera.up * m_axis;

		if ( math::abs(dirDotCamRight) > math::abs(dirDotCamUp) )
			result = dist.x * math::sign(dirDotCamRight);
		else
			result = -dist.y * math::sign(dirDotCamUp);
	}

	return result;
}
//-------------------------------------------------------------------
//	GetAxis
//-------------------------------------------------------------------
math::Vec3 EditorToolAction_Base::GetAxis(bool isSigned)const
{
	return math::AXES[(uint)m_axisEnum] * (isSigned ? (float)m_axisSign : 1.0f);
}
//-------------------------------------------------------------------
//	GetGridLeveling
//-------------------------------------------------------------------
float EditorToolAction_Base::GetGridLeveling()const
{
	float result = (float)editorGlobal::grid::LEVEL_SIZE_ONE / (float)g_pEditor->GetGrid()->GetCurrentLevel();
	result = math::max( result, 1.0f );
	return result * 0.01f;
}

/****************************************************************************************
	EditorToolAction_Resize
*****************************************************************************************/

//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
bool EditorToolAction_Resize::Use(Handle<Entity> hEntity, const math::Vec2& dist, const math::CameraTransform& camera)
{
	float amount = CalculateAmount( dist, camera );

	m_totalMovement += amount * FRAME_TIME * GetGridLeveling();

	if ( math::abs(m_totalMovement) > editorGlobal::tool::AXIS_MOVEMENT_AMOUNT_THRESHOLD )
	{
		int unitLevel = (int)g_pEditor->GetGrid()->GetUnitCurrentLevel();
		int halfUnitLevel = math::max(unitLevel / 2, 1);
		int posOnAxis = (int)(hEntity.GetAs<EditorEntity>().GetGridPos().AsVec3() * GetAxis());
		int oldScale = (int)(hEntity.GetAs<EditorEntity>().GetGridScale().AsVec3() * GetAxis(true));
		int newScale = halfUnitLevel * m_axisSign * (int)math::sign(amount);

		int snapTo = g_pEditor->GetGrid()->SnapToGrid( posOnAxis + oldScale, math::signEnum(amount), unitLevel );

		if ( snapTo != 0 )
		{
			newScale = snapTo / 2;
			newScale = math::max(math::abs(newScale), 1) * math::sign(snapTo);

			if ( m_axisSign == math::NEGATIVE )
				newScale = -newScale;
		}

		hEntity.GetAs<EditorEntity>().AddGridScale( newScale, m_axisEnum, m_axisSign );

		m_totalMovement = 0.0f;

		return true;
	}

	return false;
}

/****************************************************************************************
	EditorToolAction_Translate
*****************************************************************************************/

//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
bool EditorToolAction_Translate::Use(Handle<Entity> hEntity, const math::Vec2& dist, const math::CameraTransform& camera)
{
	float amount = CalculateAmount( dist, camera );
	m_totalMovement += amount * FRAME_TIME * GetGridLeveling();

	if ( math::abs(m_totalMovement) > editorGlobal::tool::AXIS_MOVEMENT_AMOUNT_THRESHOLD )
	{
		int currentValueOnAxis = (int)(GetAxis() * hEntity.GetAs<EditorEntity>().GetGridPos().AsVec3());
		int valueToAdd = math::max((int)g_pEditor->GetGrid()->GetUnitCurrentLevel(), editorGlobal::tool::GRID_TRANSLATE_MIN) * math::growth(amount);

		math::Vec3 addPos = GetAxis() * (float)valueToAdd;
		hEntity.GetAs<EditorEntity>().AddGridPos( VecGrid( addPos ) );

		m_totalMovement = 0.0f;

		return true;
	}

	return false;
}

/****************************************************************************************
	EditorToolAction_Rotate
*****************************************************************************************/

//-------------------------------------------------------------------
//	CalculateAmount
//-------------------------------------------------------------------
float EditorToolAction_Rotate::CalculateAmount(const math::Vec2& dist, const math::CameraTransform& camera)
{
	float result = 0.0f;

	if ( m_axisEnum == math::eAxis::Y )
		result = dist.x;
	else
	{
		float dirDotCamRight = camera.right * m_axis;
		float dirDotCamUp = camera.up * m_axis;

		if ( math::abs(dirDotCamRight) > math::abs(dirDotCamUp) )
			result = dist.y * math::sign(dirDotCamRight);
		else
			result = -dist.x * math::sign(dirDotCamUp);
	}

	if ( g_pInput->IsKeyDown(eKey::KEY_SHIFT) )
		result *= 0.1f;

	return result;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
bool EditorToolAction_Rotate::Use(Handle<Entity> hEntity, const math::Vec2& dist, const math::CameraTransform& camera)
{
	float amount = CalculateAmount( dist, camera );
	m_totalMovement += amount * FRAME_TIME * 0.0005f;

	if ( math::abs(m_totalMovement) > 0.0001f )
	{
		float angleRads = m_totalMovement;
		if ( m_axisEnum == math::eAxis::X )
			hEntity.GetAs<EditorEntity>().RotateYawPitchRoll( 0.0f, angleRads, 0.0f );
		else if ( m_axisEnum == math::eAxis::Y )
			hEntity.GetAs<EditorEntity>().RotateYawPitchRoll( angleRads, 0.0f, 0.0f );
		else if ( m_axisEnum == math::eAxis::Z )
			hEntity.GetAs<EditorEntity>().RotateYawPitchRoll( 0.0f, 0.0f, angleRads );

		m_totalMovement = 0.0f;

		return true;
	}

	return false;
}

/****************************************************************************************
	EditorToolAction_PickAxis
*****************************************************************************************/

//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
bool EditorToolAction_PickAxis::Use(Handle<Entity> hEntity, const math::Vec2& dist, const math::CameraTransform& camera)
{
	SmartPtr<Event_EditorToolUse> pEvent = new Event_EditorToolUse;
	pEvent->action = eEditorToolAction::PickAxis;
	pEvent->axisEnum = m_axisEnum;
	pEvent->axisSign = m_axisSign;

	g_pEvent->Process( pEvent );

	return true;
}

#undef FRAME_TIME
