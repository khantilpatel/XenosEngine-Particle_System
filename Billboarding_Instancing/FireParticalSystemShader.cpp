#include "FireParticalSystemShader.h"

LPCWSTR STR_CLASSNAME = L"FireParticalSystemShader";
FireParticalSystemShader::FireParticalSystemShader()
{
	m_initBuffer = 0;
	m_DrawBuffer = 0;
	m_StreamOutBuffer = 0;
	m_StagingStreamOutBuffer = 0;
	// Shader Variables 
	m_vertexShader_StreamOut = 0;
	m_pixelShader_StreamOut = 0;
	m_geometryShader_StreamOut = 0;

	m_vertexShader_Draw = 0;
	m_pixelShader_Draw = 0;
	m_geometryShader_Draw = 0;

	m_layout_streamOut = 0;
	m_layout_Draw = 0;

	//m_MaxParticles = 0;

	m_particle_parameter_buffer = 0;
	m_sampleState_Shader = 0;

	m_ShaderUtility = new ShaderUtility;

	m_EyePosW  = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Emit_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_EmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}


FireParticalSystemShader::~FireParticalSystemShader()
{

}


FireParticalSystemShader::FireParticalSystemShader(const FireParticalSystemShader& other){



}


bool FireParticalSystemShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//m_MaxParticles  = 10000;
	flag_FirstRun = true;
	// Initialize the vertex and pixel shaders.
	result = InitializeShader_StreamOut(device, hwnd, L"FireShader.vs", L"FireShader.vs" ,L"ddd");
	if(!result)
	{
		MessageBox(hwnd, STR_CLASSNAME, L"InitializeShader_StreamOut:: FAILED", MB_OK);
		return false;
	}

	//result = InitializeShader_Draw(device, hwnd, L"FireShader.vs", L"FireShader.vs" ,L"FireShader.vs");
	if(!result)
	{
		MessageBox(hwnd, STR_CLASSNAME, L"InitializeShader_Draw:: FAILED", MB_OK);
		return false;
	}


	return true;
}


