#include "GuiPageControl.h"
#include "EditorGui.h"
#include "../engine/String.h"
#include "EditorEntity.h"
#include "EditorGlobal.h"
#include "GuiComponent.h"
#include "GuiPage.h"
#include "GuiAnimation.h"
#include "RenderInfoEditor.h"
#include "../engine/api_File.h"
#include "../engine/Path.h"
#include "../engine/Texture.h"
#include "GuiComponentFunction.h"
#include "../engine/Font.h"
#include "../engine/Handle.h"
#include "../engine/Light.h"
#include "../shared/Command.h"
#include "../engine/Primitive.h"
#include "../engine/Component_Render.h"
#include "EditorEntity_TriggerCommand.h"
#include "EditorEntity_Door.h"
#include "EditorEntity_AmbientLight.h"
#include "EditorEntity_PointLight.h"
#include "EditorEntity_SpotLight.h"
#include "EditorEntity_Talk.h"
#include "EditorEntity_Partition.h"
#include "EditorEntity_Monster.h"
#include "../shared/Component_LightFlicker.h"

const String prefix_textbox = "txtbox_";
const String prefix_numberbox = "nbrbox_";
const String prefix_slider = "sldr_";
const String prefix_checkbox = "chkbox_";

//-------------------------------------------------------------------
//	ShowPageMain
//-------------------------------------------------------------------
void GuiPageControl::ShowPageMain()
{
	HashString pageName = editorGlobal::guiPage::PAGE_MAIN_MENU;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( !pPage )
	{
		float panelHeight = 0.2f;
		float buttonDimension = panelHeight * 0.8f;
		pPage = pGui->CreatePage( pageName );

		GuiComponent_Panel* pPanel = pPage->Add<GuiComponent_Panel>( "pnl_mainMenu" );

		// Panel
		{
			GuiConfig_Panel configPanel;

			configPanel.pAnimation = CreateAnimation_StrechAlphaWeak();
			configPanel.anchor.horizontal = eGuiAnchorHorizontal::Center;
			configPanel.anchor.vertical = eGuiAnchorVertical::Bottom;
			configPanel.anchor.paddingHorizontal = 0;
			configPanel.anchor.paddingVertical = 15;
			configPanel.dimension = math::Vec2( 410.0f, 140.0f );
			configPanel.layer = 9;
			configPanel.color4 = math::saturateRgb( math::normalizeRgb(200), 1.0f );

			pPanel->SetConfig( configPanel );
		}

		math::Vec2 padding;
		GuiConfig_Button configButton;
		ButtonTextInfo buttonTextInfo;
		int buttonLeftPadding = 15;
		// Button create block
		{
			configButton.pAnimation = CreateAnimation_Button();
			configButton.anchor.horizontal = eGuiAnchorHorizontal::Left;
			configButton.anchor.vertical = eGuiAnchorVertical::Center;
			configButton.anchor.paddingHorizontal = 55;
			configButton.anchor.paddingVertical = 0;
			configButton.dimension = math::Vec2(90.0f);
			configButton.layer = 8;
			configButton.pClickFunction = button_createEntity;
			configButton.flag = (uint)eEditorEntityType::Block;

			buttonTextInfo.nameSufix = "createBlock";
			buttonTextInfo.configButton = configButton;
			buttonTextInfo.text.label = "BLOCK";
			buttonTextInfo.text.charSize = 30;
			buttonTextInfo.text.color4 = math::normalizeRgb( 50 );

			padding = MakeButtonText( pPage, pPanel, buttonTextInfo );
		}

		// Button create entity
		{
			configButton.anchor.paddingHorizontal = configButton.anchor.paddingHorizontal + (int)configButton.dimension.x + buttonLeftPadding;
			configButton.pClickFunction = button_openEntitySelectMenu;

			buttonTextInfo.nameSufix = "createEntity";
			buttonTextInfo.configButton = configButton;
			buttonTextInfo.text.label = "ENTITY";
			buttonTextInfo.text.charSize = 28;
			buttonTextInfo.text.color4 = math::normalizeRgb( 50 );

			padding = MakeButtonText( pPage, pPanel, buttonTextInfo );
		}

		// Button create trigger
		{
			configButton.anchor.paddingHorizontal = configButton.anchor.paddingHorizontal + (int)configButton.dimension.x + buttonLeftPadding;
			configButton.pClickFunction = button_openTriggerSelectMenu;

			buttonTextInfo.nameSufix = "createTrigger";
			buttonTextInfo.configButton = configButton;
			buttonTextInfo.text.label = "TRIGGER";
			buttonTextInfo.text.charSize = 26;
			buttonTextInfo.text.color4 = math::normalizeRgb( 50 );

			MakeButtonText( pPage, pPanel, buttonTextInfo );
		}
	}
	
	ShowPage( pPage, true );
}
//-------------------------------------------------------------------
//	ShowPageProperty
//-------------------------------------------------------------------
bool GuiPageControl::ShowPageProperty(EditorEntity* pEntity)
{
	EditorGui* pGui = GetGui();
	GuiPage* pPage = nullptr;

	if ( pEntity )
	{
		HashString pageName;

		if ( pEntity->GetType() == eEditorEntityType::Block )
		{
			auto pRenderInfo = pEntity->GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

			PropertyNameValueTable valueTable;
			valueTable.Add( "ColorRed", pRenderInfo->color4.x, eGuiComponent::Slider );
			valueTable.Add( "ColorGreen", pRenderInfo->color4.y, eGuiComponent::Slider );
			valueTable.Add( "ColorBlue", pRenderInfo->color4.z, eGuiComponent::Slider );
			valueTable.Add( "emissiveIntensity", pRenderInfo->emissiveIntensity, eGuiComponent::Slider );
			valueTable.Add( "diffuseIntensity", pRenderInfo->diffuseIntensity, eGuiComponent::Slider );
			valueTable.Add( "ambientIntensity", pRenderInfo->ambientIntensity, eGuiComponent::Slider );
			valueTable.Add( "specularIntensity", pRenderInfo->specularIntensity, eGuiComponent::Slider );
			valueTable.Add( "specularPower", pRenderInfo->specularPower, eGuiComponent::Slider );
			valueTable.Add( "uPos", pRenderInfo->GetTexTranslate().x, eGuiComponent::Numberbox );
			valueTable.Add( "vPos", pRenderInfo->GetTexTranslate().y, eGuiComponent::Numberbox );
			valueTable.Add( "uScale", pRenderInfo->GetTexScale().x, eGuiComponent::Numberbox );
			valueTable.Add( "vScale", pRenderInfo->GetTexScale().y, eGuiComponent::Numberbox );
			valueTable.Add( "texRot", pRenderInfo->GetTexRotationDegs(), eGuiComponent::Numberbox );

			pPage = CreatePageProperty_Block( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::Door )
		{
			auto pRenderInfo = pEntity->GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

			PropertyNameValueTable valueTable;
			valueTable.Add( "Name", pEntity->GetName().GetString(), eGuiComponent::Textbox );
			valueTable.Add( "ColorRed", pRenderInfo->color4.x, eGuiComponent::Slider );
			valueTable.Add( "ColorGreen", pRenderInfo->color4.y, eGuiComponent::Slider );
			valueTable.Add( "ColorBlue", pRenderInfo->color4.z, eGuiComponent::Slider );
			valueTable.Add( "emissiveIntensity", pRenderInfo->emissiveIntensity, eGuiComponent::Slider );
			valueTable.Add( "ambientIntensity", pRenderInfo->ambientIntensity, eGuiComponent::Slider );
			valueTable.Add( "diffuseIntensity", pRenderInfo->diffuseIntensity, eGuiComponent::Slider );
			valueTable.Add( "specularIntensity", pRenderInfo->specularIntensity, eGuiComponent::Slider );
			valueTable.Add( "specularPower", pRenderInfo->specularPower, eGuiComponent::Slider );
			valueTable.Add( "uPos", pRenderInfo->GetTexTranslate().x, eGuiComponent::Slider );
			valueTable.Add( "vPos", pRenderInfo->GetTexTranslate().y, eGuiComponent::Slider );
			valueTable.Add( "uScale", pRenderInfo->GetTexScale().x, eGuiComponent::Slider );
			valueTable.Add( "vScale", pRenderInfo->GetTexScale().y, eGuiComponent::Slider );
			valueTable.Add( "texRot", pRenderInfo->GetTexRotationDegs(), eGuiComponent::Slider );
			valueTable.Add( "openAmount", ((EditorEntity_Door*)pEntity)->GetOpenAmount(), eGuiComponent::Slider );
			valueTable.Add( "duration", ((EditorEntity_Door*)pEntity)->GetDuration(), eGuiComponent::Numberbox );
			valueTable.Add( "locked", ((EditorEntity_Door*)pEntity)->IsLocked(), eGuiComponent::Checkbox );

			pPage = CreatePageProperty_Door( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::CameraTrack )
		{
			//pPage = CreatePageProperty_CameraTrack();
		}
		else if ( pEntity->GetType() == eEditorEntityType::AmbientLight )
		{
			EditorEntity_AmbientLight* pEntityAmbientLight = (EditorEntity_AmbientLight*)pEntity;

			PropertyNameValueTable valueTable;
			valueTable.Add( "ambientRed", pEntityAmbientLight->GetAmbientColor().x, eGuiComponent::Slider );
			valueTable.Add( "ambientGreen", pEntityAmbientLight->GetAmbientColor().y, eGuiComponent::Slider );
			valueTable.Add( "ambientBlue", pEntityAmbientLight->GetAmbientColor().z, eGuiComponent::Slider );

			pPage = CreatePageProperty_AmbientLight( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::PointLight )
		{
			EditorEntity_PointLight* pEntityPointLight = (EditorEntity_PointLight*)pEntity;
			auto hPointLight = pEntityPointLight->GetLight();
			bool flicker = pEntityPointLight->GetComponentPtrAs<Component_LightFlicker>()->IsEnabled();

			PropertyNameValueTable valueTable;
			valueTable.Add( "range", hPointLight->range, eGuiComponent::Slider );
			valueTable.Add( "diffuseRed", hPointLight->diffuseColor4.x, eGuiComponent::Slider );
			valueTable.Add( "diffuseGreen", hPointLight->diffuseColor4.y, eGuiComponent::Slider );
			valueTable.Add( "diffuseBlue", hPointLight->diffuseColor4.z, eGuiComponent::Slider );
			valueTable.Add( "specularRgb", hPointLight->specularColor4.x, eGuiComponent::Slider );
			valueTable.Add( "attenuation0", hPointLight->attenuation0, eGuiComponent::Slider );
			valueTable.Add( "attenuation1", hPointLight->attenuation1, eGuiComponent::Slider );
			valueTable.Add( "attenuation2", hPointLight->attenuation2, eGuiComponent::Slider );
			valueTable.Add( "flicker", flicker, eGuiComponent::Checkbox );

			pPage = CreatePageProperty_PointLight( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::SpotLight )
		{
			EditorEntity_SpotLight* pEntitySpotLight = (EditorEntity_SpotLight*)pEntity;
			auto hSpotLight = pEntitySpotLight->GetLight();
			bool flicker = pEntitySpotLight->GetComponentPtrAs<Component_LightFlicker>()->IsEnabled();

			PropertyNameValueTable valueTable;
			valueTable.Add( "range", hSpotLight->GetRange(), eGuiComponent::Slider );
			valueTable.Add( "innerCone", hSpotLight->GetInnerConeDegs(), eGuiComponent::Slider );
			valueTable.Add( "outerCone", hSpotLight->GetOuterConeDegs(), eGuiComponent::Slider );
			valueTable.Add( "diffuseRed", hSpotLight->diffuseColor4.x, eGuiComponent::Slider );
			valueTable.Add( "diffuseGreen", hSpotLight->diffuseColor4.y, eGuiComponent::Slider );
			valueTable.Add( "diffuseBlue", hSpotLight->diffuseColor4.z, eGuiComponent::Slider );
			valueTable.Add( "specularRgb", hSpotLight->specularColor4.x, eGuiComponent::Slider );
			valueTable.Add( "attenuation0", hSpotLight->attenuation0, eGuiComponent::Slider );
			valueTable.Add( "attenuation1", hSpotLight->attenuation1, eGuiComponent::Slider );
			valueTable.Add( "attenuation2", hSpotLight->attenuation2, eGuiComponent::Slider );
			valueTable.Add( "shadowEnabled", hSpotLight->shadowEnabled, eGuiComponent::Checkbox );
			valueTable.Add( "shadowBias", pEntitySpotLight->GetEditorShadowBias(), eGuiComponent::Slider );
			valueTable.Add( "distAdjust", hSpotLight->shadowDistanceAdjustment, eGuiComponent::Slider );
			valueTable.Add( "flicker", flicker, eGuiComponent::Checkbox );

			pPage = CreatePageProperty_SpotLight( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::TriggerCamera )
		{
			pPage = CreatePageProperty_TriggerCamera();
		}
		else if ( pEntity->GetType() == eEditorEntityType::TriggerCommand )
		{
			auto pTrigger = (EditorEntity_TriggerCommand*)pEntity;
			PropertyNameValueTable valueTable;

			valueTable.Add( "FireEventOnce", pTrigger->IsFireEventOnce(), eGuiComponent::Checkbox );
			valueTable.Add( "active", pTrigger->IsActive(), eGuiComponent::Checkbox );

			pPage = CreatePageProperty_TriggerCommand(valueTable);
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::Model )
		{
			auto pRenderInfo = pEntity->GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

			PropertyNameValueTable valueTable;
			valueTable.Add( "Name", pEntity->GetName().GetString(), eGuiComponent::Textbox );
			valueTable.Add( "ColorRed", pRenderInfo->color4.x, eGuiComponent::Slider );
			valueTable.Add( "ColorGreen", pRenderInfo->color4.y, eGuiComponent::Slider );
			valueTable.Add( "ColorBlue", pRenderInfo->color4.z, eGuiComponent::Slider );
			valueTable.Add( "emissiveIntensity", pRenderInfo->emissiveIntensity, eGuiComponent::Slider );
			valueTable.Add( "diffuseIntensity", pRenderInfo->diffuseIntensity, eGuiComponent::Slider );
			valueTable.Add( "ambientIntensity", pRenderInfo->ambientIntensity, eGuiComponent::Slider );
			valueTable.Add( "specularIntensity", pRenderInfo->specularIntensity, eGuiComponent::Slider );
			valueTable.Add( "specularPower", pRenderInfo->specularPower, eGuiComponent::Slider );

			pPage = CreatePageProperty_Model( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::Talk )
		{
			auto pTalk = (EditorEntity_Talk*)pEntity;
			PropertyNameValueTable valueTable;
			valueTable.Add( "Name", pTalk->GetName().GetString(), eGuiComponent::Textbox );
			valueTable.Add( "FireEventOnce", pTalk->IsFireEventOnce(), eGuiComponent::Checkbox );

			pPage = CreatePageProperty_Talk( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::Monster )
		{
			auto pMonster = (EditorEntity_Monster*)pEntity;
			PropertyNameValueTable valueTable;
			valueTable.Add( "Name", pMonster->GetName().GetString(), eGuiComponent::Textbox );
			valueTable.Add( "SpawnOnCreate", pMonster->SpawnOnCreate(), eGuiComponent::Checkbox );
			valueTable.Add( "Active", pMonster->Active(), eGuiComponent::Checkbox );

			pPage = CreatePageProperty_Monster( valueTable );
			SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::Partition )
		{
			auto pPartition = (EditorEntity_Partition*)pEntity;
			PropertyNameValueTable valueTable;
			valueTable.Add( "AssertOnEntityContact", pPartition->DebugGetAssertOnEntityContact().GetString(), eGuiComponent::Textbox );
			valueTable.Add( "AssertOnAddEntity", pPartition->DebugGetAssertOnAddEntity(), eGuiComponent::Checkbox );
			valueTable.Add( "AssertOnCreatePartition", pPartition->DebugGetAssertOnCreatePartition(), eGuiComponent::Checkbox );

			//pPage = CreatePageProperty_Partition( valueTable );
			//SetValues( pPage, valueTable );
		}
		else if ( pEntity->GetType() == eEditorEntityType::Refraction )
		{
			auto pRenderInfo = pEntity->GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

			PropertyNameValueTable valueTable;
			valueTable.Add( "ColorRed", pRenderInfo->color4.x, eGuiComponent::Slider );
			valueTable.Add( "ColorGreen", pRenderInfo->color4.y, eGuiComponent::Slider );
			valueTable.Add( "ColorBlue", pRenderInfo->color4.z, eGuiComponent::Slider );
			valueTable.Add( "refractionIntensity", pRenderInfo->refractionIntensity, eGuiComponent::Slider );
			valueTable.Add( "colorSaturation", pRenderInfo->colorSaturation, eGuiComponent::Slider );
			valueTable.Add( "colorSeparation", pRenderInfo->colorSeparation, eGuiComponent::Slider );
			valueTable.Add( "uPos", pRenderInfo->GetTexTranslate().x, eGuiComponent::Slider );
			valueTable.Add( "vPos", pRenderInfo->GetTexTranslate().y, eGuiComponent::Slider );
			valueTable.Add( "uScale", pRenderInfo->GetTexScale().x, eGuiComponent::Slider );
			valueTable.Add( "vScale", pRenderInfo->GetTexScale().y, eGuiComponent::Slider );
			valueTable.Add( "texRot", pRenderInfo->GetTexRotationDegs(), eGuiComponent::Slider );

			pPage = CreatePageProperty_Refraction( valueTable );
			SetValues( pPage, valueTable );
		}
	}

	if ( pPage )
		ShowPage( pPage, false );

	return pPage;
}
//-------------------------------------------------------------------
//	OpenTextureSelectMenu
//-------------------------------------------------------------------
void GuiPageControl::OpenTextureSelectMenu(eGuiTexture textureType)
{
	HashString pageName = editorGlobal::guiPage::PAGE_TEXTURE_SELECT;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	HashString panelName = "pnl_textureSelect";
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	uint panelWidth = math::min( (uint)viewportDimension.x - ((uint)viewportDimension.x % 100), (uint)viewportDimension.y - ((uint)viewportDimension.y % 100) );
	uint panelHeight = panelWidth;
	uint paddingHorizontal = 10;
	uint paddingVertical = 10;

	if ( !pPage )
	{
		pPage = pGui->CreatePage( pageName );
		

		// Panel
		{
			GuiComponent_Panel* pPanel = pPage->Add<GuiComponent_Panel>( panelName );
			GuiConfig_Panel configPanel;

			configPanel.pAnimation = CreateAnimation_StrechInsideOut();

			configPanel.anchor.horizontal = eGuiAnchorHorizontal::Center;
			configPanel.anchor.vertical = eGuiAnchorVertical::Center;
			configPanel.anchor.paddingHorizontal = 0;
			configPanel.anchor.paddingVertical = 0;
			configPanel.color4 = math::normalizeRgb(200, 180, 160);

			configPanel.dimension = math::Vec2( (float)(panelWidth + paddingHorizontal * 2), (float)(panelHeight + paddingVertical * 2) );
			configPanel.layer = 5;

			pPanel->SetConfig( configPanel );
		}
	}

	uint width = panelWidth / 10;
	uint height = panelHeight / 10;
	uint index = 0;
	uint colCurrent = 0;
	uint rowCurrent = 0;
	uint colCount = panelWidth / width;

	for ( auto it = g_pResource->GetTextureMngr()->GetAll(); it; it++, index++ )
	{
		if ( (*it).Get().GetType() != eTextureType::Texture2d )
			continue;

		if ( colCurrent >= colCount )
		{
			colCurrent = 0;
			rowCurrent++;
		}

		HashString btnName = "btn_colorMap" + string::intToStr(index);
		GuiComponent_Button* pButton = pPage->FindAs<GuiComponent_Button>( btnName );

		if ( !pButton )
			pButton = pPage->Add<GuiComponent_Button>( btnName, panelName );

		GuiConfig_Button configButton;
		configButton.pAnimation = CreateAnimation_Button();
		configButton.anchor.horizontal = eGuiAnchorHorizontal::Left;
		configButton.anchor.paddingHorizontal = paddingHorizontal + colCurrent * width;
		configButton.anchor.vertical = eGuiAnchorVertical::Top;
		configButton.anchor.paddingVertical = paddingVertical + rowCurrent * height;
		configButton.dimension = math::Vec2((float)width, (float)height);
		configButton.allowTransparency = false;

		if ( textureType == eGuiTexture::DiffuseMap )
			configButton.pClickFunction = button_selectDiffuseMap;
		else if ( textureType == eGuiTexture::SpecularMap )
			configButton.pClickFunction = button_selectSpecularMap;
		else if ( textureType == eGuiTexture::NormalMap )
			configButton.pClickFunction = button_selectNormalMap;
		else
			ASSERT( "Unrecognized texture type" );

		configButton.flag = (*it)->GetName().GetHash();
		configButton.hTexture = (*it);
		configButton.layer = 4;
		configButton.idleColor4 = math::Vec3( 0.7f ).Xyzw(1.0f);
		configButton.mouseOverColor4 = math::Vec4( 1.0f );

		pButton->SetConfig( configButton );

		colCurrent++;
	}

	ShowPage( pPage, false );
}
//-------------------------------------------------------------------
//	OpenModelSelectMenu
//-------------------------------------------------------------------
void GuiPageControl::OpenModelSelectMenu()
{
	HashString pageName = editorGlobal::guiPage::PAGE_MODEL_SELECT;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( !pPage )
	{
		pPage = pGui->CreatePage( pageName );

		GuiComponent_Panel* pPanel = pPage->Add<GuiComponent_Panel>( "pnl_modelSelect" );
		math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
		uint panelWidth = math::min( (uint)viewportDimension.x - ((uint)viewportDimension.x % 100), (uint)viewportDimension.y - ((uint)viewportDimension.y % 100) );
		uint panelHeight = panelWidth;
		uint paddingHorizontal = 10;
		uint paddingVertical = 10;

		// Panel
		{
			GuiConfig_Panel configPanel;

			configPanel.pAnimation = CreateAnimation_StrechInsideOut();

			configPanel.anchor.horizontal = eGuiAnchorHorizontal::Center;
			configPanel.anchor.vertical = eGuiAnchorVertical::Center;
			configPanel.anchor.paddingHorizontal = 0;
			configPanel.anchor.paddingVertical = 0;
			configPanel.color4 = math::normalizeRgb(200, 180, 160);

			configPanel.dimension = math::Vec2( (float)(panelWidth + paddingHorizontal * 2), (float)(panelHeight + paddingVertical * 2) );
			configPanel.layer = 5;

			pPanel->SetConfig( configPanel );
		}

		uint width = panelWidth / 10;
		uint height = panelHeight / 10;
		uint index = 0;
		uint colCurrent = 0;
		uint rowCurrent = 0;
		uint colCount = panelWidth / width;
		List<Handle<Model>> models;

		g_pResource->GetModelMngr()->GetAllWithLayout<Vertex_P3N3Tg3Bn3T2>( models );

		for ( auto it = models.GetIterator(); it; it++, index++ )
		{
			uint modelId = (*it)->GetId();
			String fileName = g_pResource->GetModelMngr()->GetModelFileName( modelId );

			if ( fileName.Length() == 0 )
				continue;

			if ( colCurrent >= colCount )
			{
				colCurrent = 0;
				rowCurrent++;
			}

			GuiComponent_Button* pButton = pPage->Add<GuiComponent_Button>( "btn_model" + string::intToStr(index), pPanel->GetName() );

			GuiConfig_Button configButton;
			configButton.pAnimation = CreateAnimation_Button();
			configButton.anchor.horizontal = eGuiAnchorHorizontal::Left;
			configButton.anchor.paddingHorizontal = paddingHorizontal + colCurrent * width;
			configButton.anchor.vertical = eGuiAnchorVertical::Top;
			configButton.anchor.paddingVertical = paddingVertical + rowCurrent * height;
			configButton.dimension = math::Vec2((float)width, (float)height);

			configButton.pClickFunction = button_selectModel;
			configButton.flag = modelId;

			configButton.hTexture = primitive::texture::createBlack();
			configButton.hModel = g_pResource->GetModelMngr()->GetFormatted( (*it), Vertex_P3N3::GetLayout() );
			configButton.modelAngularVelocity.yaw = 90.0f;
			configButton.layer = 4;
			configButton.idleColor4 = math::Vec3( 0.7f ).Xyzw(1.0f);
			configButton.mouseOverColor4 = math::Vec4( 1.0f );

			pButton->SetConfig( configButton );

			colCurrent++;
		}
	}

	ShowPage( pPage, false );
}
//-------------------------------------------------------------------
//	OpenEntitySelectMenu
//-------------------------------------------------------------------
void GuiPageControl::OpenEntitySelectMenu()
{
	HashString pageName = editorGlobal::guiPage::PAGE_ENTITY_SELECT;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( !pPage )
	{
		pPage = pGui->CreatePage( pageName );

		GuiComponent_Panel* pPanel = pPage->Add<GuiComponent_Panel>( "pnl_entitySelect" );

		String labels[] = { "PLAYER SPAWN", "DOOR", "CAMERA TRACK", "AMBIENT LIGHT", "POINT LIGHT", "SPOT LIGHT", "MODEL", "TALK", "MONSTER", "PARTITION", "REFRACTION" };
		uint flags[] = { 
			(uint)eEditorEntityType::PlayerSpawn, (uint)eEditorEntityType::Door, (uint)eEditorEntityType::CameraTrack,
			(uint)eEditorEntityType::AmbientLight, (uint)eEditorEntityType::PointLight, (uint)eEditorEntityType::SpotLight, (uint)eEditorEntityType::Model, 
			(uint)eEditorEntityType::Talk, (uint)eEditorEntityType::Monster, (uint)eEditorEntityType::Partition, (uint)eEditorEntityType::Refraction,
		};

		CHECK( ARRAY_COUNT(labels) == ARRAY_COUNT(flags) );

		uint buttonCount = ARRAY_COUNT(flags);
		uint buttonWidth = 110;
		uint panelTotalWidth = 750;
		uint buttonColCount = panelTotalWidth / buttonWidth;
		uint buttonRowCount = buttonCount / buttonColCount;
		uint buttonTotalHorizontalPadding = panelTotalWidth - buttonWidth * buttonColCount;
		uint buttonHorizontalPadding = buttonTotalHorizontalPadding / (buttonColCount + 1);
		uint buttonHeight = 40;
		uint buttonTotalVerticalPadding = buttonHeight * (buttonRowCount + 1);
		uint panelVerticalPadding = 40;
		uint panelTotalHeight = buttonTotalVerticalPadding + panelVerticalPadding;
		uint buttonVerticalPadding = panelVerticalPadding / (buttonRowCount + 2);

		GuiConfig_Panel configPanel;
		configPanel.pAnimation = CreateAnimation_StrechAlphaWeak();
		configPanel.anchor.horizontal = eGuiAnchorHorizontal::Center;
		configPanel.anchor.vertical = eGuiAnchorVertical::Center;
		configPanel.anchor.paddingHorizontal = 0;
		configPanel.anchor.paddingVertical = (int)g_pRenderer->GetWindowViewport().GetDimensionScaled(1.0f, 0.1f).y;
		configPanel.color4 = math::saturateRgb( math::normalizeRgb(190, 190, 255), 0.8f );
		configPanel.dimension = math::Vec2( (float)panelTotalWidth, (float)panelTotalHeight );
		configPanel.layer = 4;

		pPanel->SetConfig( configPanel );

		ButtonTextInfo buttonTextInfo;
		buttonTextInfo.configButton.anchor.horizontal = eGuiAnchorHorizontal::Left;
		buttonTextInfo.configButton.anchor.vertical = eGuiAnchorVertical::Top;
		buttonTextInfo.configButton.dimension = math::Vec2( (float)buttonWidth, (float)buttonHeight );
		buttonTextInfo.configButton.layer = configPanel.layer - 1;
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_createEntity;
		buttonTextInfo.text.charSize = 16;
		buttonTextInfo.text.color4 = math::Vec3(0.12f).Xyzw(1.0f);

		uint colCurrent = 0;
		uint rowCurrent = 0;
		for ( uint i = 0; i < buttonCount; i++ )
		{
			if ( colCurrent >= buttonColCount )
			{
				colCurrent = 0;
				rowCurrent++;
			}

			buttonTextInfo.configButton.anchor.paddingHorizontal = buttonHorizontalPadding + colCurrent * (buttonWidth + buttonHorizontalPadding);
			buttonTextInfo.configButton.anchor.paddingVertical = buttonVerticalPadding + rowCurrent * (buttonHeight + buttonVerticalPadding);
			buttonTextInfo.nameSufix = "create" + string::remove(labels[i], " ");
			buttonTextInfo.configButton.flag = flags[i];
			buttonTextInfo.text.label = labels[i];
			MakeButtonText( pPage, pPanel, buttonTextInfo );
			colCurrent++;
		}
	}

	ShowPage( pPage, false );
}
//-------------------------------------------------------------------
//	OpenTriggerSelectMenu
//-------------------------------------------------------------------
void GuiPageControl::OpenTriggerSelectMenu()
{
	HashString pageName = editorGlobal::guiPage::PAGE_TRIGGER_SELECT;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( !pPage )
	{
		pPage = pGui->CreatePage( pageName );

		GuiComponent_Panel* pPanel = pPage->Add<GuiComponent_Panel>( "pnl_triggerSelect" );

		GuiConfig_Panel configPanel;
		configPanel.pAnimation = CreateAnimation_StrechAlphaWeak();
		configPanel.anchor.horizontal = eGuiAnchorHorizontal::Center;
		configPanel.anchor.vertical = eGuiAnchorVertical::Center;
		configPanel.anchor.paddingHorizontal = 0;
		configPanel.anchor.paddingVertical = (int)g_pRenderer->GetWindowViewport().GetDimensionScaled(1.0f, 0.1f).y;
		configPanel.color4 = math::saturateRgb( math::normalizeRgb(190, 190, 255), 0.8f );
		configPanel.dimension = math::Vec2( 440, 150 );
		configPanel.layer = 4;

		pPanel->SetConfig( configPanel );

		float horizontalPadding = 0.145f;
		ButtonTextInfo buttonTextInfo;
		buttonTextInfo.nameSufix = "createTriggerCamera";
		buttonTextInfo.configButton.anchor = GuiAnchor( eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Center, 25, 0 );
		buttonTextInfo.configButton.dimension = math::Vec2( 190, 95 );
		buttonTextInfo.configButton.layer = configPanel.layer - 1;
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.text.charSize = 24;
		buttonTextInfo.text.color4 = math::Vec3(0.12f).Xyzw(1.0f);
		buttonTextInfo.text.label = "CAMERA";
		buttonTextInfo.configButton.pClickFunction = button_createEntity;
		buttonTextInfo.configButton.flag = (uint)eEditorEntityType::TriggerCamera;
		MakeButtonText( pPage, pPanel, buttonTextInfo );

		buttonTextInfo.nameSufix = "createTriggerCommand";
		buttonTextInfo.configButton.anchor.paddingHorizontal = 225;
		buttonTextInfo.text.label = "COMMAND";
		buttonTextInfo.configButton.flag = (uint)eEditorEntityType::TriggerCommand;
		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	ShowPage( pPage, false );
}
//-------------------------------------------------------------------
//	OpenTextArea
//-------------------------------------------------------------------
void GuiPageControl::OpenTextArea(const List<String>& value, uint flag)
{
	HashString pageName = editorGlobal::guiPage::PAGE_TEXT_AREA;
	HashString textAreaName = "textArea";
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );
	GuiComponent_TextArea* pTextArea = nullptr;

	if ( !pPage )
	{
		pPage = pGui->CreatePage( pageName );

		pTextArea = pPage->Add<GuiComponent_TextArea>( textAreaName );

		GuiConfig_TextArea configTextArea;
		configTextArea.anchor = GuiAnchor( eGuiAnchorHorizontal::Center, eGuiAnchorVertical::Center, 0, 0 );
		configTextArea.layer = 1;
		configTextArea.pTextAreaFunction = gui_entityProcessCommand_StringChain;
		configTextArea.charsAllowed = GUI_TEXT_ALLOW_ALL;

		pTextArea->SetConfig( configTextArea );
	}

	if ( !pTextArea )
		pTextArea = pPage->FindAs<GuiComponent_TextArea>(textAreaName);

	CHECK( pTextArea );

	pTextArea->SetFlag( flag );
	pTextArea->SetValue( value );

	ShowPage( pPage, false );
}
//-------------------------------------------------------------------
//	CreatePageProperty_Block
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_Block(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_BLOCK;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );
	
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	int defaultValueIndex = 0;
	String labelTexts[] = { "red", "green", "blue", "emissive", "ambient", "diffuse", "specular", "specPower" };
	String sufixes[] = { "colorRed", "colorGreen", "colorBlue", "emissiveIntensity", "ambientIntensity", "diffuseIntensity", "specularIntensity", "specularPower" };
	String commands[] = { 
		command::COLOR_RED, 
		command::COLOR_GREEN, 
		command::COLOR_BLUE,
		command::EMISSIVE_INTENSITY,
		command::AMBIENT_INTENSITY, 
		command::DIFFUSE_INTENSITY, 
		command::SPECULAR_INTENSITY, 
		command::SPECULAR_POWER,
	};
	math::Vec2 values[] = {
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(20.0f, 3.0f),
	};

	math::Vec2 padding;
	float paddingSpace = 25;

	SliderInfo info;
	info.padding.x = 10;
	info.padding.y = 15;
	info.fontSize = 18;
	info.pFunction = gui_entityProcessCommand_Float;
	info.layer = 8;
	
	CHECK( ARRAY_COUNT(labelTexts) == ARRAY_COUNT(sufixes) && ARRAY_COUNT(commands) == ARRAY_COUNT(values) && ARRAY_COUNT(labelTexts) == ARRAY_COUNT(commands) );

	for ( uint i = 0; i < ARRAY_COUNT(labelTexts); i++ )
	{
		info.nameSufix = sufixes[i];
		info.labelText = labelTexts[i];
		info.command = commands[i];
		info.valueMin = values[i].x;
		info.valueMax = values[i].y;
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		MakeSlider( pPage, pPanel, info );
		info.padding.y += paddingSpace;
		info.nameSufix = "";
	}

	ButtonTextInfo buttonTextInfo;
	int btnTexPadding = 10;
	// Color map button
	{
		buttonTextInfo.nameSufix = "colorMap";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Top, 25, (int)info.padding.y );
		buttonTextInfo.configButton.dimension = math::Vec2( 65, 28 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_openTextureSelectMenu;
		buttonTextInfo.configButton.flag = (uint)eGuiTexture::DiffuseMap;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 20;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "DIFF";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Specular map button
	{
		buttonTextInfo.nameSufix = "specularMap";
		buttonTextInfo.configButton.anchor.paddingHorizontal += (int)(btnTexPadding + buttonTextInfo.configButton.dimension.x);
		buttonTextInfo.configButton.pClickFunction = button_openTextureSelectMenu;
		buttonTextInfo.configButton.flag = (uint)eGuiTexture::SpecularMap;
		buttonTextInfo.text.label = "SPEC";

		padding = MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Normal map button
	{
		buttonTextInfo.nameSufix = "normalMap";
		buttonTextInfo.configButton.anchor.paddingHorizontal += (int)(btnTexPadding + buttonTextInfo.configButton.dimension.x);
		buttonTextInfo.configButton.pClickFunction = button_openTextureSelectMenu;
		buttonTextInfo.configButton.flag = (uint)eGuiTexture::NormalMap;
		buttonTextInfo.text.label = "NRML";

		padding = MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Color map transform
	{
		paddingSpace = 40.0f;

		NumberboxInfo nbxInfo;
		nbxInfo.labelText = "uPos";
		nbxInfo.fontSize = 16;
		nbxInfo.layer = info.layer;
		nbxInfo.padding = info.padding + math::Vec2(0.0f, paddingSpace);
		nbxInfo.value = valueTable.Get(nbxInfo.labelText).valueFloat;
		nbxInfo.valueMin = -5.0f;
		nbxInfo.valueMax = 5.0f;
		nbxInfo.pFunction = gui_entityProcessCommand_Float;
		nbxInfo.command = command::TEXTURE_POS_U;
		MakeNumberbox( pPage, pPanel, nbxInfo );

		paddingSpace = 25.0f;

		nbxInfo.labelText = "vPos";
		nbxInfo.padding.y += paddingSpace;
		nbxInfo.value = valueTable.Get(nbxInfo.labelText).valueFloat;
		nbxInfo.valueMin = -5.0f;
		nbxInfo.valueMax = 5.0f;
		nbxInfo.command = command::TEXTURE_POS_V;
		MakeNumberbox( pPage, pPanel, nbxInfo );

		nbxInfo.labelText = "uScale";
		nbxInfo.fontSize = 16;
		nbxInfo.padding.y += paddingSpace;
		nbxInfo.value = valueTable.Get(nbxInfo.labelText).valueFloat;
		nbxInfo.valueMin = -50.0f;
		nbxInfo.valueMax = 50.0f;
		nbxInfo.command = command::TEXTURE_SCALE_U;
		MakeNumberbox( pPage, pPanel, nbxInfo );

		nbxInfo.labelText = "vScale";
		nbxInfo.padding.y += paddingSpace;
		nbxInfo.value = valueTable.Get(nbxInfo.labelText).valueFloat;
		nbxInfo.command = command::TEXTURE_SCALE_V;
		MakeNumberbox( pPage, pPanel, nbxInfo );
		
		nbxInfo.labelText = "texRot";
		nbxInfo.padding.y += paddingSpace;
		nbxInfo.value = valueTable.Get(nbxInfo.labelText).valueFloat;
		nbxInfo.valueMin = -360.0f;
		nbxInfo.valueMax = 360.0f;
		nbxInfo.command = command::TEXTURE_ROTATION;
		MakeNumberbox( pPage, pPanel, nbxInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_Door
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_Door(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_DOOR;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );

	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	int defaultValueIndex = 0;
	String labelTexts[] = { "red", "green", "blue", "emissive", "ambient", "diffuse", "specular", "specPower" };
	String sufixes[] = { "colorRed", "colorGreen", "colorBlue", "emissiveIntensity", "ambientIntensity", "diffuseIntensity", "specularIntensity", "specularPower" };
	String commands[] = { 
		command::COLOR_RED, 
		command::COLOR_GREEN, 
		command::COLOR_BLUE,
		command::EMISSIVE_INTENSITY,
		command::AMBIENT_INTENSITY, 
		command::DIFFUSE_INTENSITY, 
		command::SPECULAR_INTENSITY, 
		command::SPECULAR_POWER,
	};
	math::Vec2 values[] = {
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(8.0f, 0.7f),
	};

	TextboxInfo textboxInfo;
	textboxInfo.padding.x = 10;
	textboxInfo.padding.y = 15;
	textboxInfo.fontSize = 15;
	textboxInfo.nameSufix = "name";
	textboxInfo.labelText = "Name";
	textboxInfo.layer = 8;
	textboxInfo.maxChars = 30;
	textboxInfo.charsAllowed = GUI_TEXT_ALLOW_LETTER | GUI_TEXT_ALLOW_NUMBER | GUI_TEXT_ALLOW_UNDERSCORE;
	textboxInfo.value = valueTable.Get(textboxInfo.nameSufix).valueStr;
	textboxInfo.pFunction = gui_entityProcessCommand_String;
	textboxInfo.command = command::NAME;
	MakeTextbox( pPage, pPanel, textboxInfo );

	math::Vec2 padding;
	float paddingSpace = 25;

	SliderInfo sliderInfo;
	sliderInfo.padding.x = textboxInfo.padding.x;
	sliderInfo.padding.y = 50;
	sliderInfo.fontSize = 18;
	sliderInfo.pFunction = gui_entityProcessCommand_Float;
	sliderInfo.layer = 8;

	CHECK( ARRAY_COUNT(labelTexts) == ARRAY_COUNT(sufixes) && ARRAY_COUNT(commands) == ARRAY_COUNT(values) && ARRAY_COUNT(labelTexts) == ARRAY_COUNT(commands) );

	for ( uint i = 0; i < ARRAY_COUNT(labelTexts); i++ )
	{
		sliderInfo.nameSufix = sufixes[i];
		sliderInfo.labelText = labelTexts[i];
		sliderInfo.command = commands[i];
		sliderInfo.valueMin = values[i].x;
		sliderInfo.valueMax = values[i].y;
		sliderInfo.value = valueTable.Get(sliderInfo.nameSufix).valueFloat;
		MakeSlider( pPage, pPanel, sliderInfo );
		sliderInfo.padding.y += paddingSpace;
		sliderInfo.nameSufix = "";
	}

	ButtonTextInfo buttonTextInfo;
	// Color map button
	{
		buttonTextInfo.nameSufix = "colorMap";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Top, 25, (int)sliderInfo.padding.y );
		buttonTextInfo.configButton.dimension = math::Vec2( 115, 35 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_openTextureSelectMenu;
		buttonTextInfo.configButton.flag = (uint)eGuiTexture::DiffuseMap;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 22;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "COLOR MAP";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Normal map button
	{
		buttonTextInfo.nameSufix = "normalMap";
		buttonTextInfo.configButton.anchor.horizontal = eGuiAnchorHorizontal::Right;
		buttonTextInfo.configButton.pClickFunction = button_openTextureSelectMenu;
		buttonTextInfo.configButton.flag = (uint)eGuiTexture::NormalMap;
		buttonTextInfo.text.label = "NORMAL MAP";

		padding = MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Color map transform
	{
		paddingSpace = 15.0f;
		sliderInfo.labelText = "uPos";
		sliderInfo.padding.y = padding.y + paddingSpace;
		sliderInfo.value = valueTable.Get(sliderInfo.labelText).valueFloat;
		sliderInfo.valueMin = -1.0f;
		sliderInfo.valueMax = 1.0f;
		sliderInfo.pFunction = gui_entityProcessCommand_Float;
		sliderInfo.command = command::TEXTURE_POS_U;
		MakeSlider( pPage, pPanel, sliderInfo );
		paddingSpace = 23.0f;

		sliderInfo.labelText = "vPos";
		sliderInfo.padding.y += paddingSpace;
		sliderInfo.value = valueTable.Get(sliderInfo.labelText).valueFloat;
		sliderInfo.command = command::TEXTURE_POS_V;
		MakeSlider( pPage, pPanel, sliderInfo );

		sliderInfo.labelText = "uScale";
		sliderInfo.padding.y += paddingSpace;
		sliderInfo.value = valueTable.Get(sliderInfo.labelText).valueFloat;
		sliderInfo.valueMin = 0.01f;
		sliderInfo.valueMax = 5.01f;
		sliderInfo.command = command::TEXTURE_SCALE_U;
		MakeSlider( pPage, pPanel, sliderInfo );

		sliderInfo.labelText = "vScale";
		sliderInfo.padding.y += paddingSpace;
		sliderInfo.value = valueTable.Get(sliderInfo.labelText).valueFloat;
		sliderInfo.valueMin = 0.01f;
		sliderInfo.valueMax = 5.01f;
		sliderInfo.command = command::TEXTURE_SCALE_V;
		MakeSlider( pPage, pPanel, sliderInfo );

		sliderInfo.labelText = "texRot";
		sliderInfo.padding.y += paddingSpace;
		sliderInfo.value = valueTable.Get(sliderInfo.labelText).valueFloat;
		sliderInfo.valueMin = 0.0f;
		sliderInfo.valueMax = 360.0f;
		sliderInfo.command = command::TEXTURE_ROTATION;
		MakeSlider( pPage, pPanel, sliderInfo );
	}

	// Door pick axis
	{
		buttonTextInfo.nameSufix = "pickAxis";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Top, 25, (int)(sliderInfo.padding.y + paddingSpace) );
		buttonTextInfo.configButton.dimension = math::Vec2( 230, 35 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_pickAxis;
		buttonTextInfo.configButton.flag = (uint)eGuiTexture::DiffuseMap;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 20;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "PICK DOOR OPEN DIRECTION";

		padding = MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Slider door
	{
		sliderInfo.labelText = "openAmount";
		sliderInfo.padding.y = padding.y + buttonTextInfo.text.charSize + 5.0f;
		sliderInfo.value = valueTable.Get(sliderInfo.labelText).valueFloat;
		sliderInfo.valueMin = 0.0f;
		sliderInfo.valueMax = 1.0f;
		sliderInfo.pFunction = gui_entityProcessCommand_Float;
		sliderInfo.command = command::OPEN_AMOUNT;
		padding.y = MakeSlider( pPage, pPanel, sliderInfo );
	}

	// Door open duration
	{
		NumberboxInfo numberboxInfo;
		numberboxInfo.labelText = "duration";
		numberboxInfo.fontSize = sliderInfo.fontSize;
		numberboxInfo.padding.x = sliderInfo.padding.x;
		numberboxInfo.padding.y = padding.y + buttonTextInfo.text.charSize - 2;
		numberboxInfo.value = valueTable.Get(numberboxInfo.labelText).valueFloat;
		numberboxInfo.pFunction = gui_entityProcessCommand_Float;
		numberboxInfo.valueMin = 0.1f;
		numberboxInfo.valueMax = 600.0f;
		numberboxInfo.command = command::DOOR_DURATION;
		numberboxInfo.layer = sliderInfo.layer;
		MakeNumberbox( pPage, pPanel, numberboxInfo );
		padding.y += numberboxInfo.fontSize + 18.0f;
	}

	// Locked
	CheckboxInfo checkBoxInfo;
	{
		checkBoxInfo.labelText = "locked";
		checkBoxInfo.padding.x = sliderInfo.padding.x;
		checkBoxInfo.padding.y = padding.y + buttonTextInfo.text.charSize - 2.0f;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.labelText).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::LOCKED;
		checkBoxInfo.fontSize = sliderInfo.fontSize;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
		padding.y += 30.0f;
	}

	{
		buttonTextInfo.nameSufix = "InsertLockTextLines";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Center, eGuiAnchorVertical::Top, 0, (int)(padding.y + paddingSpace) );
		buttonTextInfo.configButton.dimension = math::Vec2( pPanel->GetScale().x * 1.6f, 35 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_openTextArea;
		buttonTextInfo.configButton.flag = (uint)eEditorEntityType::Unknown;
		buttonTextInfo.text.label = "LOCK TEXT";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_CameraTrack
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_CameraTrack()
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_CAMERA_TRACK;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	ButtonTextInfo buttonTextInfo;
	{
		buttonTextInfo.nameSufix = "Connect";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Top, 10, 0);
		buttonTextInfo.configButton.dimension = math::Vec2( 150, 40 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_pickEntity;
		buttonTextInfo.configButton.flag = (uint)eEditorEntityType::CameraTrack;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 24;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "CONNECT";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	{
		buttonTextInfo.nameSufix = "Disconnect";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Right, eGuiAnchorVertical::Top, 0, 0);
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_cameraTrackDisconnect;
		buttonTextInfo.configButton.flag = 0;
		buttonTextInfo.text.label = "DISCONNECT";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_AmbientLight
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_AmbientLight(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_AMBIENT_LIGHT;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	String labelTexts[] = { "ambRed", "ambGreen", "ambBlue", };
	String sufixes[] = { "ambientRed", "ambientGreen", "ambientBlue", };
	String commands[] = { 
		command::AMBIENT_COLOR_RED,
		command::AMBIENT_COLOR_GREEN, 
		command::AMBIENT_COLOR_BLUE, 
	};
	math::Vec2 values[] = {
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
	};

	float verticalPadding = 20.0f;

	SliderInfo info;
	info.padding.x = 10.0f;
	info.fontSize = 20;
	info.pFunction = gui_entityProcessCommand_Float;
	info.layer = 8;

	CHECK( ARRAY_COUNT(labelTexts) == ARRAY_COUNT(sufixes) && ARRAY_COUNT(commands) == ARRAY_COUNT(values) && ARRAY_COUNT(labelTexts) == ARRAY_COUNT(commands) );

	for ( uint i = 0; i < ARRAY_COUNT(sufixes); i++ )
	{
		info.nameSufix = sufixes[i];
		info.labelText = labelTexts[i];
		info.padding.y = verticalPadding;
		info.command = commands[i];
		info.valueMin = values[i].x;
		info.valueMax = values[i].y;
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		MakeSlider( pPage, pPanel, info );
		info.nameSufix = "";
		verticalPadding += 24.0f;
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_PointLight
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_PointLight(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_POINT_LIGHT;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	String labelTexts[] = { "range", "diffRed", "diffGreen", "diffBlue", "specular", "atten0", "atten1", "atten2" };
	String sufixes[] = { "range", "diffuseRed", "diffuseGreen", "diffuseBlue", "specularRgb", "attenuation0", "attenuation1", "attenuation2" };
	String commands[] = { 
		command::LIGHT_RANGE,
		command::DIFFUSE_COLOR_RED, 
		command::DIFFUSE_COLOR_GREEN, 
		command::DIFFUSE_COLOR_BLUE,
		command::SPECULAR_COLOR_RGB, 
		command::LIGHT_ATTENUATION_0,
		command::LIGHT_ATTENUATION_1,
		command::LIGHT_ATTENUATION_2,
	};
	math::Vec2 values[] = {
		math::Vec2(0.0f, 50.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.05f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 3.0f),
	};

	float verticalPadding = 10.0f;

	SliderInfo info;
	info.padding.x = 10.0f;
	info.fontSize = 20;
	info.pFunction = gui_entityProcessCommand_Float;
	info.layer = 8;

	CHECK( ARRAY_COUNT(labelTexts) == ARRAY_COUNT(sufixes) && ARRAY_COUNT(commands) == ARRAY_COUNT(values) && ARRAY_COUNT(labelTexts) == ARRAY_COUNT(commands) );

	for ( uint i = 0; i < ARRAY_COUNT(sufixes); i++ )
	{
		info.nameSufix = sufixes[i];
		info.labelText = labelTexts[i];
		info.padding.y = verticalPadding;
		info.command = commands[i];
		info.valueMin = values[i].x;
		info.valueMax = values[i].y;
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		MakeSlider( pPage, pPanel, info );
		info.nameSufix = "";
		verticalPadding += 20.0f;
	}

	// Flicker
	CheckboxInfo checkBoxInfo;
	{
		checkBoxInfo.labelText = "flicker";
		checkBoxInfo.nameSufix = checkBoxInfo.labelText;
		checkBoxInfo.padding.x = info.padding.x;
		checkBoxInfo.padding.y = verticalPadding;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::LIGHT_FLICKER;
		checkBoxInfo.fontSize = info.fontSize;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
		verticalPadding += 30.0f;
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_SpotLight
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_SpotLight(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_SPOT_LIGHT;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );

	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	String labelTexts[] = { "range", "cone", "falloff", "diffRed", "diffGreen", "diffBlue", "specular", "atten0", "atten1", "atten2" };
	String sufixes[] = { "range", "innerCone", "outerCone", "diffuseRed", "diffuseGreen", "diffuseBlue", "specularRgb", "attenuation0", "attenuation1", "attenuation2" };
	String commands[] = { 
		command::LIGHT_RANGE,
		command::LIGHT_INNER_CONE_DEGS,
		command::LIGHT_OUTER_CONE_DEGS,
		command::DIFFUSE_COLOR_RED, 
		command::DIFFUSE_COLOR_GREEN, 
		command::DIFFUSE_COLOR_BLUE,
		command::SPECULAR_COLOR_RGB, 
		command::LIGHT_ATTENUATION_0,
		command::LIGHT_ATTENUATION_1,
		command::LIGHT_ATTENUATION_2,
	};
	math::Vec2 values[] = {
		math::Vec2(0.0f, 100.0f),
		math::Vec2(0.0f, 180.0f),
		math::Vec2(0.0f, 90.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.05f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 3.0f),
	};

	float verticalPadding = 10.0f;

	SliderInfo info;
	info.padding.x = 10.0f;
	info.fontSize = 20;
	info.pFunction = gui_entityProcessCommand_Float;
	info.layer = 8;

	CHECK( ARRAY_COUNT(labelTexts) == ARRAY_COUNT(sufixes) && ARRAY_COUNT(commands) == ARRAY_COUNT(values) && ARRAY_COUNT(labelTexts) == ARRAY_COUNT(commands) );

	for ( uint i = 0; i < ARRAY_COUNT(sufixes); i++ )
	{
		info.nameSufix = sufixes[i];
		info.labelText = labelTexts[i];
		info.padding.y = verticalPadding;
		info.command = commands[i];
		info.valueMin = values[i].x;
		info.valueMax = values[i].y;
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		MakeSlider( pPage, pPanel, info );
		info.nameSufix = "";
		verticalPadding += 20.0f;
	}

	// ShadowEnabled
	CheckboxInfo checkBoxInfo;
	{
		checkBoxInfo.labelText = "shadow";
		checkBoxInfo.nameSufix = "shadowEnabled";
		checkBoxInfo.padding.x = info.padding.x;
		checkBoxInfo.padding.y = verticalPadding;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::SHADOW_ENABLED;
		checkBoxInfo.fontSize = info.fontSize;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
		verticalPadding += 30.0f;
	}

	// ShadowBias
	{
		info.labelText = "bias";
		info.nameSufix = "shadowBias";
		info.padding = checkBoxInfo.padding + math::Vec2(0.0f, (float)checkBoxInfo.fontSize * 1.2f + 10.0f);
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		info.valueMin = 0.0f;
		info.valueMax = 1.0f;
		info.command = command::SHADOW_BIAS;
		verticalPadding = MakeSlider( pPage, pPanel, info );
	}

	// Shadow distance adjustment
	{
		info.labelText = "distAdjust";
		info.nameSufix = info.labelText;
		info.padding.y = verticalPadding + 20.0f;
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		info.defaultValue.use = true;
		info.defaultValue.value = 0.0f;
		info.valueMin = -5.0f;
		info.valueMax = 5.0f;
		info.command = command::SHADOW_DISTANCE_ADJUSTMENT;
		verticalPadding = MakeSlider( pPage, pPanel, info );
	}

		// Flicker
	{
		checkBoxInfo.labelText = "flicker";
		checkBoxInfo.nameSufix = checkBoxInfo.labelText;
		checkBoxInfo.padding.y = verticalPadding + 20.0f;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::LIGHT_FLICKER;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
		verticalPadding += 30.0f;
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_TriggerCamera
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_TriggerCamera()
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_TRIGGER_CAMERA;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	ButtonTextInfo buttonTextInfo;
	{
		buttonTextInfo.nameSufix = "Connect";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Top, 10, 20);
		buttonTextInfo.configButton.dimension = math::Vec2( 100, 30 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_pickEntity;
		buttonTextInfo.configButton.flag = (uint)eEditorEntityType::Unknown;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 20;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "CONNECT";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_TriggerCommand
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_TriggerCommand(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_TRIGGER_COMMAND;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	ButtonTextInfo buttonTextInfo;
	{
		buttonTextInfo.nameSufix = "InsertCommands";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Center, eGuiAnchorVertical::Top, 0, 20);
		buttonTextInfo.configButton.dimension = math::Vec2( pPanel->GetScale().x * 1.6f, 35 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_openTextArea;
		buttonTextInfo.configButton.flag = (uint)eEditorEntityType::Unknown;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 24;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "INSERT COMMANDS";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Locked
	CheckboxInfo checkBoxInfo;
	{
		checkBoxInfo.nameSufix = "FireEventOnce";
		checkBoxInfo.labelText = "Fire once";
		checkBoxInfo.padding.x = 15;
		checkBoxInfo.padding.y = 65;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::EVENT_FIRE_ONCE;
		checkBoxInfo.fontSize = 16;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
	}

	{
		checkBoxInfo.nameSufix = "Active";
		checkBoxInfo.labelText = "Active";
		checkBoxInfo.padding.x = 15;
		checkBoxInfo.padding.y = 90;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::ACTIVE;
		checkBoxInfo.fontSize = 16;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_Model
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_Model(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_MODEL;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );

	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	int defaultValueIndex = 0;
	String labelTexts[] = { "red", "green", "blue", "emissive", "ambient", "diffuse", "specular", "specPower" };
	String sufixes[] = { "colorRed", "colorGreen", "colorBlue", "emissiveIntensity", "ambientIntensity", "diffuseIntensity", "specularIntensity", "specularPower" };
	String commands[] = { 
		command::COLOR_RED, 
		command::COLOR_GREEN, 
		command::COLOR_BLUE,
		command::EMISSIVE_INTENSITY,
		command::AMBIENT_INTENSITY, 
		command::DIFFUSE_INTENSITY, 
		command::SPECULAR_INTENSITY, 
		command::SPECULAR_POWER,
	};
	math::Vec2 values[] = {
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(8.0f, 0.7f),
	};

	math::Vec2 padding;
	float paddingSpace = 25;
	float paddingLeft = 10;

	{
		TextboxInfo textboxInfo;
		textboxInfo.padding.x = paddingLeft;
		textboxInfo.padding.y = 10;
		textboxInfo.fontSize = 15;
		textboxInfo.nameSufix = "name";
		textboxInfo.labelText = "Name";
		textboxInfo.layer = 8;
		textboxInfo.maxChars = 30;
		textboxInfo.charsAllowed = GUI_TEXT_ALLOW_LETTER | GUI_TEXT_ALLOW_NUMBER | GUI_TEXT_ALLOW_UNDERSCORE;
		textboxInfo.value = valueTable.Get(textboxInfo.nameSufix).valueStr;
		textboxInfo.pFunction = gui_entityProcessCommand_String;
		textboxInfo.command = command::NAME;
		MakeTextbox( pPage, pPanel, textboxInfo );
	}

	SliderInfo info;
	info.padding.x = paddingLeft;
	info.padding.y = 45;
	info.fontSize = 18;
	info.pFunction = gui_entityProcessCommand_Float;
	info.layer = 8;

	CHECK( ARRAY_COUNT(labelTexts) == ARRAY_COUNT(sufixes) && ARRAY_COUNT(commands) == ARRAY_COUNT(values) && ARRAY_COUNT(labelTexts) == ARRAY_COUNT(commands) );

	for ( uint i = 0; i < ARRAY_COUNT(labelTexts); i++ )
	{
		info.nameSufix = sufixes[i];
		info.labelText = labelTexts[i];
		info.command = commands[i];
		info.valueMin = values[i].x;
		info.valueMax = values[i].y;
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		MakeSlider( pPage, pPanel, info );
		info.padding.y += paddingSpace;
		info.nameSufix = "";
	}

	ButtonTextInfo buttonTextInfo;
	// Color map button
	{
		buttonTextInfo.nameSufix = "model";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Top, 25, (int)info.padding.y );
		buttonTextInfo.configButton.dimension = math::Vec2( 115, 35 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_openModelSelectMenu;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 22;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "MODEL";

		MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_Talk
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_Talk(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_TALK;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	float paddingLeft = 15.0f;
	pPage = pGui->CreatePage( pageName );
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	TextboxInfo textboxInfo;
	textboxInfo.padding.x = 10;
	textboxInfo.padding.y = paddingLeft;
	textboxInfo.fontSize = 15;
	textboxInfo.nameSufix = "name";
	textboxInfo.labelText = "Name";
	textboxInfo.layer = 8;
	textboxInfo.maxChars = 30;
	textboxInfo.charsAllowed = GUI_TEXT_ALLOW_LETTER | GUI_TEXT_ALLOW_NUMBER | GUI_TEXT_ALLOW_UNDERSCORE;
	textboxInfo.value = valueTable.Get(textboxInfo.nameSufix).valueStr;
	textboxInfo.pFunction = gui_entityProcessCommand_String;
	textboxInfo.command = command::NAME;
	MakeTextbox( pPage, pPanel, textboxInfo );

	ButtonTextInfo buttonTextInfo;
	math::Vec2 buttonPadding;
	int paddingTop = 10;
	{
		buttonTextInfo.nameSufix = "InsertTextLines";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Center, eGuiAnchorVertical::Top, 0, 40 + paddingTop);
		buttonTextInfo.configButton.dimension = math::Vec2( pPanel->GetScale().x * 1.6f, 35 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_openTextArea;
		buttonTextInfo.configButton.flag = editorGlobal::gui::flag::FLAG_ENTITY_TALK_TEXT.GetHash();
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.charSize = 24;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);
		buttonTextInfo.text.label = "TEXT LINES";

		buttonPadding = MakeButtonText( pPage, pPanel, buttonTextInfo );

		buttonTextInfo.nameSufix = "InsertLines";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Center, eGuiAnchorVertical::Top, 0, (int)buttonPadding.y + paddingTop);
		buttonTextInfo.configButton.flag = editorGlobal::gui::flag::FLAG_ENTITY_TALK_COMMAND.GetHash();
		buttonTextInfo.text.label = "EVENT ON INTERACT";

		buttonPadding = MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Locked
	CheckboxInfo checkBoxInfo;
	{
		checkBoxInfo.nameSufix = "FireEventOnce";
		checkBoxInfo.labelText = "Fire once";
		checkBoxInfo.padding.x = paddingLeft;
		checkBoxInfo.padding.y = buttonPadding.y + buttonTextInfo.text.charSize - 2.0f;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::EVENT_FIRE_ONCE;
		checkBoxInfo.fontSize = 16;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_Partition
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_Partition(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_PARTITION;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	float paddingLeft = 15.0f;
	pPage = pGui->CreatePage( pageName );
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	TextboxInfo textboxInfo;
	textboxInfo.padding.x = 10;
	textboxInfo.padding.y = paddingLeft;
	textboxInfo.fontSize = 15;
	textboxInfo.nameSufix = "AssertOnEntityContact";
	textboxInfo.labelText = "DbgEnt";
	textboxInfo.layer = 8;
	textboxInfo.maxChars = 20;
	textboxInfo.charsAllowed = GUI_TEXT_ALLOW_LETTER | GUI_TEXT_ALLOW_NUMBER | GUI_TEXT_ALLOW_UNDERSCORE;
	textboxInfo.value = valueTable.Get(textboxInfo.nameSufix).valueStr;
	textboxInfo.pFunction = gui_entityProcessCommand_String;
	textboxInfo.command = command::DEBUG_ENTITY_CONTACT;
	MakeTextbox( pPage, pPanel, textboxInfo );

	int paddingTop = 30;

	// Debug on add entity
	CheckboxInfo checkBoxInfo;
	{
		checkBoxInfo.nameSufix = "AssertOnAddEntity";
		checkBoxInfo.labelText = "DbgAddEnt";
		checkBoxInfo.padding.x = paddingLeft;
		checkBoxInfo.padding.y = paddingTop + textboxInfo.fontSize - 2.0f;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::DEBUG_ADD_ENTITY;
		checkBoxInfo.fontSize = 16;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
	}

	// Debug create partition
	{
		checkBoxInfo.nameSufix = "AssertOnCreatePartition";
		checkBoxInfo.labelText = "DbgCreate";
		checkBoxInfo.padding.x = paddingLeft;
		checkBoxInfo.padding.y = checkBoxInfo.padding.y + paddingTop + checkBoxInfo.fontSize - 10.0f;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::DEBUG_CREATE_PARTITION;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_Refraction
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_Refraction(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_REFRACTION;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	pPage = pGui->CreatePage( pageName );
	
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	int defaultValueIndex = 0;
	String labelTexts[] = { "red", "green", "blue", "refraction", "saturation", "separation", };
	String sufixes[] = { "colorRed", "colorGreen", "colorBlue", "refractionIntensity", "colorSaturation", "colorSeparation", };
	String commands[] = { 
		command::COLOR_RED, 
		command::COLOR_GREEN, 
		command::COLOR_BLUE,
		command::REFRACTION_INTENSITY,
		command::COLOR_SATURATION,
		command::COLOR_SEPARATION,
	};
	math::Vec2 values[] = {
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 0.1f),
		math::Vec2(0.0f, 1.0f),
		math::Vec2(0.0f, 0.02f),
	};

	math::Vec2 padding;
	float paddingSpace = 25;

	SliderInfo info;
	info.padding.x = 10;
	info.padding.y = 15;
	info.fontSize = 18;
	info.pFunction = gui_entityProcessCommand_Float;
	info.layer = 8;
	
	CHECK( ARRAY_COUNT(labelTexts) == ARRAY_COUNT(sufixes) && ARRAY_COUNT(commands) == ARRAY_COUNT(values) && ARRAY_COUNT(labelTexts) == ARRAY_COUNT(commands) );

	for ( uint i = 0; i < ARRAY_COUNT(labelTexts); i++ )
	{
		info.nameSufix = sufixes[i];
		info.labelText = labelTexts[i];
		info.command = commands[i];
		info.valueMin = values[i].x;
		info.valueMax = values[i].y;
		info.value = valueTable.Get(info.nameSufix).valueFloat;
		MakeSlider( pPage, pPanel, info );
		info.padding.y += paddingSpace;
		info.nameSufix = "";
	}

	ButtonTextInfo buttonTextInfo;

	// Normal map button
	{
		buttonTextInfo.nameSufix = "normalMap";
		buttonTextInfo.configButton.anchor = GuiAnchor(eGuiAnchorHorizontal::Left, eGuiAnchorVertical::Top, 25, (int)info.padding.y );
		buttonTextInfo.configButton.dimension = math::Vec2( 115, 35 );
		buttonTextInfo.configButton.pAnimation = CreateAnimation_Button();
		buttonTextInfo.configButton.pClickFunction = button_openTextureSelectMenu;
		buttonTextInfo.configButton.flag = (uint)eGuiTexture::NormalMap;
		buttonTextInfo.configButton.layer = 8;
		buttonTextInfo.text.label = "NORMAL MAP";
		buttonTextInfo.text.charSize = 22;
		buttonTextInfo.text.color4 = math::Vec3(0.16f).Xyzw(1.0f);

		padding = MakeButtonText( pPage, pPanel, buttonTextInfo );
	}

	// Color map transform
	{
		paddingSpace = 15.0f;
		info.labelText = "uPos";
		info.padding.y = padding.y + paddingSpace;
		info.value = valueTable.Get(info.labelText).valueFloat;
		info.valueMin = -1.0f;
		info.valueMax = 1.0f;
		info.pFunction = gui_entityProcessCommand_Float;
		info.command = command::TEXTURE_POS_U;
		MakeSlider( pPage, pPanel, info );
		paddingSpace = 23.0f;

		info.labelText = "vPos";
		info.padding.y += paddingSpace;
		info.value = valueTable.Get(info.labelText).valueFloat;
		info.pFunction = gui_entityProcessCommand_Float;
		info.command = command::TEXTURE_POS_V;
		MakeSlider( pPage, pPanel, info );

		info.labelText = "uScale";
		info.padding.y += paddingSpace;
		info.value = valueTable.Get(info.labelText).valueFloat;
		info.valueMin = 0.01f;
		info.valueMax = 5.01f;
		info.pFunction = gui_entityProcessCommand_Float;
		info.command = command::TEXTURE_SCALE_U;
		MakeSlider( pPage, pPanel, info );

		info.labelText = "vScale";
		info.padding.y += paddingSpace;
		info.value = valueTable.Get(info.labelText).valueFloat;
		info.valueMin = 0.01f;
		info.valueMax = 5.01f;
		info.pFunction = gui_entityProcessCommand_Float;
		info.command = command::TEXTURE_SCALE_V;
		MakeSlider( pPage, pPanel, info );

		info.labelText = "texRot";
		info.padding.y += paddingSpace;
		info.value = valueTable.Get(info.labelText).valueFloat;
		info.valueMin = 0.0f;
		info.valueMax = 360.0f;
		info.pFunction = gui_entityProcessCommand_Float;
		info.command = command::TEXTURE_ROTATION;
		MakeSlider( pPage, pPanel, info );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePageProperty_Monster
//-------------------------------------------------------------------
GuiPage* GuiPageControl::CreatePageProperty_Monster(PropertyNameValueTable& valueTable)
{
	HashString pageName = editorGlobal::guiPage::PAGE_PROPERTY_REFRACTION;
	EditorGui* pGui = GetGui();
	GuiPage* pPage = pGui->FindPage( pageName );

	if ( pPage )
		return pPage;

	float paddingLeft = 15.0f;
	pPage = pGui->CreatePage( pageName );
	
	GuiComponent_Panel* pPanel = CreatePropertyPanel( pPage );

	TextboxInfo textboxInfo;
	textboxInfo.padding.x = 10;
	textboxInfo.padding.y = paddingLeft;
	textboxInfo.fontSize = 15;
	textboxInfo.nameSufix = "name";
	textboxInfo.labelText = "Name";
	textboxInfo.layer = 8;
	textboxInfo.maxChars = 30;
	textboxInfo.charsAllowed = GUI_TEXT_ALLOW_LETTER | GUI_TEXT_ALLOW_NUMBER | GUI_TEXT_ALLOW_UNDERSCORE;
	textboxInfo.value = valueTable.Get(textboxInfo.nameSufix).valueStr;
	textboxInfo.pFunction = gui_entityProcessCommand_String;
	textboxInfo.command = command::NAME;
	MakeTextbox( pPage, pPanel, textboxInfo );


	// Spawn on create
	CheckboxInfo checkBoxInfo;
	{
		checkBoxInfo.nameSufix = "SpawnOnCreate";
		checkBoxInfo.labelText = "Spawn";
		checkBoxInfo.padding.x = paddingLeft;
		checkBoxInfo.padding.y = 50.0f;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::SPAWN_ON_CREATE;
		checkBoxInfo.fontSize = 16;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
	}

	// Active
	{
		checkBoxInfo.nameSufix = "Active";
		checkBoxInfo.labelText = "Active";
		checkBoxInfo.padding.x = paddingLeft;
		checkBoxInfo.padding.y = checkBoxInfo.padding.y + checkBoxInfo.fontSize + 20.0f;
		checkBoxInfo.value = valueTable.Get(checkBoxInfo.nameSufix).valueBool;
		checkBoxInfo.pFunction = gui_entityProcessCommand_Bool;
		checkBoxInfo.command = command::ACTIVE;
		MakeCheckbox( pPage, pPanel, checkBoxInfo );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	CreatePropertyPanel
//-------------------------------------------------------------------
GuiComponent_Panel* GuiPageControl::CreatePropertyPanel(GuiPage* pPage)
{
	GuiComponent_Panel* pResult = pPage->Add<GuiComponent_Panel>( "pnl_propertyMenu" );

	{
		GuiConfig_Panel configPanel;

		configPanel.pAnimation = CreateAnimation_StrechAlphaWeak();
		configPanel.dimension = math::Vec2( 300, 650 );
		configPanel.anchor.horizontal = eGuiAnchorHorizontal::Right;
		configPanel.anchor.vertical = eGuiAnchorVertical::Top;
		configPanel.anchor.paddingHorizontal = 10;
		configPanel.anchor.paddingVertical = (int)g_pRenderer->GetWindowViewport().GetDimensionScaled(1.0f, 0.05f).y;
		configPanel.layer = 9;
		configPanel.color4 = math::normalizeRgb( 151, 235, 255 );

		pResult->SetConfig( configPanel );
	}

	return pResult;
}
//-------------------------------------------------------------------
//	CreateAnimation_StrechAlphaWeak
//-------------------------------------------------------------------
GuiAnimation* GuiPageControl::CreateAnimation_StrechAlphaWeak()
{
	GuiAnimation* pResult = new GuiAnimation;
	GuiAnimationData anim;

	anim.scale.x = 0.0f;
	anim.scale.y = 0.8f;
	anim.saturate = 1.0f;
	anim.alpha = 0.0f;
	pResult->Add( anim, 0 );

	anim.scale.x = 0.0f;
	anim.scale.y = 0.8f;
	anim.saturate = 1.0f;
	anim.alpha = 0.0f;
	pResult->Add( anim, 40 );

	anim.scale.x = 0.95f;
	anim.scale.y = 0.9f;
	anim.saturate = 1.0f;
	anim.alpha = 0.7f;
	pResult->Add( anim, 70 );

	anim.scale.x = 1.0f;
	anim.scale.y = 1.0f;
	anim.saturate = 0.0f;
	anim.alpha = 1.0f;
	pResult->Add( anim, 100 );

	return pResult;
}
//-------------------------------------------------------------------
//	CreateAnimation_StrechInsideOut
//-------------------------------------------------------------------
GuiAnimation* GuiPageControl::CreateAnimation_StrechInsideOut()
{
	GuiAnimation* pResult = new GuiAnimation;
	GuiAnimationData anim;

	anim.scale.x = 0.0f;
	anim.scale.y = 0.8f;
	anim.saturate = 1.0f;
	pResult->Add( anim, 0 );

	anim.scale.x = 0.7f;
	anim.scale.y = 0.7f;
	anim.saturate = 0.5f;
	pResult->Add( anim, 60 );

	anim.scale.x = 1.0f;
	anim.scale.y = 1.0f;
	anim.saturate = 0.0f;
	pResult->Add( anim, 100 );

	return pResult;
}
//-------------------------------------------------------------------
//	CreateAnimation_StrechElastic
//-------------------------------------------------------------------
GuiAnimation* GuiPageControl::CreateAnimation_StrechElastic()
{
	GuiAnimation* pResult = new GuiAnimation();
	GuiAnimationData anim;

	anim.scale.x = 0.9f;
	anim.scale.y = 1.0f;
	anim.saturate = 1.0f;
	anim.alpha = 0.4f;
	pResult->Add( anim, 0 );

	anim.scale.x = 1.1f;
	anim.scale.y = 0.9f;
	anim.saturate = 0.5f;
	anim.alpha = 0.8f;
	pResult->Add( anim, 60 );

	anim.scale.x = 1.0f;
	anim.scale.y = 1.0f;
	anim.saturate = 0.0f;
	anim.alpha = 1.0f;
	pResult->Add( anim, 100 );

	return pResult;
}
//-------------------------------------------------------------------
//	CreateAnimation_Button
//-------------------------------------------------------------------
GuiAnimation* GuiPageControl::CreateAnimation_Button()
{
	GuiAnimation* pResult = new GuiAnimation;
	GuiAnimationData anim;

	anim.scale.x = 0.0f;
	anim.scale.y = 0.0f;
	anim.alpha = 0.0f;
	pResult->Add( anim, 0 );

	anim.scale.x = 0.45f;
	anim.scale.y = 0.50f;
	anim.alpha = 0.0f;
	pResult->Add( anim, 55 );

	anim.scale.x = 0.50f;
	anim.scale.y = 0.55f;
	anim.alpha = 0.2f;
	pResult->Add( anim, 60 );

	anim.scale.x = 1.11f;
	anim.scale.y = 1.09f;
	anim.alpha = 1.0f;
	pResult->Add( anim, 88 );

	anim.scale.x = 1.0f;
	anim.scale.y = 1.0f;
	pResult->Add( anim, 100 );

	return pResult;
}
//-------------------------------------------------------------------
//	MakeSlider
//-------------------------------------------------------------------
float GuiPageControl::MakeSlider(GuiPage* pPage, IGuiComponent* pParent, SliderInfo& info)
{
	String componentName = info.nameSufix.Length() > 0 ? info.nameSufix : string::remove( info.labelText, " " );
	GuiComponent_Text* pText = pPage->Add<GuiComponent_Text>( "txt_" + componentName,  pParent ? pParent->GetName() : "" );
	GuiComponent_Slider* pSlider = pPage->Add<GuiComponent_Slider>( prefix_slider + componentName, pParent ? pParent->GetName() : "" );

	// Text
	{
		GuiConfig_Text configText;

		configText.anchor.horizontal = eGuiAnchorHorizontal::Left;
		configText.anchor.vertical = eGuiAnchorVertical::Top;
		configText.anchor.paddingHorizontal = (int)info.padding.x;
		configText.anchor.paddingVertical = (int)info.padding.y - (int)(info.fontSize * 0.5f);
		configText.charSize = (int)info.fontSize;
		configText.color4 = math::normalizeRgb( 245, 245, 245 );
		configText.text = info.labelText;
		configText.layer = info.layer;
		configText.fontName = "consolas";

		pText->SetConfig( configText );
	}

	// Slider
	{
		GuiConfig_Slider configSlider;

		configSlider.anchor.horizontal = eGuiAnchorHorizontal::Right;
		configSlider.anchor.vertical = eGuiAnchorVertical::Top;
		configSlider.anchor.paddingHorizontal = 15;
		configSlider.anchor.paddingVertical = (int)info.padding.y;
		configSlider.command = info.command;

		configSlider.dimension.x = 170;
		configSlider.dimension.y = 7;
		configSlider.circleScale = 5;
		configSlider.defaultValue.use = info.defaultValue.use;
		configSlider.defaultValue.value = info.defaultValue.value;
		configSlider.minValue = info.valueMin;
		configSlider.maxValue = info.valueMax;

		configSlider.pSliderFunction = info.pFunction;
		configSlider.layer = info.layer;

		pSlider->SetConfig( configSlider );
		pSlider->SetValue( info.value );

		CHECK( info.pFunction );
	}

	return info.padding.y + pText->GetScale().y;
}
//-------------------------------------------------------------------
//	MakeButtonText
//-------------------------------------------------------------------
math::Vec2 GuiPageControl::MakeButtonText(GuiPage* pPage, IGuiComponent* pParent, ButtonTextInfo& info)
{
	GuiComponent_Button* pButton = pPage->Add<GuiComponent_Button>( "btn_" + info.nameSufix, pParent ? pParent->GetName() : "" );
	
	pButton->SetConfig( info.configButton );

	// Text
	{
		GuiConfig_Text configText;

		configText.anchor.horizontal = eGuiAnchorHorizontal::Center;
		configText.anchor.vertical = eGuiAnchorVertical::Center;
		configText.charSize = (int)info.text.charSize;
		configText.color4 = info.text.color4;
		configText.text = info.text.label;
		configText.layer = info.configButton.layer - 1;
		configText.fontName = "consolas";

		GuiComponent_Text* pText = pPage->Add<GuiComponent_Text>( "txt_" + info.nameSufix, pButton->GetName() );
		pText->SetConfig( configText );
	}

	return pButton->GetPos() - pButton->GetScale();
}
//-------------------------------------------------------------------
//	MakeTextbox
//-------------------------------------------------------------------
void GuiPageControl::MakeTextbox(GuiPage* pPage, IGuiComponent* pParent, TextboxInfo& info)
{
	String componentName = info.nameSufix.Length() > 0 ? info.nameSufix : string::remove( info.labelText, " " );
	GuiComponent_Text* pText = pPage->Add<GuiComponent_Text>( "txt_" + componentName,  pParent ? pParent->GetName() : "" );
	GuiComponent_Textbox* pTextbox = pPage->Add<GuiComponent_Textbox>( prefix_textbox + componentName, pParent ? pParent->GetName() : "" );

	// Text
	{
		GuiConfig_Text configText;

		configText.anchor.horizontal = eGuiAnchorHorizontal::Left;
		configText.anchor.vertical = eGuiAnchorVertical::Top;
		configText.anchor.paddingHorizontal = (int)info.padding.x;
		configText.anchor.paddingVertical = (int)info.padding.y + 2;
		configText.charSize = (int)info.fontSize;
		configText.color4 = math::normalizeRgb( 245 );
		configText.text = info.labelText;
		configText.layer = info.layer;
		configText.fontName = "consolas";

		pText->SetConfig( configText );
	}

	// Textbox
	{
		GuiConfig_Textbox configTextbox;

		configTextbox.anchor.horizontal = eGuiAnchorHorizontal::Right;
		configTextbox.anchor.vertical = eGuiAnchorVertical::Top;
		configTextbox.anchor.paddingHorizontal = 15;
		configTextbox.anchor.paddingVertical = (int)info.padding.y - 1;
		configTextbox.command = info.command;

		configTextbox.charsAllowed = info.charsAllowed;
		configTextbox.maxChars = info.maxChars;
		configTextbox.charSize = info.fontSize - 2;
		configTextbox.dimension.x = 220;
		configTextbox.dimension.y = (float)info.fontSize + 6.0f;

		configTextbox.pTextboxFunction = info.pFunction;
		configTextbox.layer = info.layer;

		pTextbox->SetConfig( configTextbox );
		pTextbox->SetValue( info.value );

		CHECK( info.pFunction );
	}
}
//-------------------------------------------------------------------
//	MakeNumberbox
//-------------------------------------------------------------------
void GuiPageControl::MakeNumberbox(GuiPage* pPage, IGuiComponent* pParent, NumberboxInfo& info)
{
	String componentName = info.nameSufix.Length() > 0 ? info.nameSufix : string::remove( info.labelText, " " );
	GuiComponent_Text* pText = pPage->Add<GuiComponent_Text>( "txt_" + componentName,  pParent ? pParent->GetName() : "" );
	GuiComponent_Numberbox* pNumberbox = pPage->Add<GuiComponent_Numberbox>( prefix_numberbox + componentName, pParent ? pParent->GetName() : "" );

	// Text
	{
		GuiConfig_Text configText;

		configText.anchor.horizontal = eGuiAnchorHorizontal::Left;
		configText.anchor.vertical = eGuiAnchorVertical::Top;
		configText.anchor.paddingHorizontal = (int)info.padding.x;
		configText.anchor.paddingVertical = (int)info.padding.y + 2;
		configText.charSize = (int)info.fontSize;
		configText.color4 = math::normalizeRgb( 245 );
		configText.text = info.labelText;
		configText.layer = info.layer;
		configText.fontName = "consolas";

		pText->SetConfig( configText );
	}

	// Numberbox
	{
		GuiConfig_Numberbox configNumberbox;

		configNumberbox.anchor.horizontal = eGuiAnchorHorizontal::Right;
		configNumberbox.anchor.vertical = eGuiAnchorVertical::Top;
		configNumberbox.anchor.paddingHorizontal = 100;
		configNumberbox.anchor.paddingVertical = (int)info.padding.y - 1;
		configNumberbox.valueMin = info.valueMin;
		configNumberbox.valueMax = info.valueMax;
		configNumberbox.command = info.command;

		configNumberbox.charSize = info.fontSize - 2;
		configNumberbox.dimension.x = 100;
		configNumberbox.dimension.y = (float)info.fontSize + 6.0f;

		configNumberbox.pNumberboxFunction = info.pFunction;
		configNumberbox.layer = info.layer;

		pNumberbox->SetConfig( configNumberbox );
		pNumberbox->SetValue( info.value );

		CHECK( info.pFunction );
	}
}
//-------------------------------------------------------------------
//	MakeCheckbox
//-------------------------------------------------------------------
void GuiPageControl::MakeCheckbox(GuiPage* pPage, IGuiComponent* pParent, CheckboxInfo& info)
{
	String componentName = info.nameSufix.Length() > 0 ? info.nameSufix : string::remove( info.labelText, " " );
	GuiComponent_Text* pText = pPage->Add<GuiComponent_Text>( "txt_" + componentName,  pParent ? pParent->GetName() : "" );
	GuiComponent_Checkbox* pCheckbox = pPage->Add<GuiComponent_Checkbox>( prefix_checkbox + componentName, pParent ? pParent->GetName() : "" );

	// Text
	GuiConfig_Text configText;
	{

		configText.anchor.horizontal = eGuiAnchorHorizontal::Left;
		configText.anchor.vertical = eGuiAnchorVertical::Top;
		configText.anchor.paddingHorizontal = (int)info.padding.x;
		configText.anchor.paddingVertical = (int)info.padding.y;
		configText.charSize = (int)info.fontSize;
		configText.color4 = math::normalizeRgb( 245 );
		configText.text = info.labelText;
		configText.layer = info.layer;
		configText.fontName = "consolas";

		pText->SetConfig( configText );
	}

	// Checkbox
	{
		GuiConfig_Checkbox configCheckbox;

		configCheckbox.anchor.horizontal = eGuiAnchorHorizontal::Left;
		configCheckbox.anchor.vertical = eGuiAnchorVertical::Top;
		configCheckbox.anchor.paddingHorizontal = 100;
		configCheckbox.anchor.paddingVertical = (int)info.padding.y - 1;
		configCheckbox.command = info.command;
		configCheckbox.dimension = (float)info.fontSize * 1.1f;
		configCheckbox.pCheckboxFunction = info.pFunction;
		configCheckbox.layer = info.layer;

		pCheckbox->SetConfig( configCheckbox );
		pCheckbox->SetValue( info.value );

		CHECK( info.pFunction );
	}
}
//-------------------------------------------------------------------
//	SetValues
//-------------------------------------------------------------------
void GuiPageControl::SetValues(GuiPage* pPage, const PropertyNameValueTable& valueTable)
{
	for ( auto it = valueTable.GetIterator(); it; it++ )
	{
		if ( (*it).componentType == eGuiComponent::Textbox )
			SetValue_Textbox( pPage, prefix_textbox + (*it).sufix.GetString(), (*it).valueStr );
		else if ( (*it).componentType == eGuiComponent::Numberbox )
			SetValue_Numberbox( pPage, prefix_numberbox + (*it).sufix.GetString(), (*it).valueFloat );
		else if ( (*it).componentType == eGuiComponent::Slider )
			SetValue_Slider( pPage, prefix_slider + (*it).sufix.GetString(), (*it).valueFloat );
		else if ( (*it).componentType == eGuiComponent::Checkbox )
			SetValue_Checkbox( pPage, prefix_checkbox + (*it).sufix.GetString(), (*it).valueBool );
		else
			ASSERT( "Invalid component" );
	}
}
//-------------------------------------------------------------------
//	SetValue_Slider
//-------------------------------------------------------------------
void GuiPageControl::SetValue_Slider(GuiPage* pPage, const String& componentName, float value)
{
	GuiComponent_Slider* pSlider = pPage->FindAs<GuiComponent_Slider>( componentName );

	CHECK( pSlider );

	if ( pSlider )
		pSlider->SetValue( value );
}
//-------------------------------------------------------------------
//	SetValue_Textbox
//-------------------------------------------------------------------
void GuiPageControl::SetValue_Textbox(GuiPage* pPage, const String& componentName, const String& value)
{
	GuiComponent_Textbox* pTextbox = pPage->FindAs<GuiComponent_Textbox>( componentName );

	CHECK( pTextbox );

	if ( pTextbox )
		pTextbox->SetValue( value );
}
//-------------------------------------------------------------------
//	SetValue_Numberbox
//-------------------------------------------------------------------
void GuiPageControl::SetValue_Numberbox(GuiPage* pPage, const String& componentName, float value)
{
	GuiComponent_Numberbox* pNumberbox = pPage->FindAs<GuiComponent_Numberbox>( componentName );

	CHECK( pNumberbox );

	if ( pNumberbox )
		pNumberbox->SetValue( value );
}
//-------------------------------------------------------------------
//	SetValue_Checkbox
//-------------------------------------------------------------------
void GuiPageControl::SetValue_Checkbox(GuiPage* pPage, const String& componentName, bool value)
{
	GuiComponent_Checkbox* pCheckbox = pPage->FindAs<GuiComponent_Checkbox>( componentName );

	CHECK( pCheckbox );

	if ( pCheckbox )
		pCheckbox->SetValue( value );
}
//-------------------------------------------------------------------
//	GetGui
//-------------------------------------------------------------------
EditorGui* GuiPageControl::GetGui()
{
	return g_pEditor->GetGui();
}
//-------------------------------------------------------------------
//	ShowPage
//-------------------------------------------------------------------
void GuiPageControl::ShowPage(GuiPage* pPage, bool clearStack)
{
	EditorGui* pGui = GetGui();

	for ( auto it = pGui->GetStack().GetIterator(); it; it++ )
	{
		if ( pPage->GetName() != (*it)->GetName() )
			(*it)->Show( false );
	}

	if ( clearStack )
		pGui->ClearStack();

	pGui->AddToStack( pPage );
	pPage->Show( true );
}
