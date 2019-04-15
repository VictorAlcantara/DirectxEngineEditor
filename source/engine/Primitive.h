#pragma once

#include "../engine/Handle.h"
#include "../engine/Model.h"
#include "../engine/Texture.h"

namespace primitive
{
	namespace geometry
	{
		Handle<Model> createLine();
		Handle<Model> createCubeLine();
		Handle<Model> createCircleLine(uint lines);
		Handle<Model> createSphereLine(uint stacks, uint halfSlices);
		Handle<Model> createCylinderLine(uint halfSlices);
		Handle<Model> createPyramidLine(uint baseVertexCount);
		Handle<Model> createCircle(uint slices);
		Handle<Model> createSquare();
		Handle<Model> createBlock();
	};

	namespace texture
	{
		Handle<ITexture> createWhite();
		Handle<ITexture> createGray();
		Handle<ITexture> createBlack();
		Handle<ITexture> createRed();
		Handle<ITexture> createGreen();
		Handle<ITexture> createBlue();
		Handle<ITexture> createYellow();
		Handle<ITexture> createOrange();
		Handle<ITexture> createPurple();
		Handle<ITexture> createPink();
		Handle<ITexture> createNormal();

		static Handle<ITexture> (*const createFunctionPtrs[])() = { createWhite, createGray, createBlack, 
			createRed, createGreen, createBlue, createYellow, 
			createOrange, createPurple, createPink, createNormal };
	};
};

////-------------------------------------------------------------------
////	CreateModel (cylinder)
////-------------------------------------------------------------------
//void BlockPosition::CreateModel(uint sliceCount)
//{
//	const uint vertexCount = sliceCount * 2 + 2;
//	const uint indexCount = sliceCount * 12;
//	Vertex_P3N3* vertices = new Vertex_P3N3[vertexCount];
//	uint* indices = new uint[indexCount];
//	float y = 1.0f;
//	math::Vec4 rotationPoint( 1.0f, 0.0f, 0.0f, 0.0f );
//	float rotationDegs = 360.0f / (float)sliceCount;
//
//	{
//		uint index = 0;
//
//		// Top
//		vertices[0].pos = math::Vec3(0.0f, y, 0.0f);
//		vertices[0].normal = math::Vec3(0.0f, 1.0f, 0.0f);
//		for ( uint slice = 0; slice < sliceCount; slice++ )
//		{
//			index = slice + 1;
//			math::Mtx44 rotation = math::matrixRotateDegs( 0.0f, 1.0f, 0.0f, rotationDegs * (float)slice );
//
//			vertices[index].pos = (rotationPoint * rotation).Xyz();
//			vertices[index].normal = math::normalize(vertices[index].pos);
//			vertices[index].pos.y = y;
//		}
//
//		// Bottom
//		for ( uint slice = 0; slice < sliceCount; slice++ )
//		{
//			index = sliceCount + slice + 1;
//			vertices[index] = vertices[slice + 1];
//			vertices[index].pos.y = -y;
//		}
//		index = vertexCount - 1;
//		vertices[index].pos = math::Vec3(0.0f, -y, 0.0f);
//		vertices[index].normal = math::Vec3(0.0f, -1.0f, 0.0f);
//	}
//
//	// Index
//	{
//		uint index = 0;
//
//		// Top
//		for ( uint slice = 0; slice < sliceCount; slice++ )
//		{
//			index = slice * 3;
//
//			indices[index++] = 0;
//			indices[index++] = slice + 1;
//			indices[index++] = (slice + 1) % sliceCount + 1;
//		}
//
//		// Middle
//		for ( uint slice = 0; slice < sliceCount; slice++ )
//		{
//			index = (sliceCount * 3) + slice * 6;
//			uint sliceTopBase = slice + 1;
//			uint sliceTopNext = sliceTopBase % sliceCount + 1;
//			uint sliceBottomBase = sliceTopBase + sliceCount;
//			uint sliceBottomNext = (sliceCount + 1) + (slice + 1) % sliceCount;
//
//			indices[index++] = sliceTopBase;
//			indices[index++] = sliceBottomBase;
//			indices[index++] = sliceTopNext;
//
//			indices[index++] = sliceTopNext;
//			indices[index++] = sliceBottomBase;
//			indices[index++] = sliceBottomNext;
//		}
//
//		// Top
//		index = indexCount - sliceCount * 3;
//		for ( uint slice = 0; slice < sliceCount; slice++ )
//		{
//			indices[index++] = vertexCount - 1;
//			indices[index++] = (sliceCount + 1) + (slice + 1) % sliceCount;
//			indices[index++] = (sliceCount + 1) + slice;
//		}
//	}
//
//	m_pModel = new Model;
//	m_pModel->Initialize( eTopology::Triangle, vertices, vertexCount, indices, indexCount );
//
//	safeArrayRelease( indices );
//	safeArrayRelease( vertices );
//}