bool FireParticalSystemShader::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
									  D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
									  float frameTime, float gameTime, XMFLOAT3 camEyePos )
{
	HRESULT result;
	D3DXMATRIX* ViewProj = new D3DXMATRIX;
		
	D3DXMatrixMultiply(ViewProj, &viewMatrix, &projectionMatrix);

		///////// STREAM_OUT/////////
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters_StreamOut(deviceContext, frameTime,gameTime,camEyePos,ViewProj);
	if(!result)
	{
		//return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader_StreamOut(deviceContext);

	//RenderShader_Draw(deviceContext);
	return  true;
}

void FireParticalSystemShader::Shutdown()
{
	ShutdownShader_StreamOut();
	ShutdownShader_Draw();
}


void FireParticalSystemShader::RenderShader_StreamOut(ID3D11DeviceContext* deviceContext)
{
	//HRESULT result;

	unsigned int stride;
	unsigned int offset ;
	ID3D11Buffer* bufferPointers;

	// Set the buffer strides.
	stride = sizeof(Particle);

	// Set the buffer offsets.
	offset = 0;

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	deviceContext->SOSetTargets(1, &m_StreamOutBuffer, &offset);
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout_streamOut);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader_StreamOut, NULL, 0);
	deviceContext->GSSetShader(m_geometryShader_StreamOut, NULL, 0);


	ID3D11Buffer* bufferArray[1] = {0};
	deviceContext->PSSetShader(NULL, NULL, 0);
	

	

    
	if(flag_FirstRun)
	{
		bufferPointers = m_initBuffer;
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &bufferPointers, &stride, &offset);
		deviceContext->Draw(1,0);
		//flag_FirstRun = false;
	}
	else
	{
		bufferPointers = m_DrawBuffer;
		deviceContext->IASetVertexBuffers(0, 1, &bufferPointers, &stride, &offset);
		deviceContext->DrawAuto();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// DEBUG
//	deviceContext->CopyResource( m_StagingStreamOutBuffer, m_StreamOutBuffer );
//
//D3D11_MAPPED_SUBRESOURCE data;
//deviceContext->Map( m_StagingStreamOutBuffer, 0, D3D11_MAP_READ, 0, &data ) ;
//
//	struct GS_OUTPUT
//	{
//		D3DXVECTOR3 COLOUR;
//		D3DXVECTOR3 DOMAIN_SHADER_LOCATION;
//		D3DXVECTOR3 WORLD_POSITION;
//	};
//
//	Particle *pRaw = reinterpret_cast< Particle* >( data.pData );
//
//
//	/* Work with the pRaw[] array here */
//	// Consider StringCchPrintf() and OutputDebugString() as simple ways of printing the above struct, or use the debugger and step through.
//	
//	deviceContext->Unmap( m_StagingStreamOutBuffer, 0 );
//
	//////////////////////////////////////////////////////////////////////////////////

//	swap(m_DrawBuffer, m_StreamOutBuffer);
	


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 1. Comple Shaders VS, GS for StreamOut Stage (m_vertexShader_StreamOut|m_geometryShader_StreamOut)
// 2. Create InputLayout for VS stage as struct Particle |vertex buffer bind type|(m_layout_streamOut)
// 3. Create Vertex Buffer m_initBuffer m_DrawBuffer m_StreamOutBuffer
// 4. Create Constant/Uniform Buffer for m_particle_parameter_buffer|m_sampleState_Shader|m_RandomTexSRV_Shader.
bool FireParticalSystemShader::InitializeShader_StreamOut(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename,
														  WCHAR* gsFilename ,WCHAR* psFilename )
{

	HRESULT result;
	ID3D10Blob* errorMessage;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1. Comple Shaders VS, GS for StreamOut Stage (m_vertexShader_StreamOut|m_geometryShader_StreamOut)
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* geometryShaderBuffer;
	

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "StreamOutVS", "vs_5_0",
							D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(gsFilename, NULL, NULL, "StreamOutGS", "gs_5_0", 0, 0, NULL, 
								   &geometryShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, gsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, gsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		NULL, &m_vertexShader_StreamOut);
	if(FAILED(result))
	{
		return false;
	}


	////////////////////////////////////////////////////////////////////
	///Create Geometry Shader with Stream-out
	/////////////////////////////////////////////////

	D3D11_SO_DECLARATION_ENTRY pDecl[] =
	{
		//// semantic name, semantic index, start component, component count, output slot
			{ 0 ,"POSITION" , 0, 0, 3, 0 },   // output all components of position
		{ 0 ,"VELOCITY" , 0, 0, 3, 0 }, 	     // output the first 2 texture coordinates
		{ 0 ,"SIZE"     , 0, 0, 2, 0 }, 	     // output the first 2 texture coordinates
		{ 0 ,"AGE"      , 0, 0, 1, 0 },  	     // output the first 2 texture coordinates
		{ 0 ,"TYPE"     , 0, 0, 1, 0 }, 	     // output the first 2 texture coordinates

			// semantic name, semantic index, start component, component count, output slot
		//{ 0 ,"POSITION", 0, 0, 3, 0 },   // output all components of position
		//{ 0 ,"VELOCITY"  , 0, 0, 3, 0 }, 	     // output the first 2 texture coordinates
	};


	result = device->CreateGeometryShaderWithStreamOutput( geometryShaderBuffer->GetBufferPointer(), 
		geometryShaderBuffer->GetBufferSize(), pDecl, 5, NULL, 
		0,  D3D11_SO_NO_RASTERIZED_STREAM, NULL, &m_geometryShader_StreamOut );






	if(FAILED(result))
	{
		return false;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Create InputLayout for VS stage as struct Particle |vertex buffer bind type|(m_layout_streamOut)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	result = createInputLayoutDesc_StreamOut(device, vertexShaderBuffer);

	if(FAILED(result))
	{
		return false;
	}
	
	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;

	//////////////////////////////////////////////////////////////////////////////////
	// 3. Create Vertex Buffer m_initBuffer m_DrawBuffer m_StreamOutBuffer
	//////////////////////////////////////////////////////////////////////////////////
	FireParticalSystemShader::InitVertextBuffers(device);

	////////////////////////////////////////////////
	// Emit postion for particals in fire effect
	m_Emit_Position = XMFLOAT3(0.0f, 2.0f, 2.0f); 
	////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. Create Constant/Uniform Buffer for m_particle_parameter_buffer|m_sampleState_Shader|m_RandomTexSRV_Shader.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	createConstantBuffer_StreamOut(device);


	return true;
}

bool FireParticalSystemShader::InitVertextBuffers(ID3D11Device* device)
{
	

	///////////////////////////////////////////////////////////////////////
	// Create Vertex buffer for initial injection of the emiter particle
	// 1. m_initBuffer
	///////////////////////////////////////////////////////////////////////

	Particle* particle;
	D3D11_BUFFER_DESC particleBufferDesc;
    D3D11_SUBRESOURCE_DATA particleDataResource;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 1;

	// Create the vertex array.
	particle  = new Particle;	
	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	//ZeroMemory(&particle, sizeof(Particle));
	particle->Size =  XMFLOAT2(0.0,10.0);
	particle->InitialVel = XMFLOAT3(3.0,0.0,0.0);
	particle->InitialPos = XMFLOAT3(12.0,32.0,0.0);
	particle->Age  = 23.0f;
	particle->Type = 1; 
	
    particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    particleBufferDesc.ByteWidth = sizeof(Particle) * m_vertexCount;
    particleBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    particleBufferDesc.CPUAccessFlags = 0;
    particleBufferDesc.MiscFlags = 0;
	particleBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    particleDataResource.pSysMem = particle;
	particleDataResource.SysMemPitch = 0;
	particleDataResource.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&particleBufferDesc, &particleDataResource, &m_initBuffer);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the vertex array now that the vertex buffer has been created and loaded.
	delete particle;	
	particle = 0;

	///////////////////////////////////////////////////////////////////////
	// Create Stream-out buffer instance without binding any data
	// 1. m_StreamOutBuffer.
	// 2. m_DrawBuffer.
	///////////////////////////////////////////////////////////////

	int m_nBufferSize = m_MaxParticles;

		D3D11_BUFFER_DESC bufferDesc =
		{
			m_nBufferSize,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_STREAM_OUTPUT,
			0,
			0,
			0
		};

	// Set up the description of the instance buffer.
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(Particle) * m_nBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT ;//| D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer( &bufferDesc, NULL, &m_StreamOutBuffer );

		if(FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer( &bufferDesc, NULL, &m_DrawBuffer );

		if(FAILED(result))
	{
		return false;
	}

		D3D11_BUFFER_DESC soDesc;


soDesc.ByteWidth			= sizeof(Particle) *m_nBufferSize; // 10mb
soDesc.MiscFlags			= 0;
soDesc.StructureByteStride	= 0;
soDesc.BindFlags		= 0;
soDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
soDesc.Usage			= D3D11_USAGE_STAGING;

result = device->CreateBuffer( &soDesc, NULL, &m_StagingStreamOutBuffer );

	
	if(FAILED(result))
	{
		return false;
	}

		return true;
}

bool FireParticalSystemShader::createInputLayoutDesc_StreamOut(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer )
{
	HRESULT result = true;
	const D3D11_INPUT_ELEMENT_DESC m_Particle[5] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
		
	unsigned int numElements;


	// Get a count of the elements in the layout.
    numElements = sizeof(m_Particle) / sizeof(m_Particle[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(m_Particle, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout_streamOut);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}





///////////////////////////////////////////////////////////////////////////////
// Binds Shader Parameters at runtime every frame:
// 1. ParticleParametersBufferType.
// 2. Random function Texture.
// 3. Sampler for the  texture sampling function.
bool FireParticalSystemShader::SetShaderParameters_StreamOut(ID3D11DeviceContext* deviceContext,
															 float timeStep, float gameTime,
															 XMFLOAT3 camEyePos, D3DXMATRIX* ViewProj )
{

	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	ParticleParametersBufferType* dataPtr;
	unsigned int bufferNumber;
	
	////////////////////////////////////////////////////////////
	// Copy the values from CPU to Buffer for later use in GPU
	////////////////////////////////////////////////////////////

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_particle_parameter_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (ParticleParametersBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->gTimeStep = 0.5;
	dataPtr->gGameTime = gameTime;
	dataPtr->gEmitPosition = m_Emit_Position;
	dataPtr->gEyePosition = camEyePos;
	dataPtr->gViewProj = *ViewProj;

	// Unlock the constant buffer.
    deviceContext->Unmap(m_particle_parameter_buffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
	

	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_particle_parameter_buffer);
//	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//////////////////////////////////////////////////////////////
	// Set the Randome Texture resource into the pipleine 
	//////////////////////////////////////////////////////////////
	// Set shader texture resource in the pixel shader.
	deviceContext->GSSetShaderResources(0, 1, &m_RandomTexSRV_Shader);

	deviceContext->PSSetShaderResources(0, 1, &m_FireTexture);
	//////////////////////////////////////////////////////////////
	// Set the Sampler state object into the pipleine 
	//////////////////////////////////////////////////////////////
	deviceContext->GSSetSamplers(0, 1, &m_sampleState_Shader);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState_Shader);

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// 1. Create a particle buffer to send parameters for particle system stream-out shader
// 2. Create a random texture resource for random function in shader
// 3. Create a Sample Descriptor for pixel shader or sampling form texture
bool FireParticalSystemShader::createConstantBuffer_StreamOut(ID3D11Device* device)
{
	D3D11_BUFFER_DESC particleBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	HRESULT result = true;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    particleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	particleBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    particleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    particleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    particleBufferDesc.MiscFlags = 0;
	particleBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&particleBufferDesc, NULL, &m_particle_parameter_buffer);
	if(FAILED(result))
	{
		result = false;
	}

	m_RandomTexSRV_Shader = m_ShaderUtility->CreateRandomTexture1DSRV(device);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState_Shader);
	if(FAILED(result))
	{
		return false;
	}
	
	m_FireTexture =  m_ShaderUtility->CreateTextureFromFile(device, L"flare0.dds");

	return true;
}
//////////////////////////////////////////////////////////////////////////////


void FireParticalSystemShader::ShutdownShader_StreamOut()
{


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 1. Comple Shaders VS, GS for StreamOut Stage (m_vertexShader_Draw|m_geometryShader_Draw|m_PixelShader_Draw)
// 2. Create InputLayout for VS stage as struct Particle |vertex buffer bind type|(m_layout_Draw)
bool FireParticalSystemShader::InitializeShader_Draw(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, 
													 WCHAR* gsFilename ,WCHAR* psFilename)
{

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* geometryShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;


	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1. Comple Shaders VS, GS for StreamOut Stage (m_vertexShader_StreamOut|m_geometryShader_StreamOut)
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "DrawVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	  // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "DrawGS", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &geometryShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "DrawPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	
	

    // Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader_Draw);
	if(FAILED(result))
	{
		return false;
	}

	    // Create the pixel shader from the buffer.
	result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader_Draw);
	if(FAILED(result))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader_Draw);
	if(FAILED(result))
	{
		return false;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Create InputLayout for VS stage as struct Particle |vertex buffer bind type|(m_layout_Draw)
	////////////////////////////////////////////////////////////////////////////////////////////////

	result = createInputLayoutDesc_Draw(device, vertexShaderBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;
	
	//result = createConstantBuffer_StreamOut(device);
	/*if(FAILED(result))
	{
		return false;
	}*/

	return true;
}





//////////////////////////////
// DRAW SHADER FUNCTIONS
//////////////////////////////

bool FireParticalSystemShader::createConstantBuffer_Draw(ID3D11Device* device)
{

	return true;
}

bool FireParticalSystemShader::createInputLayoutDesc_Draw(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer )
{
		HRESULT result = true;
	const D3D11_INPUT_ELEMENT_DESC m_Particle[5] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
		
	unsigned int numElements;


	// Get a count of the elements in the layout.
    numElements = sizeof(m_Particle) / sizeof(m_Particle[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(m_Particle, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout_Draw);
	if(FAILED(result))
	{
		return false;
	}


	return true;
}


bool FireParticalSystemShader::RenderShader_Draw(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides;
	unsigned int offsets;

	strides = sizeof(Particle); 	

	// Set the buffer offsets.
	offsets = 0;
	
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGL);

	ID3D11Buffer* bufferArray[1] = {0};

	deviceContext->SOSetTargets(1, bufferArray, 0);
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	deviceContext->IASetVertexBuffers(0, 1, &m_DrawBuffer, &strides, &offsets);
	
	// Set the vertex input layout.
	//deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader_Draw, NULL, 0);
	deviceContext->GSSetShader(m_geometryShader_Draw, NULL, 0);
   // deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	
	deviceContext->PSSetShader(m_pixelShader_Draw, NULL, 0);
	// Set the sampler state in the pixel shader.
	//deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawAuto();//(vertexCount, instanceCount, 0, 0);
	return true;
}

bool FireParticalSystemShader::SetShaderParameters_Draw(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, 
															 D3DXMATRIX, ID3D11ShaderResourceView*)
{
	return true;
}





void FireParticalSystemShader::ShutdownShader_Draw()
{


}


void FireParticalSystemShader::OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*)
{

}