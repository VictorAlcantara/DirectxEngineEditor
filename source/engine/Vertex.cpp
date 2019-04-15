#include "../engine/Vertex.h"

/******************************************************************************
	Struct VertexLayoutElement
*******************************************************************************/

//-------------------------------------------------------------------
//	operator==
//-------------------------------------------------------------------
bool VertexLayoutElement::operator==(const VertexLayoutElement& other)const
{
	return 
		semantic == other.semantic &&
		type == other.type &&
		typeSize == other.typeSize &&
		typeCount == other.typeCount &&
		semanticIndex == other.semanticIndex;
}
//-------------------------------------------------------------------
bool VertexLayoutElement::operator!=(const VertexLayoutElement& other)const
{
	return !(*this == other);
}

/******************************************************************************
	Struct VertexLayout
*******************************************************************************/

uint VertexLayout::s_idControl = 0;

//-------------------------------------------------------------------
//	MakeId
//-------------------------------------------------------------------
void VertexLayout::MakeId()
{
	id = ++s_idControl;
}
//-------------------------------------------------------------------
//	IsEqual
//-------------------------------------------------------------------
bool VertexLayout::IsEqual(const VertexLayout& v)const
{
	if ( elements.Count() != v.elements.Count() )
		return false;

	for ( uint i = 0; i < elements.Count(); i++ )
	{
		if ( elements[i] != v.elements[i] )
			return false;
	}

	return true;
}
//-------------------------------------------------------------------
//	Has
//-------------------------------------------------------------------
bool VertexLayout::Has(eVertexLayoutSemantic semantic, eVertexLayoutType type, float typeCount)const
{
	for ( uint i = 0; i < elements.Count(); i++ )
	{
		if ( elements[i].semantic == semantic &&
			elements[i].type == type &&
			elements[i].typeCount == typeCount )
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
	Vertex layout definition
*******************************************************************************/

const VertexLayout Vertex_P2T2::layout = VertexLayout( 
	VertexLayoutElement(eVertexLayoutSemantic::Position, eVertexLayoutType::Float, 2),
	VertexLayoutElement(eVertexLayoutSemantic::TexCoord, eVertexLayoutType::Float, 2)
);

const VertexLayout Vertex_P3::layout = VertexLayout( 
	VertexLayoutElement(eVertexLayoutSemantic::Position, eVertexLayoutType::Float, 3)
);

const VertexLayout Vertex_P3C4::layout = VertexLayout( 
	VertexLayoutElement(eVertexLayoutSemantic::Position, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::Color, eVertexLayoutType::Float, 4)
);

const VertexLayout Vertex_P3T2::layout = VertexLayout( 
	VertexLayoutElement(eVertexLayoutSemantic::Position, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::TexCoord, eVertexLayoutType::Float, 2)
);

const VertexLayout Vertex_P3N3::layout = VertexLayout( 
	VertexLayoutElement(eVertexLayoutSemantic::Position, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::Normal, eVertexLayoutType::Float, 3)
);

const VertexLayout Vertex_P3N3T2::layout = VertexLayout( 
	VertexLayoutElement(eVertexLayoutSemantic::Position, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::Normal, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::TexCoord, eVertexLayoutType::Float, 2)
);

const VertexLayout Vertex_P3N3Tg3Bn3T2::layout = VertexLayout( 
	VertexLayoutElement(eVertexLayoutSemantic::Position, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::Normal, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::Tangent, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::Binormal, eVertexLayoutType::Float, 3),
	VertexLayoutElement(eVertexLayoutSemantic::TexCoord, eVertexLayoutType::Float, 2)
);

/******************************************************************************
	Struct definition
*******************************************************************************/

VertexLayoutElement::VertexLayoutElement()
{
	semantic = eVertexLayoutSemantic::Unknown;
	type = eVertexLayoutType::Unknown;
	typeSize = 0;
	typeCount = 0;
	semanticIndex = 0;
}

VertexLayoutElement::VertexLayoutElement(eVertexLayoutSemantic _semantic, eVertexLayoutType _type, uint _typeCount)
{
	semantic = _semantic;
	type = _type;
	typeCount = _typeCount;
	semanticIndex = 0;

	switch ( type )
	{
		case eVertexLayoutType::Float:
			typeSize = 4;
		break;

		default:
			THROW( "Unrecognized vertex layout type when acquiring type size" );
		break;
	}
}
