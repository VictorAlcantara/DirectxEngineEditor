#pragma once

#include "ResourceModule.h"
#include "Class.h"
#include "../engine/Model.h"
#include "String.h"
#include "../engine/Vertex.h"
#include "ModelRepository.h"
#include "Table.h"
#include "Pointer.h"
#include "ObjLoader.h"

class Shader;

class ModelManager : public NonCopyable
{
	RESOURCE_MODULE;

	public:
		virtual ~ModelManager();

		void Initialize();
		void Shutdown();

		bool HasModelByName(const HashString& name)const;
		bool HasModel(const Handle<Model>&)const;
		bool HasModel(uint id)const;

		Handle<Model> GetFormattedByName(const HashString& name, const VertexLayout&);
		Handle<Model> GetFormatted(const Handle<Model>&, const VertexLayout&);
		Handle<Model> GetFormattedById(uint modelId, const VertexLayout&);

		String GetModelFileName(uint modelId)const;
		String GetModelFileName(Handle<Model>)const;

		void Render(const Handle<Model>&, Shader*);

		void LoadDirectory(const String& directoryPath);
		template <class VERTEX_TYPE>
		Handle<Model> LoadObj(const String& path);
		// TODO: maybe have a struct for vertex creation info
		template <class VERTEX_TYPE>
		Handle<Model> Create(const HashString& name, eTopology, const VERTEX_TYPE* vertices, uint vertexCount, uint* indices, uint indexCount);

		template <class VERTEX_TYPE>
		Handle<Model> Create(eTopology, const VERTEX_TYPE* vertices, uint vertexCount, uint* indices, uint indexCount);
		
		template <class VERTEX_TYPE>
		void GetAllWithLayout(List<Handle<Model>>& outResult);

	private:
		ModelManager();

		template <class VERTEX_TYPE>
		Handle<Model> CreateWithRepositoryKey(uint repositoryKey, eTopology, const VERTEX_TYPE* vertices, uint vertexCount, uint* indices, uint indexCount);

		bool m_initialized;
		uint m_idControl;
		Table<SmartPtr<ModelRepository>, uint> m_tModelRepository;
		HandleManager<Model> m_modelHandleRepository;
};

