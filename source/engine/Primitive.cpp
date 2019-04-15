#include "../engine/Primitive.h"
#include "../engine/CoreInterface.h"
#include "../engine/Vertex.h"

/****************************************************************************************
	geometry namespace
*****************************************************************************************/

//-------------------------------------------------------------------
//	createLine
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createLine()
{
	HashString name( "line"  );

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3C4::GetLayout() );

	uint vertexCount = 2;
	Vertex_P3C4* vertices = new Vertex_P3C4[vertexCount];

	vertices[0].pos = math::Vec3(0.0f);
	vertices[0].color = math::Vec4(1.0f);
	vertices[1].pos = math::Vec3(0.0f, 0.0f, 1.0f);
	vertices[1].color = math::Vec4(1.0f);

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::LineStrip, vertices, vertexCount, nullptr, 0 );

	safeArrayRelease( vertices );

	return result;
}
//-------------------------------------------------------------------
//	createCubeLine
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createCubeLine()
{
	HashString name("squareLine");

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3C4::GetLayout() );

	float dist = 1.0f;
	uint vertexCount = 8;
	uint indexCount = 24;
	Vertex_P3C4* vertices = new Vertex_P3C4[vertexCount];
	uint* indices = new uint[indexCount];

	for ( uint i = 0; i < vertexCount; i++ )
		vertices[i].color = math::Vec4(1.0f);

	// Bottom
	vertices[0].pos = math::Vec3(  dist, -dist, -dist );
	vertices[1].pos = math::Vec3(  dist, -dist,  dist );
	vertices[2].pos = math::Vec3( -dist, -dist,  dist );
	vertices[3].pos = math::Vec3( -dist, -dist, -dist );

	// Top
	vertices[4].pos = math::Vec3(  dist,  dist, -dist );
	vertices[5].pos = math::Vec3(  dist,  dist,  dist );
	vertices[6].pos = math::Vec3( -dist,  dist,  dist );
	vertices[7].pos = math::Vec3( -dist,  dist, -dist );

	// Bottom
	indices[ 0] = 0;
	indices[ 1] = 1;
	indices[ 2] = 1;
	indices[ 3] = 2;
	indices[ 4] = 2;
	indices[ 5] = 3;
	indices[ 6] = 3;
	indices[ 7] = 0;
	// Top
	indices[ 8] = 4;
	indices[ 9] = 5;
	indices[10] = 5;
	indices[11] = 6;
	indices[12] = 6;
	indices[13] = 7;
	indices[14] = 7;
	indices[15] = 4;
	// Sides
	indices[16] = 0;
	indices[17] = 4;
	indices[18] = 1;
	indices[19] = 5;
	indices[20] = 2;
	indices[21] = 6;
	indices[22] = 3;
	indices[23] = 7;

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::Line, vertices, vertexCount, indices, indexCount );

	safeArrayRelease( indices );
	safeArrayRelease( vertices );

	return result;
}
//-------------------------------------------------------------------
//	createCircleLine
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createCircleLine(uint lines)
{
	CHECK( math::between<uint>(lines, 4, 1000) );

	lines = (uint)math::clamp( (float)lines, 4.0f, 1000.0f );

	HashString name( "circleLine_" + string::intToStr(lines) );

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3C4::GetLayout() );

	uint vertexCount = lines + 1;
	Vertex_P3C4* vertices = new Vertex_P3C4[vertexCount];

	for ( uint i = 0; i < lines; i++ )
	{
		float degs = (360.0f / (float)lines) * (float)i;
		
		vertices[i].pos.x = math::cosDegs(degs);
		vertices[i].pos.y = math::sinDegs(degs);
		vertices[i].pos.z = 0.0f;
		vertices[i].color = math::Vec4(1.0f);
	}

	vertices[vertexCount - 1] = vertices[0];

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::LineStrip, vertices, vertexCount, nullptr, 0 );

	safeArrayRelease( vertices );

	return result;
}
//-------------------------------------------------------------------
//	createSphereLine
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createSphereLine(uint stacks, uint halfSlices)
{
	CHECK( stacks >= 1 && halfSlices >= 2 );

	HashString name("sphereLine_" + string::intToStr(stacks) + "_" + string::intToStr(halfSlices));

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3C4::GetLayout() );

	uint slices = halfSlices * 2;
	uint stackSteps = stacks * 2 + 2;

	uint vertexCount = halfSlices * stackSteps + stacks * slices + halfSlices + stacks;
	Vertex_P3C4* vertices = new Vertex_P3C4[vertexCount];
	uint indexCount = 0;
	uint* indices = nullptr;

	math::Vec3 vertex( 0.0f, 1.0f, 0.0f );
	uint idxControl = 0;

	// Vertical lines
	for ( uint idxSlice = 0; idxSlice < halfSlices; idxSlice++ )
	{
		float sliceRotation = (float)idxSlice * (180.0f / (float)halfSlices);
		math::Mtx44 mtxSliceRotate = math::matrixYawDegs( sliceRotation );

		for ( uint idxStack = 0; idxStack < stackSteps; idxStack++ )
		{
			float stackRotation = (float)idxStack * (360.0f / (float)stackSteps);
			math::Mtx44 mtxStackRotate = math::matrixRollDegs( -stackRotation );
			vertices[idxControl++].pos = (vertex.Xyzw(1.0f) * (mtxStackRotate * mtxSliceRotate)).Xyz(); // Gimbal lock :/
		}

		vertices[idxControl++].pos = vertex;
	}

	// Horizontal lines
	for ( uint idxStack = 0; idxStack < stacks; idxStack++ )
	{
		float stackRotation = (float)(idxStack + 1) * (360.0f / (float)stackSteps);
		math::Mtx44 mtxStackRotate = math::matrixRollDegs( -stackRotation );

		for ( uint idxSlice = 0; idxSlice < slices; idxSlice++ )
		{
			float sliceRotation = (float)(idxSlice) * (360.0f / (float)slices);
			math::Mtx44 mtxSliceRotate = math::matrixYawDegs( sliceRotation );
			vertices[idxControl++].pos = (vertex.Xyzw(1.0f) * (mtxStackRotate * mtxSliceRotate)).Xyz(); // Gimbal lock :/
		}

		vertices[idxControl++].pos = (vertex.Xyzw(1.0f) * mtxStackRotate).Xyz();
	}

	for ( uint i = 0; i < vertexCount; i++ )
		vertices[i].color = math::Vec4(1.0f);

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::LineStrip, vertices, vertexCount, indices, indexCount );

	safeArrayRelease( indices );
	safeArrayRelease( vertices );

	return result;
}
//-------------------------------------------------------------------
//	createCylinderLine
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createCylinderLine(uint halfSlices)
{
	CHECK( halfSlices > 1 );

	HashString name("cylinderLine_" + string::intToStr(halfSlices));

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3C4::GetLayout() );

	uint vertexCount = 5 * halfSlices + (halfSlices * 2 + 1) * 2 + 1;
	Vertex_P3C4* vertices = new Vertex_P3C4[vertexCount];
	uint* indices = nullptr;
	uint indexCount = 0;

	for ( uint i = 0; i < halfSlices; i++ )
	{
		float rotation = (float)i * 180.0f / (float)halfSlices;
		math::Mtx44 yaw = math::matrixYawDegs( rotation );

		vertices[i * 5 + 0].pos = math::Vec3(  1.0f, -1.0f, 0.0f ) * yaw;
		vertices[i * 5 + 1].pos = math::Vec3(  1.0f,  1.0f, 0.0f ) * yaw;
		vertices[i * 5 + 2].pos = math::Vec3( -1.0f,  1.0f, 0.0f ) * yaw;
		vertices[i * 5 + 3].pos = math::Vec3( -1.0f, -1.0f, 0.0f ) * yaw;
		vertices[i * 5 + 4].pos = vertices[i * 5 + 0].pos;
	}
	
	vertices[halfSlices * 5].pos = math::Vec3( 0.0f, -1.0f, 0.0f );

	for ( uint h = 0; h <= 1; h++ )
	{
		uint indexBase = 5 * halfSlices + 1 + (halfSlices * 2 + 1) * h;
		float y = h == 0 ? -1.0f : 1.0f;

		for ( uint i = 0; i < halfSlices * 2 + 1; i++ )
		{
			math::Mtx44 yaw = math::matrixYawDegs( (float)(i % (halfSlices * 2)) * 360.0f / (halfSlices * 2.0f) );
			vertices[indexBase + i].pos = math::Vec3( 1.0f, y, 0.0f ) * yaw;
		}
	}

	for ( uint i = 0; i < vertexCount; i++ )
		vertices[i].color = math::Vec4(1.0f);

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::LineStrip, vertices, vertexCount, indices, indexCount );

	safeArrayRelease( indices );
	safeArrayRelease( vertices );

	return result;
}
//-------------------------------------------------------------------
//	createPyramidLine
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createPyramidLine(uint baseVertexCount)
{
	CHECK( baseVertexCount > 2 );

	if ( baseVertexCount < 3 )
		baseVertexCount = 3;

	HashString name("pyramidLine_" + string::intToStr(baseVertexCount));

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3C4::GetLayout() );

	uint vertexCount = 1 + baseVertexCount + 1; // 1 for base point and 1 for end point
	Vertex_P3C4* vertices = new Vertex_P3C4[vertexCount];
	uint indexCount = baseVertexCount * 2 * 2 + baseVertexCount * 2;
	uint* indices = new uint[indexCount];

	// Vertices
	for ( uint i = 0; i < vertexCount; i++ )
		vertices[i].color = math::Vec4( 1.0f );

	vertices[0].pos = math::Vec3( 0.0f, 0.0f, 1.0f );

	for ( uint i = 0; i < baseVertexCount; i++ )
	{
		math::Vec3 pointToRotate( 1.0f, 0.0f, 1.0f );
		math::Mtx44 rotation = math::matrixRollDegs( ((float)i / (float)baseVertexCount) * 360.0f );
		vertices[i + 1].pos = pointToRotate * rotation;
	}

	vertices[vertexCount - 1].pos = math::Vec3(0.0f);

	// Indices
	{
		uint idx = 0;
		// Base
		for ( uint i = 0; i < baseVertexCount; i++ )
		{
			/*
				Example for baseVertexCount = 3
				0) 0 1 1 2
				1) 0 2 2 3
				2) 0 3 3 4
				3) 0 4 4 1
			*/

			indices[idx++] = 0;
			indices[idx++] = i + 1;
			indices[idx++] = i + 1;
			indices[idx++] = 1 + ((i + 1) % baseVertexCount);
		}

		// Lines to pointy end
		for ( uint i = 0; i < baseVertexCount; i++ )
		{
			uint pointyEndIdx = vertexCount - 1;

			indices[idx++] = pointyEndIdx;
			indices[idx++] = 1 + i;
		}

		CHECK( idx <= indexCount );
	}

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::Line, vertices, vertexCount, indices, indexCount );

	safeArrayRelease( indices );
	safeArrayRelease( vertices );

	return result;
}
//-------------------------------------------------------------------
//	createCircle
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createCircle(uint slices)
{
	CHECK( math::between<uint>(slices, 3, 50) );

	slices = (uint)math::clamp( (float)slices, 3.0f, 50.0f );

	HashString name("circle_" + string::intToStr(slices));

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3N3T2::GetLayout() );

	uint vertexCount = slices + 1;
	uint indexCount = slices * 3;
	Vertex_P3N3T2* vertices = new Vertex_P3N3T2[vertexCount];
	uint* indices = new uint[indexCount];
	float dist = 1.0f;

	// Make the vertices
	vertices[0].pos = math::Vec3( 0.0f, 0.0f, 0.0f );
	vertices[0].normal = math::Vec3( 0.0f, 0.0f, -1.0f );
	vertices[0].texCoord = math::ndcToScreenNormalized( vertices[0].pos.Xy() );

	for ( uint i = 0; i < slices; i++ )
	{
		float rotationDegs = -360.0f * ((float)i / (float)slices);

		float x = math::cosDegs( rotationDegs );
		float y = math::sinDegs( rotationDegs );

		vertices[i + 1].pos = math::Vec3( x, y, 0.0f );
		vertices[i + 1].normal = math::Vec3( 0.0f, 0.0f, -1.0f );
		vertices[i + 1].texCoord = math::ndcToScreenNormalized( x, y );
	}

	// Make the indices
	for ( uint i = 0; i < slices; i++ )
	{
		indices[i * 3 + 0] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = (i + 1) % slices + 1;
	}

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::Triangle, vertices, vertexCount, indices, indexCount );

	safeArrayRelease( indices );
	safeArrayRelease( vertices );

	return result;
}
//-------------------------------------------------------------------
//	createSquare
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createSquare()
{
	HashString name("square");
	float dist = 1.0f;
	Vertex_P3N3Tg3Bn3T2 vertices[4];

	if ( g_pResource->GetModelMngr()->HasModelByName(name) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3N3Tg3Bn3T2::GetLayout() );

	vertices[0].pos = math::Vec3( -dist, -dist, 0.0f );
	vertices[1].pos = math::Vec3( -dist,  dist, 0.0f );
	vertices[2].pos = math::Vec3(  dist, -dist, 0.0f );
	vertices[3].pos = math::Vec3(  dist,  dist, 0.0f );

	vertices[0].texCoord = math::Vec2( 0.0f, 1.0f );
	vertices[1].texCoord = math::Vec2( 0.0f, 0.0f );
	vertices[2].texCoord = math::Vec2( 1.0f, 1.0f );
	vertices[3].texCoord = math::Vec2( 1.0f, 0.0f );

	vertices[0].normal = math::Vec3( 0.0f, 0.0f, -1.0f );
	vertices[1].normal = math::Vec3( 0.0f, 0.0f, -1.0f );
	vertices[2].normal = math::Vec3( 0.0f, 0.0f, -1.0f );
	vertices[3].normal = math::Vec3( 0.0f, 0.0f, -1.0f );

	math::Vec3 tangent;
	math::Vec3 binormal;
	math::calculateTangentBinormal( vertices[0].pos, vertices[1].pos, vertices[2].pos, vertices[0].texCoord, vertices[1].texCoord, vertices[2].texCoord, tangent, binormal );

	vertices[0].tangent = tangent;
	vertices[1].tangent = tangent;
	vertices[2].tangent = tangent;
	vertices[3].tangent = tangent;

	vertices[0].binormal = binormal;
	vertices[1].binormal = binormal;
	vertices[2].binormal = binormal;
	vertices[3].binormal = binormal;

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::TriangleStrip, vertices, ARRAY_COUNT(vertices), nullptr, 0 );
	return result;
}
//-------------------------------------------------------------------
//	createBlock
//-------------------------------------------------------------------
Handle<Model> primitive::geometry::createBlock()
{
	HashString name("block");
	float dist = 1.0f;
	Vertex_P3N3Tg3Bn3T2 vertices[4 * 6];
	uint indices[6 * 6];
	math::Vec3 tangent;
	math::Vec3 binormal;

	if ( g_pResource->GetModelMngr()->HasModelByName( name ) )
		return g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3N3Tg3Bn3T2::GetLayout() );

	// Front
	vertices[0].pos = math::Vec3( -dist, -dist, -dist );
	vertices[1].pos = math::Vec3( -dist,  dist, -dist );
	vertices[2].pos = math::Vec3(  dist,  dist, -dist );
	vertices[3].pos = math::Vec3(  dist, -dist, -dist );

	vertices[0].normal = math::Vec3( 0.0f, 0.0f, -1.0f );
	vertices[1].normal = vertices[0].normal;
	vertices[2].normal = vertices[0].normal;
	vertices[3].normal = vertices[0].normal;

	vertices[0].texCoord = math::Vec2( 0.0f, 1.0f );
	vertices[1].texCoord = math::Vec2( 0.0f, 0.0f );
	vertices[2].texCoord = math::Vec2( 1.0f, 0.0f );
	vertices[3].texCoord = math::Vec2( 1.0f, 1.0f );

	math::calculateTangentBinormal( vertices[0].pos, vertices[1].pos, vertices[2].pos, vertices[0].texCoord, vertices[1].texCoord, vertices[2].texCoord, tangent, binormal );

	vertices[0].tangent = tangent;
	vertices[1].tangent = tangent;
	vertices[2].tangent = tangent;
	vertices[3].tangent = tangent;

	vertices[0].binormal = binormal;
	vertices[1].binormal = binormal;
	vertices[2].binormal = binormal;
	vertices[3].binormal = binormal;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	// Back
	vertices[4].pos = math::Vec3(  dist, -dist,  dist );
	vertices[5].pos = math::Vec3(  dist,  dist,  dist );
	vertices[6].pos = math::Vec3( -dist,  dist,  dist );
	vertices[7].pos = math::Vec3( -dist, -dist,  dist );

	vertices[4].normal = math::Vec3( 0.0f, 0.0f, 1.0f );
	vertices[5].normal = vertices[4].normal;
	vertices[6].normal = vertices[4].normal;
	vertices[7].normal = vertices[4].normal;

	vertices[4].texCoord = math::Vec2( 0.0f, 1.0f );
	vertices[5].texCoord = math::Vec2( 0.0f, 0.0f );
	vertices[6].texCoord = math::Vec2( 1.0f, 0.0f );
	vertices[7].texCoord = math::Vec2( 1.0f, 1.0f );

	math::calculateTangentBinormal( vertices[4].pos, vertices[5].pos, vertices[6].pos, vertices[4].texCoord, vertices[5].texCoord, vertices[6].texCoord, tangent, binormal );

	vertices[4].tangent = tangent;
	vertices[5].tangent = tangent;
	vertices[6].tangent = tangent;
	vertices[7].tangent = tangent;

	vertices[4].binormal = binormal;
	vertices[5].binormal = binormal;
	vertices[6].binormal = binormal;
	vertices[7].binormal = binormal;


	indices[6] = 4;
	indices[7] = 5;
	indices[8] = 6;

	indices[ 9] = 6;
	indices[10] = 7;
	indices[11] = 4;

	// Right
	vertices[ 8].pos = math::Vec3(  dist, -dist, -dist );
	vertices[ 9].pos = math::Vec3(  dist,  dist, -dist );
	vertices[10].pos = math::Vec3(  dist,  dist,  dist );
	vertices[11].pos = math::Vec3(  dist, -dist,  dist );

	vertices[ 8].normal  = math::Vec3( 1.0f, 0.0f, 0.0f );
	vertices[ 9].normal  = vertices[8].normal;
	vertices[10].normal = vertices[8].normal;
	vertices[11].normal = vertices[8].normal;

	vertices[ 8].texCoord = math::Vec2( 0.0f, 1.0f );
	vertices[ 9].texCoord = math::Vec2( 0.0f, 0.0f );
	vertices[10].texCoord = math::Vec2( 1.0f, 0.0f );
	vertices[11].texCoord = math::Vec2( 1.0f, 1.0f );

	math::calculateTangentBinormal( vertices[8].pos, vertices[9].pos, vertices[10].pos, vertices[8].texCoord, vertices[9].texCoord, vertices[10].texCoord, tangent, binormal );

	vertices[ 8].tangent = tangent;
	vertices[ 9].tangent = tangent;
	vertices[10].tangent = tangent;
	vertices[11].tangent = tangent;

	vertices[ 8].binormal = binormal;
	vertices[ 9].binormal = binormal;
	vertices[10].binormal = binormal;
	vertices[11].binormal = binormal;

	indices[12] = 8;
	indices[13] = 9;
	indices[14] = 10;

	indices[15] = 10;
	indices[16] = 11;
	indices[17] = 8;

	// Left
	vertices[12].pos = math::Vec3( -dist, -dist,  dist );
	vertices[13].pos = math::Vec3( -dist,  dist,  dist );
	vertices[14].pos = math::Vec3( -dist,  dist, -dist );
	vertices[15].pos = math::Vec3( -dist, -dist, -dist );

	vertices[12].normal = math::Vec3( -1.0f, 0.0f, 0.0f );
	vertices[13].normal = vertices[12].normal;
	vertices[14].normal = vertices[12].normal;
	vertices[15].normal = vertices[12].normal;

	vertices[12].texCoord = math::Vec2( 0.0f, 1.0f );
	vertices[13].texCoord = math::Vec2( 0.0f, 0.0f );
	vertices[14].texCoord = math::Vec2( 1.0f, 0.0f );
	vertices[15].texCoord = math::Vec2( 1.0f, 1.0f );

	math::calculateTangentBinormal( vertices[12].pos, vertices[13].pos, vertices[14].pos, vertices[12].texCoord, vertices[13].texCoord, vertices[14].texCoord, tangent, binormal );

	vertices[12].tangent = tangent;
	vertices[13].tangent = tangent;
	vertices[14].tangent = tangent;
	vertices[15].tangent = tangent;

	vertices[12].binormal = binormal;
	vertices[13].binormal = binormal;
	vertices[14].binormal = binormal;
	vertices[15].binormal = binormal;

	indices[18] = 12;
	indices[19] = 13;
	indices[20] = 14;

	indices[21] = 14;
	indices[22] = 15;
	indices[23] = 12;

	// Up
	vertices[16].pos = math::Vec3( -dist,  dist, -dist );
	vertices[17].pos = math::Vec3( -dist,  dist,  dist );
	vertices[18].pos = math::Vec3(  dist,  dist,  dist );
	vertices[19].pos = math::Vec3(  dist,  dist, -dist );

	vertices[16].normal = math::Vec3(0.0f, 1.0f, 0.0f);
	vertices[17].normal = vertices[16].normal;
	vertices[18].normal = vertices[16].normal;
	vertices[19].normal = vertices[16].normal;

	vertices[16].texCoord = math::Vec2( 0.0f, 1.0f );
	vertices[17].texCoord = math::Vec2( 0.0f, 0.0f );
	vertices[18].texCoord = math::Vec2( 1.0f, 0.0f );
	vertices[19].texCoord = math::Vec2( 1.0f, 1.0f );

	math::calculateTangentBinormal( vertices[16].pos, vertices[17].pos, vertices[18].pos, vertices[16].texCoord, vertices[17].texCoord, vertices[18].texCoord, tangent, binormal );

	vertices[16].tangent = tangent;
	vertices[17].tangent = tangent;
	vertices[18].tangent = tangent;
	vertices[19].tangent = tangent;

	vertices[16].binormal = binormal;
	vertices[17].binormal = binormal;
	vertices[18].binormal = binormal;
	vertices[19].binormal = binormal;

	indices[24] = 16;
	indices[25] = 17;
	indices[26] = 18;

	indices[27] = 18;
	indices[28] = 19;
	indices[29] = 16;

	// Down
	vertices[20].pos = math::Vec3(  dist, -dist, -dist );
	vertices[21].pos = math::Vec3(  dist, -dist,  dist );
	vertices[22].pos = math::Vec3( -dist, -dist,  dist );
	vertices[23].pos = math::Vec3( -dist, -dist, -dist );

	vertices[20].normal = math::Vec3(0.0f, -1.0f, 0.0f);
	vertices[21].normal = vertices[20].normal;
	vertices[22].normal = vertices[20].normal;
	vertices[23].normal = vertices[20].normal;

	vertices[20].texCoord = math::Vec2( 0.0f, 1.0f );
	vertices[21].texCoord = math::Vec2( 0.0f, 0.0f );
	vertices[22].texCoord = math::Vec2( 1.0f, 0.0f );
	vertices[23].texCoord = math::Vec2( 1.0f, 1.0f );

	math::calculateTangentBinormal( vertices[20].pos, vertices[21].pos, vertices[22].pos, vertices[20].texCoord, vertices[21].texCoord, vertices[22].texCoord, tangent, binormal );

	vertices[20].tangent = tangent;
	vertices[21].tangent = tangent;
	vertices[22].tangent = tangent;
	vertices[23].tangent = tangent;

	vertices[20].binormal = binormal;
	vertices[21].binormal = binormal;
	vertices[22].binormal = binormal;
	vertices[23].binormal = binormal;

	indices[30] = 20;
	indices[31] = 21;
	indices[32] = 22;

	indices[33] = 22;
	indices[34] = 23;
	indices[35] = 20;

	Handle<Model> result = g_pResource->GetModelMngr()->Create( name, eTopology::Triangle, vertices, ARRAY_COUNT(vertices), indices, ARRAY_COUNT(indices) );
	return result;
}

