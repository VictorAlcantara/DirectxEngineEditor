#pragma once

#include "Vector.h"
#include "Type.h"
#include "../engine/Vertex.h"
#include "Array.h"

/****************************************************************************************
	Template vertex function
*****************************************************************************************/

//-------------------------------------------------------------------
//	vertexGet_Pos3
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline bool vertexGet_Pos3(const VERTEX_TYPE* vertices = nullptr, uint count = 0, math::Vec3* dest = nullptr)
{
	return false;
}
//-------------------------------------------------------------------
//	vertexGet_Color
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline bool vertexGet_Color(const VERTEX_TYPE* vertices = nullptr, uint count = 0, math::Vec4* dest = nullptr)
{
	return false;
}
//-------------------------------------------------------------------
//	vertexGet_TexCoord
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline bool vertexGet_TexCoord(const VERTEX_TYPE* vertices = nullptr, uint count = 0, math::Vec2* dest = nullptr)
{
	return false;
}
//-------------------------------------------------------------------
//	vertexGet_Normal
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline bool vertexGet_Normal(const VERTEX_TYPE* vertices = nullptr, uint count = 0, math::Vec3* dest = nullptr)
{
	return false;
}
//-------------------------------------------------------------------
//	vertexGet_Tangent
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline bool vertexGet_Tangent(const VERTEX_TYPE* vertices = nullptr, uint count = 0, math::Vec3* dest = nullptr)
{
	return false;
}
//-------------------------------------------------------------------
//	vertexGet_Binormal
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline bool vertexGet_Binormal(const VERTEX_TYPE* vertices = nullptr, uint count = 0, math::Vec3* dest = nullptr)
{
	return false;
}

//=============================================================================
//	Macros for simplifying writing template specialization
//=============================================================================

#define MACRO_VERTEXGET_POS3(vertex_type) template <> inline bool vertexGet_Pos3<vertex_type>(const vertex_type* vertices, uint count, math::Vec3* dest) { \
	if ( dest ) { for ( uint i = 0; i < count; i++ ) dest[i] = vertices[i].pos; } \
	return true; }

#define MACRO_VERTEXGET_COLOR(vertex_type) template <> inline bool vertexGet_Color<vertex_type>(const vertex_type* vertices, uint count, math::Vec4* dest) { \
	if ( dest ) { for ( uint i = 0; i < count; i++ ) dest[i] = vertices[i].color; } \
	return true; }


#define MACRO_VERTEXGET_TEXCOORD(vertex_type) template <> inline bool vertexGet_TexCoord<vertex_type>(const vertex_type* vertices, uint count, math::Vec2* dest) { \
	if ( dest ) { for ( uint i = 0; i < count; i++ ) dest[i] = vertices[i].texCoord; } \
	return true; }

#define MACRO_VERTEXGET_NORMAL(vertex_type) template <> inline bool vertexGet_Normal<vertex_type>(const vertex_type* vertices, uint count, math::Vec3* dest) { \
	if ( dest ) { for ( uint i = 0; i < count; i++ ) dest[i] = vertices[i].normal; } \
	return true; }

#define MACRO_VERTEXGET_TANGENT(vertex_type) template <> inline bool vertexGet_Tangent<vertex_type>(const vertex_type* vertices, uint count, math::Vec3* dest) { \
	if ( dest ) { for ( uint i = 0; i < count; i++ ) dest[i] = vertices[i].tangent; } \
	return true; }

#define MACRO_VERTEXGET_BINORMAL(vertex_type) template <> inline bool vertexGet_Binormal<vertex_type>(const vertex_type* vertices, uint count, math::Vec3* dest) { \
	if ( dest ) { for ( uint i = 0; i < count; i++ ) dest[i] = vertices[i].binormal; } \
	return true; }


/****************************************************************************************
	Non-class declarations
*****************************************************************************************/

enum class eVertexLayoutSemantic
{
	Position,
	Normal,
	Tangent,
	Binormal,
	Color,
	TexCoord,
	Unknown,
};

enum class eVertexLayoutType
{
	Float,
	Unknown,
};

/****************************************************************************************
	structs related to vertex type inner work
*****************************************************************************************/

//=============================================================================
//	VertexLayoutElement
//=============================================================================
struct VertexLayoutElement 
{
	eVertexLayoutSemantic semantic;
	eVertexLayoutType type;
	uint typeSize;
	uint typeCount;
	uint semanticIndex;

	VertexLayoutElement();
	VertexLayoutElement(eVertexLayoutSemantic, eVertexLayoutType, uint typeCount);

	bool operator==(const VertexLayoutElement&)const;
	bool operator!=(const VertexLayoutElement&)const;
};
//=============================================================================
//	VertexLayout
//=============================================================================
struct VertexLayout
{
	public:
		template <class... Args>
		VertexLayout(Args... args);