//-------------------------------------------------------------------
//	LoadObj
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
Handle<Model> ModelManager::LoadObj(const String& filePath)
{
	HashString name = string::extractFileName( filePath, false );
	Handle<Model> hResult = GetFormattedByName( name, VERTEX_TYPE::GetLayout() );

	if ( !hResult )
	{
		ObjLoader loader;

		if ( loader.Load(filePath) )
		{
			hResult = Create( name, eTopology::Triangle, loader.GetData().vertices, loader.GetData().vertexCount, loader.GetData().indices, loader.GetData().indexCount );

			SmartPtr<ModelRepository> pRepo = m_tModelRepository.Get( hResult->GetId() );
			CHECK( pRepo );
			pRepo->SetModelFileName( name.GetString() );
		}
		else
			ASSERT( "Failed loading model" );

		hResult = GetFormattedByName( name, VERTEX_TYPE::GetLayout() );
	}

	CHECK( hResult );
	return hResult;
}
//-------------------------------------------------------------------
//	Create
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline Handle<Model> ModelManager::Create(eTopology topology, const VERTEX_TYPE* vertices, uint vertexCount, uint* indices, uint indexCount)
{
	uint id = ++m_idControl;

	if ( m_tModelRepository.Find(id) )
	{
		THROW( "ModelManager::Create() Id already exists" );
		return Handle<Model>();
	}

	return CreateWithRepositoryKey( id, topology, vertices, vertexCount, indices, indexCount );
}
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline Handle<Model> ModelManager::Create(const HashString& name, eTopology topology, const VERTEX_TYPE* vertices, uint vertexCount, uint* indices, uint indexCount)
{
	if ( name.GetHash() == 0 )
	{
		THROW( "ModelManager::Create() Hash is zero" );
		return Handle<Model>();
	}

	return CreateWithRepositoryKey( name.GetHash(), topology, vertices, vertexCount, indices, indexCount );
}
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline Handle<Model> ModelManager::CreateWithRepositoryKey(uint repositoryKey, eTopology topology, const VERTEX_TYPE* vertices, uint vertexCount, uint* indices, uint indexCount)
{
	if ( m_tModelRepository.Find(repositoryKey) )
	{
		ASSERT( "Model already has a repository, cannot create" );
		return Handle<Model>();
	}

	if ( repositoryKey == 0 )
	{
		ASSERT( "Invalid key" );
		return Handle<Model>();
	}

	SmartPtr<ModelRepository> pModelRepo = new ModelRepository( repositoryKey, &m_modelHandleRepository );
	pModelRepo->Initialize();

	m_tModelRepository.Add( pModelRepo, repositoryKey );

	if ( vertexGet_Pos3<VERTEX_TYPE>() )
	{
		math::Vec3* repoVertPos = new math::Vec3[vertexCount];
		vertexGet_Pos3( vertices, vertexCount, repoVertPos );
		pModelRepo->SetModelVertex_Pos( repoVertPos, vertexCount );
		safeArrayRelease( repoVertPos );
	}
	if ( vertexGet_TexCoord<VERTEX_TYPE>() )
	{
		math::Vec2* repoVertTexCoord = new math::Vec2[vertexCount];
		vertexGet_TexCoord( vertices, vertexCount, repoVertTexCoord );
		pModelRepo->SetModelVertex_TexCoord( repoVertTexCoord, vertexCount );
		safeArrayRelease( repoVertTexCoord );
	}
	if ( vertexGet_Normal<VERTEX_TYPE>() )
	{
		math::Vec3* repoVertNormal = new math::Vec3[vertexCount];
		vertexGet_Normal( vertices, vertexCount, repoVertNormal );
		pModelRepo->SetModelVertex_Normal( repoVertNormal, vertexCount );
		safeArrayRelease( repoVertNormal );
	}
	if ( vertexGet_Tangent<VERTEX_TYPE>() )
	{
		math::Vec3* repoVertTangent = new math::Vec3[vertexCount];
		vertexGet_Tangent( vertices, vertexCount, repoVertTangent );
		pModelRepo->SetModelVertex_Tangent( repoVertTangent, vertexCount );
		safeArrayRelease( repoVertTangent );
	}
	if ( vertexGet_Binormal<VERTEX_TYPE>() )
	{
		math::Vec3* repoVertBinormal = new math::Vec3[vertexCount];
		vertexGet_Binormal( vertices, vertexCount, repoVertBinormal );
		pModelRepo->SetModelVertex_Binormal( repoVertBinormal, vertexCount );
		safeArrayRelease( repoVertBinormal );
	}
	if ( vertexGet_Color<VERTEX_TYPE>() )
	{
		math::Vec4* repoVertColor = new math::Vec4[vertexCount];
		vertexGet_Color( vertices, vertexCount, repoVertColor );
		pModelRepo->SetModelVertex_Color( repoVertColor, vertexCount );
		safeArrayRelease( repoVertColor );
	}

	pModelRepo->SetModelTopology( topology );
	pModelRepo->SetModelIndex( indices, indexCount );

	return pModelRepo->CreateForLayout<VERTEX_TYPE>();
}
//-------------------------------------------------------------------
//	GetAllWithLayout
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline void ModelManager::GetAllWithLayout(List<Handle<Model>>& outResult)
{
	for ( auto it = m_tModelRepository.GetIterator(); it; it++ )
	{
		if ( (*it)->IsCompatible(VERTEX_TYPE::GetLayout()) )
			outResult.AddLast( (*it)->CreateForLayout(VERTEX_TYPE::GetLayout()) );
	}
}
