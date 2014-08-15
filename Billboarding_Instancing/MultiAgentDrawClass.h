////////////////////////////////////////////////////////////////////////////////
// Filename: MultiAgentDrawClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MultiAgentDrawCLASS_H_
#define _MultiAgentDrawCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "GeometryGenerator.h"
#include "ShaderUtility.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class MultiAgentDrawClass
{

	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		XMMATRIX gridScaling;		
	};

	struct Basic32
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct VertexType
	{
		D3DXVECTOR4 position;
		D3DXVECTOR2 texture;
	};

public:
	MultiAgentDrawClass();
	MultiAgentDrawClass(const MultiAgentDrawClass&);
	~MultiAgentDrawClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void RenderShader(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix,
		D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
		XMFLOAT3 camEyePos);

private:
	bool InitializeShader(ID3D11Device*, HWND);
	bool createInputLayoutDesc(ID3D11Device* device);
	bool createConstantBuffer_TextureBuffer(ID3D11Device* device);
	bool InitVertextBuffers(ID3D11Device* device);

	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);



	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix,
		D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11ComputeShader* m_computeShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

	// Texture Resource
	ID3D11SamplerState* m_sampleState;
	ID3D11ShaderResourceView* m_FloorTextureSRV;
	// Buffer data
	ID3D11Buffer* mShapesVB;
	ID3D11Buffer* mShapesIB;

	// Constant Buffer
	ID3D11Buffer* m_world_matrix_buffer;

	//Transformation from local to world Coordinates
	XMFLOAT4X4 mGridWorld;

	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	// Custom Classes and Utility
	ShaderUtility* m_ShaderUtility;
};

#endif