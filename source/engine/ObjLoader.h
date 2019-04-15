#pragma once

#include "../engine/Vertex.h"
#include "Pointer.h"
#include "String.h"
#include "../engine/File.h"
#include "Math.h"
#include "Array.h"

struct ObjData
{
	Vertex_P3N3Tg3Bn3T2* vertices;
	uint vertexCount;
	uint* indices;
	uint indexCount;

	void Clear()
	{
		safeArrayRelease(vertices);
		vertexCount = 0;
		safeArrayRelease(indices);
		indexCount = 0;
	}

	ObjData() : vertices(nullptr), vertexCount(0), indices(nullptr), indexCount(0) {}
	~ObjData() { Clear(); }
};

class ObjLoader
{
	public:
		ObjLoader();
		~ObjLoader();

		bool Load(const String& pathFile);
		void Unload();

		bool IsLoaded()const { return m_loaded; }

		const ObjData& GetData()const;

	private:
		void FileCount(const FileReader&, 
			FixedArray<math::Vec3>& outPoints, FixedArray<math::Vec2>& outTexCoords, FixedArray<math::Vec3>& outNormals, FixedArray<Vertex_P3N3Tg3Bn3T2>& outVertices);

		bool Is(const char* line, uint size, const String& cmp)const;
		bool IsVertex(const char*, uint size)const;
		bool IsVertex(const String&)const;
		bool IsTexCoord(const char*, uint size)const;
		bool IsTexCoord(const String&)const;
		bool IsNormal(const char*, uint size)const;
		bool IsNormal(const String&)const;
		bool IsFace(const char*, uint size)const;
		bool IsFace(const String&)const;

		uint ReadLine(const char* line, uint size, String* outResult, uint resultMax)const;
		math::Vec2 ReadLine_Vec2(const char*, uint size)const;
		math::Vec3 ReadLine_Vec3(const char*, uint size)const;
		void ReadLine_Face(const char*, uint size, int* outFaces, uint faceMax)const;

		bool m_loaded;
		ObjData m_data;
};
