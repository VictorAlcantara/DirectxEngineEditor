#include "ObjLoader.h"
#include "../engine/List.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ObjLoader::ObjLoader()
{
	Unload();
}
//-------------------------------------------------------------------
ObjLoader::~ObjLoader()
{
	Unload();
}
//-------------------------------------------------------------------
//	Unload
//-------------------------------------------------------------------
void ObjLoader::Unload()
{
	m_loaded = false;
	m_data.Clear();
}
//-------------------------------------------------------------------
//	Load
//-------------------------------------------------------------------
bool ObjLoader::Load(const String& pathFile)
{
	CHECK( !IsLoaded() );

	Unload();

	FileReader file;

	if ( !file.Load( pathFile, eFileMode::Text ) )
		return false;

	FixedArray<math::Vec3> points;
	FixedArray<math::Vec2> texCoords;
	FixedArray<math::Vec3> normals;
	FixedArray<Vertex_P3N3Tg3Bn3T2> vertices;

	FileCount( file, points, texCoords, normals, vertices );

	for ( uint i = 0; i < file.LineCount(); i++ )
	{
		const char* line = file.GetLine(i).GetData();
		uint lineSize = file.GetLine(i).GetSize();

		if ( IsVertex(line, lineSize) )
		{
			math::Vec3 vertex = ReadLine_Vec3( line, lineSize );
			vertex.z = -vertex.z;
			points += vertex;
		}
		else if ( IsTexCoord(line, lineSize) )
		{
			math::Vec2 texCoord = ReadLine_Vec2( line, lineSize );
			//texCoord.x = 1.0f - texCoord.x;
			texCoord.y = 1.0f - texCoord.y;
			texCoords += texCoord;
		}
		else if ( IsNormal(line, lineSize) )
			normals += math::normalize( ReadLine_Vec3(line, lineSize) );
		else if ( IsFace(line, lineSize) )
		{
			// v vt vn for each triangle
			int face[9];
			Vertex_P3N3Tg3Bn3T2 vertex[3];

			ReadLine_Face( line, lineSize, face, ARRAY_COUNT(face) );
			const uint handnessIdxs[] = { 0, 2, 1 };

			for ( uint i = 0; i < 3; i++ )
			{
				uint handIdx = handnessIdxs[i];
				int texCoordIdx = face[handIdx * 3 + 1];

				vertex[i].pos = points[face[handIdx * 3]];
				vertex[i].normal = normals[face[handIdx * 3 + 2]];

				if ( texCoordIdx >= 0 )
					vertex[i].texCoord = texCoords[face[handIdx * 3 + 1]];
			}

			math::Vec3 tangent;
			math::Vec3 binormal;

			math::calculateTangentBinormal( vertex[0].pos, vertex[1].pos, vertex[2].pos, vertex[0].texCoord, vertex[1].texCoord, vertex[2].texCoord, tangent, binormal );

			for ( uint i = 0; i < 3; i++ )
			{
				vertex[i].tangent = tangent;
				vertex[i].binormal = binormal;

				vertices += vertex[i];
			}
		}
	}

	m_data.Clear();
	m_data.vertexCount = vertices.Count();
	m_data.vertices = new Vertex_P3N3Tg3Bn3T2[m_data.vertexCount];
	
	memoryCopy( vertices.GetPointer(), m_data.vertices, m_data.vertexCount );

	m_loaded = true;

	return true;
}
//-------------------------------------------------------------------
//	FileCount
//-------------------------------------------------------------------
void ObjLoader::FileCount(const FileReader& file, 
	FixedArray<math::Vec3>& outPoints, FixedArray<math::Vec2>& outTexCoords, FixedArray<math::Vec3>& outNormals, FixedArray<Vertex_P3N3Tg3Bn3T2>& outVertices)
{
	uint pointCount = 0;
	uint texCoordCount = 0;
	uint normalCount = 0;
	uint faceCount = 0;

	for ( uint i = 0; i < file.LineCount(); i++ )
	{
		const char* line = file.GetLine(i).GetData();
		uint lineSize = file.GetLine(i).GetSize();

		if ( lineSize < 2 )
			continue;

		if ( IsVertex(line, lineSize) )
			pointCount++;
		else if ( IsTexCoord(line, lineSize) )
			texCoordCount++;
		else if ( IsNormal(line, lineSize) )
			normalCount++;
		else if ( IsFace(line, lineSize) )
			faceCount++;
	}

	outPoints.Clear( pointCount );
	outTexCoords.Clear( texCoordCount );
	outNormals.Clear( normalCount );
	outVertices.Clear( faceCount * 3 );
}
//-------------------------------------------------------------------
//	Is
//-------------------------------------------------------------------
bool ObjLoader::Is(const char* line, uint lineSize, const String& cmp)const
{
	if ( cmp.Length() >= lineSize )
		return false;

	uint i = 0;
	for ( ; i < cmp.Length(); i++ )
	{
		if ( line[i] != cmp[i] )
			return false;
	}

	if ( line[i] != ' ' )
		return false;

	return true;
}
//-------------------------------------------------------------------
bool ObjLoader::IsVertex(const char* line, uint lineSize)const
{
	return Is( line, lineSize, "v" );
}
//-------------------------------------------------------------------
bool ObjLoader::IsVertex(const String& s)const
{
	return IsVertex( s.AsChar(), s.Length() );
}
//-------------------------------------------------------------------
bool ObjLoader::IsTexCoord(const char* line, uint lineSize)const
{
	return Is( line, lineSize, "vt" );
}
//-------------------------------------------------------------------
bool ObjLoader::IsTexCoord(const String& s)const
{
	return IsTexCoord( s.AsChar(), s.Length() );
}
//-------------------------------------------------------------------
bool ObjLoader::IsNormal(const char* line, uint lineSize)const
{
	return Is( line, lineSize, "vn" );
}
//-------------------------------------------------------------------
bool ObjLoader::IsNormal(const String& s)const
{
	return IsNormal( s.AsChar(), s.Length() );
}
//-------------------------------------------------------------------
bool ObjLoader::IsFace(const char* line, uint lineSize)const
{
	return Is( line, lineSize, "f" );
}
//-------------------------------------------------------------------
bool ObjLoader::IsFace(const String& s)const
{
	return IsFace( s.AsChar(), s.Length() );
}
//-------------------------------------------------------------------
//	ReadLine
//-------------------------------------------------------------------
uint ObjLoader::ReadLine(const char* line, uint size, String* outResult, uint resultMax)const
{
	int current = -1;
	bool isNumber = false;

	for ( uint i = 0; i < size; i++ )
	{
		char c = line[i];
		bool wasNumber = isNumber;
		isNumber = string::isNumber(c);

		if ( isNumber )
		{
			if ( !wasNumber )
				current++;

			if ( current >= (int)resultMax )
				break;

			outResult[current] += c;
		}
	}

	return current + 1;
}
//-------------------------------------------------------------------
math::Vec2 ObjLoader::ReadLine_Vec2(const char* line, uint size)const
{
	String sResult[2];
	uint resultCount = ReadLine( line, size, sResult, ARRAY_COUNT(sResult) );

	CHECK( resultCount == ARRAY_COUNT(sResult) );

	math::Vec2 result;
	result.x = string::strToFloat(sResult[0]);
	result.y = string::strToFloat(sResult[1]);

	return result;
}
//-------------------------------------------------------------------
math::Vec3 ObjLoader::ReadLine_Vec3(const char* line, uint size)const
{
	String sResult[3];
	uint resultCount = ReadLine( line, size, sResult, ARRAY_COUNT(sResult) );

	CHECK( resultCount == ARRAY_COUNT(sResult) );

	math::Vec3 result;
	result.x = string::strToFloat(sResult[0]);
	result.y = string::strToFloat(sResult[1]);
	result.z = string::strToFloat(sResult[2]);

	return result;
}
//-------------------------------------------------------------------
void ObjLoader::ReadLine_Face(const char* line, uint size, int* outFaces, uint faceMax)const
{
	if ( faceMax != 9 )
	{
		ASSERT( "Invalid face count" );
		return;
	}

	String sResult[12];
	uint resultCount = ReadLine( line, size, sResult, ARRAY_COUNT(sResult) );

	CHECK( resultCount == 6 || resultCount == 9 || resultCount == 12 );

	for ( uint i = 0, resultCurrent = 0; i < 9; i++ )
	{
		if ( resultCount == 6 && (i == 1 || i == 4 || i == 7) )
			outFaces[i] = -1;
		else
			outFaces[i] = string::strToInt( sResult[resultCurrent++] ) - 1;
	}
}
//-------------------------------------------------------------------
//	GetData
//-------------------------------------------------------------------
const ObjData& ObjLoader::GetData()const
{
	CHECK( m_loaded );
	return m_data;
}
