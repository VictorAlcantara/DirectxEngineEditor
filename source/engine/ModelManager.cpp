#include "ModelManager.h"
#include "../engine/EngineGlobal.h"
#include "../engine/Shader.h"
#include "../engine/CoreInterface.h"
#include "../engine/Path.h"
#include "../engine/api_File.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ModelManager::ModelManager() : 
	m_tModelRepository( engineDefault::model::MODEL_MANAGER_TABLE_SIZE, &tableHashFunc_Uint ),
	m_modelHandleRepository( engineDefault::model::MODEL_HANDLE_MANAGER_SLOT_SIZE )
{
	m_idControl = 0;
	m_initialized = false;
}
//-------------------------------------------------------------------
ModelManager::~ModelManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void ModelManager::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	for ( auto it = m_tModelRepository.GetIterator(); it; it++ )
		(*it)->Shutdown();

	m_tModelRepository.Clear();
	m_modelHandleRepository.Clear();
	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void ModelManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_initialized = true;
}
//-------------------------------------------------------------------
//	LoadDirectory
//-------------------------------------------------------------------
void ModelManager::LoadDirectory(const String& directoryPath)
{
	List<String> files;

	if ( !platform::file::readAllFilesInDirectory( directoryPath.AsChar(), files ) )
	{
		ASSERT( "Error opening model directory" );
		return;
	}

	String directoryFormatted = directoryPath;

	if ( directoryFormatted.Last() != '/' && directoryFormatted.Last() != '\\' )
		directoryFormatted += '\\';

	for ( auto it = files.GetIterator(); it; it++ )
	{
		if ( string::extractFileExtension(*it, true) == path::EXTENSION_MODEL )
			LoadObj<Vertex_P3N3Tg3Bn3T2>( directoryFormatted + (*it) );
	}
}
//-------------------------------------------------------------------
//	HasModel
//-------------------------------------------------------------------
bool ModelManager::HasModelByName(const HashString& name)const
{
	return HasModel( name.GetHash() );
}
//-------------------------------------------------------------------
bool ModelManager::HasModel(const Handle<Model>& hModel)const
{
	return HasModel( hModel->GetId() );
}
//-------------------------------------------------------------------
bool ModelManager::HasModel(uint repositoryKey)const
{
	return m_tModelRepository.Find( repositoryKey );
}
//-------------------------------------------------------------------
//	GetFormatted
//-------------------------------------------------------------------
Handle<Model> ModelManager::GetFormattedByName(const HashString& name, const VertexLayout& layout)
{
	return GetFormattedById( name.GetHash(), layout );
}
//-------------------------------------------------------------------
Handle<Model> ModelManager::GetFormatted(const Handle<Model>& hModel, const VertexLayout& layout)
{
	CHECK( hModel->GetId() > 0 );
	return GetFormattedById( hModel->GetId(), layout );
}
//-------------------------------------------------------------------
Handle<Model> ModelManager::GetFormattedById(uint repositoryKey, const VertexLayout& layout)
{
	SmartPtr<ModelRepository> pModelRepo;

	pModelRepo = m_tModelRepository.Get( repositoryKey );
	
	if ( !pModelRepo )
		return Handle<Model>();

	return (*pModelRepo)->CreateForLayout( layout );
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void ModelManager::Render(const Handle<Model>& hModel, Shader* pShader)
{
	g_pRenderer->Render( &GetFormatted( hModel, pShader->GetVertexLayout() ).Get(), pShader );
}
//-------------------------------------------------------------------
//	GetModelFileName
//-------------------------------------------------------------------
String ModelManager::GetModelFileName(Handle<Model> hModel)const
{
	return GetModelFileName( hModel->GetId() );
}
//-------------------------------------------------------------------
String ModelManager::GetModelFileName(uint modelId)const
{
	SmartPtr<ModelRepository> pRepo = m_tModelRepository.Get( modelId );
	String result;

	CHECK( pRepo );

	if ( pRepo )
		result = pRepo->GetModelFileName();

	return result;
}
