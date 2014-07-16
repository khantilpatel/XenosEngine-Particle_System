
#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H


//#include <DirectXMath.h>
//#include "d3dUtil.h"

#include <windows.h>
#include <winbase.h>
#include <windef.h>
#include <winnt.h>
//#include <d3d11.h>
//#include <d3dx10math.h>
//#include <d3dx11async.h>
//#include <fstream>
#include <xnamath.h>
//#include "MathHelper.h"
#include <vector>

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex(){}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv){}
		Vertex(
			float px, float py, float pz, 
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px,py,pz), Normal(nx,ny,nz),
			  TangentU(tx, ty, tz), TexC(u,v){}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

		///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);
};

#endif