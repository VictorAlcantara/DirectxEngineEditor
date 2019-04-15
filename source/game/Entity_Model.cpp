#include "Entity_Model.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/CoreInterface.h"
#include "../editor/Grid.h"
#include "../shared/EntityUtil.h"
#include "../engine/Path.h"
#include "../engine/Component_Render.h"
#include "../engine/api_File.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_Model::Entity_Model() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
Entity_Model::~Entity_Model()
{
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_Model::OnInitialize()
{
	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hDiffuseMap = primitive::texture::createWhite();
	pRenderInfo->material = eMaterial::Opaque;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision>();
	m_hCollision->SetCollider( eColliderInfo::Aabb );

	UpdateCollisionInfo();
}
//-------------------------------------------------------------------
//	UpdateCollisionInfo
//-------------------------------------------------------------------
void Entity_Model::UpdateCollisionInfo()
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>();

	m_hCollision->GetCollider(true)->aabbScale = pRenderInfo->GetAabb().GetScale();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_Model::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_MODEL );

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::EMISSIVE_INTENSITY, pRenderInfo->emissiveIntensity );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_INTENSITY, pRenderInfo->ambientIntensity );
	isCommandValid |= pLine->SetFloat( command::DIFFUSE_INTENSITY, pRenderInfo->diffuseIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_INTENSITY, pRenderInfo->specularIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_POWER, pRenderInfo->specularPower );
	isCommandValid |= pLine->SetBool( command::VISIBLE, pRenderInfo->visible );

	if ( !isCommandValid )
	{
		VecGrid gridData;
		math::Vec3 vec3Data;
		math::Vec4 vec4Data;
		String strData;
		bool bData;

		isCommandValid = true;

		if ( pLine->SetQuotedString(command::NAME, strData) )
		{
			SetName( strData );
		}
		else if ( pLine->SetBool(command::NOCLIP, bData) )
		{
			m_hCollision->SetNoclip( bData );
		}
		else if ( pLine->SetIntArray(command::GRID_POS, &gridData[0], 3) )
			pRenderInfo->SetTranslation( Grid::ToVec3(gridData) );
		else if ( pLine->SetIntArray(command::GRID_SCALE, &gridData[0], 3) )
			pRenderInfo->SetScale( Grid::ToVec3(gridData) );
		else if ( pLine->SetQuotedString(command::MODEL, strData) )
		{
			Handle<Model> hModel = g_pResource->GetModelMngr()->LoadObj<Vertex_P3N3Tg3Bn3T2>( path::model(strData) );

			CHECK( hModel );

			if ( !hModel )
				hModel = primitive::geometry::createBlock();
			else
			{
				uint modelId = hModel->GetId();
				String textureFile = path::texture(strData);
				if ( platform::file::exists(textureFile) )
				{
					auto hTexture = g_pResource->GetTextureMngr()->CreateFromFile( textureFile, false );
					if ( hTexture )
						pRenderInfo->hDiffuseMap = hTexture;
				}
			}

			pRenderInfo->SetModel( hModel );
		}
		else if ( pLine->SetFloatArray(command::ROTATION_EULER, &vec3Data[0], 3, false) )
		{
			math::Euler angle;

			angle.type = math::eAngle::Radian;
			angle.yaw = vec3Data.y;
			angle.pitch = vec3Data.x;
			angle.roll = vec3Data.z;

			pRenderInfo->SetRotation( angle );
		}
		else
			isCommandValid = false;
	
		UpdateCollisionInfo();
	}

	CHECK( isCommandValid );
}
