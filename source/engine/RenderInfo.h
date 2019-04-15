#pragma once

#include "../engine/String.h"
#include "../engine/Model.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "../engine/Pointer.h"
#include "../engine/Exception.h"

typedef ushort renderFlag;

struct RenderInfo;
class ITexture;

#define RENDER_INFO_DECL(className) \
	static const HashString& GetTypeStatic() { static HashString type(#className); return type; } \
	const HashString& GetType()const override final { return GetTypeStatic(); }

const renderFlag renderInfoFlagAll = (renderFlag)(~0);

/****************************************************************************************
	Enum declaration
*****************************************************************************************/

enum class eMaterial
{
	None,
	Opaque,
	Transparent,
	Refraction,
};

enum class eRenderFlagTexture
{
	None = 0,
	Emissive = 1 << 1,
	Ambient = 1 << 2,
	Diffuse = 1 << 3,
	Specular = 1 << 4,
	NormalMap = 1 << 5,
};

enum class eRenderFlagModel
{
	None = 0,
	Primary = 1 << 1,
};

enum class eRenderFlagFloat
{
	None = 0,
	EmissiveIntensity = 1 << 1,
	AmbientIntensity = 1 << 2,
	DiffuseIntensity = 1 << 3,
	SpecularIntensity = 1 << 4,
	SpecularSaturate = 1 << 5,
	SpecularPower = 1 << 6,
	RefractionIntensity = 1 << 7,
	ColorSaturation = 1 << 8,
	ColorSeparation = 1 << 9,
};

enum class eRenderFlagVec3
{
	None = 0,
};

enum class eRenderFlagVec4
{
	None = 0,
	EmissiveColor = 1 << 1,
	AmbientColor = 1 << 2,
	DiffuseColor = 1 << 3,
	SpecularColor = 1 << 4,
	Color = 1 << 5,
};

enum class eRenderFlagMatrix3
{
	None = 0,
	TextureTransform =  1 << 1,
};

enum class eRenderFlagMatrix4
{
	None = 0,
	WorldTransform = 1 << 1,
};

/****************************************************************************************
	RenderGroup
*****************************************************************************************/

template <class TYPE>
class RenderGroup
{
	private:
		struct TypeByFlag
		{
			TYPE* pointer;
			renderFlag flag;

			TypeByFlag() : pointer(nullptr), flag(0) {}
		};

	friend struct RenderInfo;

	public:
		~RenderGroup();

		TYPE& Get(renderFlag);
		const TYPE& Get(renderFlag)const;

	private:
		RenderGroup();
		RenderGroup(const RenderGroup&) = delete;

		void Create(uint count);
		void Add(TYPE*, renderFlag);

		RenderGroup<TYPE>& operator=(const RenderGroup&) = delete;

		TypeByFlag* m_arrayPointerFlag;
		uint m_count;
		uint m_lastIndex;
};

//=============================================================================
//	Render group template definition
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline RenderGroup<TYPE>::RenderGroup()
{
	m_arrayPointerFlag = nullptr;
	m_count = 0;
	m_lastIndex = 0;
}
//-------------------------------------------------------------------
template <class TYPE>
inline RenderGroup<TYPE>::~RenderGroup()
{
	safeArrayRelease( m_arrayPointerFlag );
	m_count = 0;
	m_lastIndex = 0;
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template <class TYPE>
TYPE& RenderGroup<TYPE>::Get(renderFlag flag)
{
	for ( uint i = 0; i < m_count; i++ )
	{
		if ( m_arrayPointerFlag[i].flag > 0 && m_arrayPointerFlag[i].flag & flag )
			return *m_arrayPointerFlag[i].pointer;
	}

	ASSERT( "RenderGroup::Get() not found" );

	return *m_arrayPointerFlag[0].pointer;
}
//-------------------------------------------------------------------
template <class TYPE>
const TYPE& RenderGroup<TYPE>::Get(renderFlag flag)const
{
	for ( uint i = 0; i < m_count; i++ )
	{
		if ( m_arrayPointerFlag[i].flag > 0 && m_arrayPointerFlag[i].flag & flag )
			return *m_arrayPointerFlag[i].pointer;
	}

	ASSERT( "RenderGroup::Get() not found" );

	return *m_arrayPointerFlag[0].pointer;
}
//-------------------------------------------------------------------
//	Create
//-------------------------------------------------------------------
template <class TYPE>
inline void RenderGroup<TYPE>::Create(uint count)
{
	CHECK( count > 0 && !m_arrayPointerFlag );

	count = math::min<uint>( count, 16 );

	if ( !m_arrayPointerFlag )
	{
		m_arrayPointerFlag = new TypeByFlag[count];
		m_count = count;
	}
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
template <class TYPE>
inline void RenderGroup<TYPE>::Add(TYPE* pData, renderFlag flag)
{
	CHECK( pData );
	CHECK( m_count > 0 );
	CHECK( m_lastIndex < m_count );

	if ( m_lastIndex < m_count )
	{
		for ( uint i = 0; i < m_lastIndex; i++ )
		{
			if ( flag != 0 && flag & m_arrayPointerFlag[i].flag )
			{
				ASSERT( "Flag already included" );
			}
		}

		m_arrayPointerFlag[m_lastIndex].pointer = pData;
		m_arrayPointerFlag[m_lastIndex].flag = flag;

		m_lastIndex++;
	}
}

/****************************************************************************************
	RenderInfo declaration
*****************************************************************************************/

struct RenderInfo
{
	virtual const HashString& GetType()const = 0;

	eMaterial material;
	eCullMode cullMode;
	eFillMode fillMode;
	bool visible;
	bool castShadow;
	RenderGroup<Handle<ITexture>> textureGroup;
	RenderGroup<TextureSampler> textureSamplerGroup;
	RenderGroup<Handle<Model>> modelGroup;
	RenderGroup<float> floatGroup;
	RenderGroup<math::Vec2> vec2Group;
	RenderGroup<math::Vec3> vec3Group;
	RenderGroup<math::Vec4> vec4Group;
	RenderGroup<math::Mtx33> matrix3Group;
	RenderGroup<math::Mtx44> matrix4Group;

	const math::Aabb3& GetAabb()const { return aabbLocal; }
	const math::Aabb3& GetAabbGlobal()const { return aabbGlobal; }

	void SetTransform(const math::Transform&);
	void SetTranslation(const math::Vec3&);
	void SetScale(const math::Vec3&);
	void SetRotation(const math::Euler&);
	void SetRotation(const math::Mtx44&);

	const math::Transform& GetTransform()const { return transform; }

	//virtual void OnRender() {}

	protected:
		RenderInfo() : material(eMaterial::None), cullMode(eCullMode::None), fillMode(eFillMode::Solid), visible(true), castShadow(true) {}

		void UseTexture(uint count);
		void AddTexture(Handle<ITexture>*, TextureSampler*, renderFlag renderFlagTexture);
		void UseModel(uint count);
		void AddModel(Handle<Model>*, renderFlag renderFlagModel);
		void UseFloat(uint count);
		void AddFloat(float*, renderFlag renderFlagFloat);
		void UseVec2(uint count);
		void AddVec2(math::Vec2*, renderFlag renderFlagVec2);
		void UseVec3(uint count);
		void AddVec3(math::Vec3*, renderFlag renderFlagVec3);
		void UseVec4(uint count);
		void AddVec4(math::Vec4*, renderFlag renderFlagVec4);
		void UseMatrix3(uint count);
		void AddMatrix3(math::Mtx33*, renderFlag renderFlagMatrix3);
		void UseMatrix4(uint count);
		void AddMatrix4(math::Mtx44*, renderFlag renderFlagMatrix4);

		virtual void OnUpdateTransform() {}

		math::Transform transform;
		math::Aabb3 aabbLocal;
		math::Aabb3 aabbGlobal;
};

/****************************************************************************************
	RenderInfo_EntitySceneBase declaration
*****************************************************************************************/

struct RenderInfo_EntitySceneBase : public RenderInfo
{
	math::Vec4 color4;
	float emissiveIntensity;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float specularPower;
	float refractionIntensity;
	float colorSaturation;
	float colorSeparation;

	void SetModel(Handle<Model>& hModel);
	const Handle<Model>& GetModel()const { return hModel; }
	
	void SetTexTranslate(const math::Vec2&);
	void SetTexScale(const math::Vec2&);
	void SetTexRotationDegs(float);

	const math::Vec2& GetTexTranslate()const { return texTranslate; }
	const math::Vec2& GetTexScale()const { return texScale; }
	float GetTexRotationDegs()const { return texRotateDegs; }

	void UpdateAabb();

	protected:
		RenderInfo_EntitySceneBase() = delete;
		RenderInfo_EntitySceneBase(const RenderInfo_EntitySceneBase&) = delete;
		RenderInfo_EntitySceneBase(uint floatGroupSize, uint vec4GroupSize, uint modelGroupSize, uint matrix3GroupSize);

		void OnUpdateTransform()override final;

		RenderInfo_EntitySceneBase& operator=(const RenderInfo_EntitySceneBase&) = delete;

	private:
		Handle<Model> hModel;
		math::Vec2 texTranslate;
		math::Vec2 texScale;
		float texRotateDegs;
		math::Mtx33 texTransform;
};