////-------------------------------------------------------------------
////	CreateModel (sphere)
////-------------------------------------------------------------------
//void BlockResize::CreateModel(uint numStacks, uint numSlices)
//{
//	const uint numVerts = 2 + numSlices * numStacks;
//	const uint numIndices =  6 * numSlices * numStacks;
//	Vertex_P3N3* vertices = new Vertex_P3N3[numVerts];
//	uint* indices = new uint[numIndices];
//	math::Vec3 point( 0.0f, 1.0f, 0.0f );
//	float rotateZ = 180.0f / (float)(numStacks + 1U);
//	float rotateY = 360.0f / (float)numSlices;
//	uint index = 0;
//	const uint numColors = numStacks + 1;
//	math::Vec4* colors = new math::Vec4[numColors];
//	colors[0] = math::Vec4( 0.0f, 0.05f, 0.1f, 1.0f );
//	const float intensityStart = 0.3f;
//
//	for ( uint i = 1; i < numColors; i++ )
//	{
//		float d = (float)i / (float)numColors;
//		colors[i] = colors[0];
//		colors[i].y += d * 0.5f;
//		colors[i].z += d;
//	}
//
//	for ( uint i = 0; i < numVerts; i++ )
//	{
//		vertices[i].pos = math::Vec3();
//		vertices[i].normal = math::Vec3();
//	}
//
//	for ( uint i = 0; i < numIndices; i++ )
//		indices[i] = 0;
//
//	// Top
//	vertices[index].pos = point;
//	vertices[index].normal = math::Vec3(0.0f, 1.0f, 0.0f);
//
//	// Sides
//	for ( uint i = 0; i < numStacks; i++ )
//	{
//		math::Mtx44 rotateStack = math::matrixRotateDegs( 0.0f, 0.0f, -1.0f, (float)(i + 1U) * rotateZ );
//
//		for ( uint j = 0; j < numSlices; j++ )
//		{
//			float colorIntensity = (float)(j + 1) / (float)numSlices;
//
//			if ( colorIntensity > 0.5f )
//				colorIntensity = 1.0f - colorIntensity;
//
//			colorIntensity = intensityStart + (1.0f - intensityStart) * colorIntensity;
//
//			math::Mtx44 rotateSlice = math::matrixRotateDegs( 0.0f, 1.0f, 0.0f, (float)j * rotateY );
//			math::Vec3 pos = (math::Vec4(point, 1.0f) * rotateStack * rotateSlice).Xyz();
//
//			index++;
//			vertices[index].pos = pos;
//			vertices[index].normal = math::normalize(pos);
//		}
//	}
//
//	// Bottom
//	index++;
//	vertices[index].pos = -point;
//	vertices[index].normal = math::Vec3(0.0f, -1.0f, 0.0f);
//
//	// Top
//	for ( uint slice = 0; slice < numSlices; slice++ )
//	{
//		uint idx = slice * 3;
//
//		indices[idx++] = 0;
//		indices[idx++] = slice + 1;
//		indices[idx++] = ( (slice + 1) % numSlices ) + 1;
//	}
//
//	// Sides
//	for ( uint stack = 1; stack < numStacks; stack++ )
//	{
//		uint sliceTopBase = (stack-1) * numSlices + 1;
//		uint sliceCurrBase = sliceTopBase + numSlices;
//
//		for ( uint slice = 0; slice < numSlices; slice++ )
//		{
//			uint idx = 3 * numSlices + (stack - 1) * numSlices * 6 + slice * 6;
//			uint sliceTopIdx = sliceTopBase + slice;
//			uint sliceCurrIdx = sliceCurrBase + slice;
//			uint sliceIdxNormalized = (slice + 1) % numSlices;
//
//			indices[idx++] = sliceTopBase + slice;
//			indices[idx++] = sliceCurrIdx;
//			indices[idx++] = sliceTopBase + sliceIdxNormalized;
//
//			indices[idx++] = sliceTopBase + sliceIdxNormalized;
//			indices[idx++] = sliceCurrIdx;
//			indices[idx++] = sliceCurrBase + sliceIdxNormalized;
//
//		}
//	}
//
//	// Bottom
//	uint idxBottomVertex = numVerts - 1;
//	for ( uint slice = 0; slice < numSlices; slice++ )
//	{
//		uint idx = numIndices + 3 * (slice - numSlices);
//		uint sliceTopBase = 1 + numSlices * (numStacks - 1);
//
//		indices[idx++] = sliceTopBase + slice;
//		indices[idx++] = idxBottomVertex;
//		indices[idx++] = sliceTopBase + ((slice+1)  % numSlices );
//	}
//
//	m_pModel = new Model;
//	//pSphere->Initialize( eTopology::LineStrip, vertices, numVerts );
//	m_pModel->Initialize( eTopology::Triangle, vertices, numVerts, indices, numIndices );
//
//	safeArrayRelease( vertices );
//	safeArrayRelease( indices );
//	safeArrayRelease( colors );
//}