		uint id;
		uint size;
		Array<VertexLayoutElement> elements;
	
		bool IsEqual(const VertexLayout&)const;
		bool Has(eVertexLayoutSemantic, eVertexLayoutType, float typeCount)const;

	private:
		template <class T, class ...Args>
		void AddElement(const T& element, const Args&... args)
		{
			elements += element;
			AddElement( args... );
		}

		void AddElement() {}

		void MakeId();
		static uint s_idControl;
};

//=============================================================================
//	VertexLayout definitions
//=============================================================================

template<class ...Args>
inline VertexLayout::VertexLayout(Args ...args)
{
	AddElement( args... );

	size = 0;
	for ( uint i = 0; i < elements.Count(); i++ )
	{
		uint semanticIndex = 0;
		size += elements[i].typeSize * elements[i].typeCount;

		for ( uint j = 0; j < i; j++ )
		{
			if ( elements[i].semantic == elements[j].semantic )
				semanticIndex++;
		}

		elements[i].semanticIndex = semanticIndex;
	}

	MakeId();
}

/****************************************************************************************
	Vertex types declaration
*****************************************************************************************/

//=============================================================================
//	Vertex_P2T2
//=============================================================================

struct Vertex_P2T2
{
		math::Vec2 pos;
		math::Vec2 texCoord;

		static const VertexLayout& GetLayout() { return layout; }

	private:
		static const VertexLayout layout;
};

MACRO_VERTEXGET_TEXCOORD(Vertex_P2T2);

//=============================================================================
//	Vertex_P3
//=============================================================================

struct Vertex_P3
{
	math::Vec3 pos;

	static const VertexLayout& GetLayout() { return layout; }

	private:
		static const VertexLayout layout;
};

MACRO_VERTEXGET_POS3(Vertex_P3);

//=============================================================================
//	Vertex_P3C4
//=============================================================================

struct Vertex_P3C4
{
	math::Vec3 pos;
	math::Vec4 color;

	static const VertexLayout& GetLayout() { return layout; }

	private:
		static const VertexLayout layout;
};

MACRO_VERTEXGET_POS3(Vertex_P3C4);
MACRO_VERTEXGET_COLOR(Vertex_P3C4);

//=============================================================================
//	Vertex_P3T2
//=============================================================================

struct Vertex_P3T2
{
	math::Vec3 pos;
	math::Vec2 texCoord;

	static const VertexLayout& GetLayout() { return layout; }

	private:
		static const VertexLayout layout;
};

MACRO_VERTEXGET_POS3(Vertex_P3T2);
MACRO_VERTEXGET_TEXCOORD(Vertex_P3T2);

//=============================================================================
//	Vertex_P3N3
//=============================================================================

struct Vertex_P3N3
{
	math::Vec3 pos;
	math::Vec3 normal;

	static const VertexLayout& GetLayout() { return layout; }

	private:
		static const VertexLayout layout;
};

MACRO_VERTEXGET_POS3(Vertex_P3N3);
MACRO_VERTEXGET_NORMAL(Vertex_P3N3);

//=============================================================================
//	Vertex_P3N3T2
//=============================================================================

struct Vertex_P3N3T2
{
	math::Vec3 pos;
	math::Vec3 normal;
	math::Vec2 texCoord;

	static const VertexLayout& GetLayout() { return layout; }

	private:
		static const VertexLayout layout;
};

MACRO_VERTEXGET_POS3(Vertex_P3N3T2);
MACRO_VERTEXGET_NORMAL(Vertex_P3N3T2);
MACRO_VERTEXGET_TEXCOORD(Vertex_P3N3T2);

//=============================================================================
//	Vertex_P3N3Tg3Bn3T2
//=============================================================================

struct Vertex_P3N3Tg3Bn3T2
{
	math::Vec3 pos;
	math::Vec3 normal;
	math::Vec3 tangent;
	math::Vec3 binormal;
	math::Vec2 texCoord;

	static const VertexLayout& GetLayout() { return layout; }

	private:
	static const VertexLayout layout;
};

MACRO_VERTEXGET_POS3(Vertex_P3N3Tg3Bn3T2);
MACRO_VERTEXGET_NORMAL(Vertex_P3N3Tg3Bn3T2);
MACRO_VERTEXGET_TANGENT(Vertex_P3N3Tg3Bn3T2);
MACRO_VERTEXGET_BINORMAL(Vertex_P3N3Tg3Bn3T2);
MACRO_VERTEXGET_TEXCOORD(Vertex_P3N3Tg3Bn3T2);



#undef MACRO_VERTEXGET_POS3
#undef MACRO_VERTEXGET_COLOR
#undef MACRO_VERTEXGET_TEXCOORD
#undef MACRO_VERTEXGET_NORMAL
#undef MACRO_VERTEXGET_TANGENT
#undef MACRO_VERTEXGET_BINORMAL