/****************************************************************************************
	texture namespace
*****************************************************************************************/


//-------------------------------------------------------------------
//	createOneColorTexture
//-------------------------------------------------------------------
Handle<ITexture> createOneColorTexture(const uchar* pData, const String& name)
{
	Handle<ITexture> hTexture = g_pResource->GetTextureMngr()->Get( name );

	if ( hTexture.IsValid() )
		return hTexture;

	return g_pResource->GetTextureMngr()->CreateFromStream( name, pData, 1, 1, 32, false );
}
//-------------------------------------------------------------------
//	createWhite
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createWhite()
{
	uchar data[4] = { 255, 255, 255, 255 };
	return createOneColorTexture( data, "white" );
}
//-------------------------------------------------------------------
//	createGray
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createGray()
{
	uchar data[4] = { 128, 128, 128, 255 };
	return createOneColorTexture( data, "gray" );
}
//-------------------------------------------------------------------
//	createBlack
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createBlack()
{
	uchar data[4] = { 0, 0, 0, 255 };
	return createOneColorTexture( data, "black" );
}
//-------------------------------------------------------------------
//	createRed
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createRed()
{
	uchar data[4] = { 255, 0, 0, 255 };
	return createOneColorTexture( data, "Red" );
}
//-------------------------------------------------------------------
//	createGreen
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createGreen()
{
	uchar data[4] = { 0, 255, 0, 255 };
	return createOneColorTexture( data, "Green" );
}
//-------------------------------------------------------------------
//	createBlue
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createBlue()
{
	uchar data[4] = { 0, 0, 255, 255 };
	return createOneColorTexture( data, "Blue" );
}
//-------------------------------------------------------------------
//	createYellow
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createYellow()
{
	uchar data[4] = { 255, 255, 0, 255 };
	return createOneColorTexture( data, "Yellow" );
}
//-------------------------------------------------------------------
//	createOrange
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createOrange()
{
	uchar data[4] = { 255, 128, 0, 255 };
	return createOneColorTexture( data, "Orange" );
}
//-------------------------------------------------------------------
//	createPurple
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createPurple()
{
	uchar data[4] = { 128, 0, 128, 255 };
	return createOneColorTexture( data, "Purple" );
}
//-------------------------------------------------------------------
//	createPink
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createPink()
{
	uchar data[4] = { 255, 0, 255, 255 };
	return createOneColorTexture( data, "Pink" );
}
//-------------------------------------------------------------------
//	createNormal
//-------------------------------------------------------------------
Handle<ITexture> primitive::texture::createNormal()
{
	uchar data[4] = { 128, 128, 255, 255 };
	return createOneColorTexture( data, "normal" );
}
