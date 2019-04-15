#include "EditorEntity_Model.h"
#include "RenderInfoEditor.h"
#include "EditorGlobal.h"
#include "../engine/Debug.h"
#include "../engine/Texture.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/ScriptReader.h"
#include "../shared/EntityUtil.h"
#include "../engine/Path.h"
#include "../engine/Component_Render.h"
#include "../engine/api_File.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_Model::EditorEntity_Model() : EditorEntity(true)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Model::OnShutdown_EditorEntity()
{
	g_pScene->Debug_Remove( m_hAabb );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Model::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE | TOOL_ALLOW_ROTATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = primitive::texture::createWhite();
	pRenderInfo->ambientIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 1.0f;
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->specularPower = 1.0f;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	SetGridScale( VecGrid(editorGlobal::grid::UNIT_SIZE_ONE) );

	m_hAabb = g_pScene->Debug_AddCube();
	m_hAabb->color4 = math::normalizeRgb( 0, 255, 150 );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorEntity_Model::Update()
{
	{
		auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(false);
		m_hAabb->pos = pRenderInfo->GetTransform().GetPos();
		m_hAabb->scale = pRenderInfo->GetAabb().GetScale();
	}

	if ( g_pKeyMap->CanPerform(editorGlobal::action::ENTITY_RESET) )
	{
		auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);
		auto transform = pRenderInfo->GetTransform();
		transform.rotation = math::Mtx44();
		pRenderInfo->SetTransform(transform);
	}
}
//-------------------------------------------------------------------
//	RotateYawPitchRoll
//-------------------------------------------------------------------
void EditorEntity_Model::RotateYawPitchRoll(float yaw, float pitch, float roll)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);

	m_angle.yaw += yaw;
	m_angle.pitch += pitch;
	m_angle.roll += roll;

	math::Mtx44 rotate = math::matrixYawPitchRoll( yaw, pitch, roll );

	auto transform = pRenderInfo->GetTransform();
	transform.rotation = transform.rotation * rotate;
	pRenderInfo->SetTransform( transform );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_Model::GetSerialized()const
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();
	String modelName = g_pResource->GetModelMngr()->GetModelFileName( pRenderInfo->GetModel()->GetId() );

	m_angle.yaw = math::remainder( m_angle.yaw, math::TWO_PI );
	m_angle.pitch =  math::remainder( m_angle.pitch, math::TWO_PI );
	m_angle.roll = math::remainder( m_angle.roll, math::TWO_PI );

	return command::serializeLine(command::ENTITY_MODEL) + 
		command::serializeQuoted( command::NAME, GetName() ) +
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString() ) + 
		command::serializeLine(command::COLOR, string::vec4ToStr(pRenderInfo->color4) ) + 
		command::serializeLine(command::EMISSIVE_INTENSITY, string::floatToStr(pRenderInfo->emissiveIntensity) ) + 
		command::serializeLine(command::AMBIENT_INTENSITY, string::floatToStr(pRenderInfo->ambientIntensity) ) + 
		command::serializeLine(command::DIFFUSE_INTENSITY, string::floatToStr(pRenderInfo->diffuseIntensity) ) + 
		command::serializeLine(command::SPECULAR_INTENSITY, string::floatToStr(pRenderInfo->specularIntensity) ) + 
		command::serializeLine(command::SPECULAR_POWER, string::floatToStr(pRenderInfo->specularPower) ) + 
		command::serializeLine(command::ROTATION_EULER, string::eulerToStr(m_angle) ) + 
		(modelName.Length() > 0 ? command::serializeLine(command::MODEL, string::quote(modelName) ) : "") + 
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Model::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_MODEL );
	isCommandValid |= ProcessCommand_Name(pLine);

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::EMISSIVE_INTENSITY, pRenderInfo->emissiveIntensity );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_INTENSITY, pRenderInfo->ambientIntensity );
	isCommandValid |= pLine->SetFloat( command::DIFFUSE_INTENSITY, pRenderInfo->diffuseIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_INTENSITY, pRenderInfo->specularIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_POWER, pRenderInfo->specularPower );

	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	if ( !isCommandValid )
	{
		VecGrid gridData;
		math::Vec3 vec3Data;
		math::Vec4 vec4Data;
		String strData;

		isCommandValid = true;

		if ( pLine->SetQuotedString(command::MODEL, strData) )
		{
			Handle<Model> hModel = g_pResource->GetModelMngr()->LoadObj<Vertex_P3N3Tg3Bn3T2>( path::model(strData) );

			CHECK( hModel );

			if ( !hModel )
				hModel = GetModel();

			pRenderInfo->SetModel( hModel );
			OnModelChange();

		}
		else if ( pLine->SetFloatArray(command::ROTATION_EULER, &vec3Data[0], 3, false) )
		{
			RotateYawPitchRoll( vec3Data.y, vec3Data.x, vec3Data.z );
		}
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
//-------------------------------------------------------------------
//	OnModelChange
//-------------------------------------------------------------------
void EditorEntity_Model::OnModelChange()
{
	auto pInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);
	String modelName = g_pResource->GetModelMngr()->GetModelFileName( pInfo->GetModel() );

	if ( modelName.Length() == 0 )
		return;

	String textureFile = path::texture(modelName);
	if ( platform::file::exists(textureFile) )
	{
		auto hTexture = g_pResource->GetTextureMngr()->CreateFromFile( textureFile, false );
		if ( hTexture )
			pInfo->hDiffuseTexture = hTexture;
	}
}