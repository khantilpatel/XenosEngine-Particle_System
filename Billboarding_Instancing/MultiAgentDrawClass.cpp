////////////////////////////////////////////////////////////////////////////////
// Filename: MultiAgentDrawClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "MultiAgentDrawClass.h"
#include "MultiAgentDraw_CSCompiled.h"
#include "MultiAgentDraw_VSCompiled.h"
#include "MultiAgentDraw_PSCompiled.h"

MultiAgentDrawClass::MultiAgentDrawClass()
{
	m_vertexShader = 0;
	m_computeShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
	m_ShaderUtility = new ShaderUtility;
}


MultiAgentDrawClass::MultiAgentDrawClass(const MultiAgentDrawClass& other)
{
}


MultiAgentDrawClass::~MultiAgentDrawClass()
{
}


bool MultiAgentDrawClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////
///////TASKS//////////////////////////////////////////////////
// 1. Comple shader and create the shader instance
// 2. Define Input layout for the vertex shader, vertex buffer data.
// 3. Initiate the Constant shader variable buffer and texture buffers.
////////////////////////////////////////////////////////////////////
bool MultiAgentDrawClass::InitializeShader(ID3D11Device* device, HWND hwnd)
{
	HRESULT result;
	ID3D10Blob* errorMessage;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	
	// Compile and Create PixelShader Object
	result = device->CreateComputeShader(g_cshader, sizeof(g_cshader), nullptr, &m_computeShader);
	if (FAILED(result))
	{
		return false;
	}

	// Compile and Create VertexShader Object
	result = device->CreateVertexShader(g_vshader, sizeof(g_vshader), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}


	// Compile and Create PixelShader Object
	result = device->CreatePixelShader(g_pshader, sizeof(g_pshader), nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}



	result = createInputLayoutDesc(device);
	if (FAILED(result))
	{
		return false;
	}

	result = createConstantBuffer_TextureBuffer(device);
	if (FAILED(result))
	{
		return false;
	}

	InitVertextBuffers( device); 

	// Load Texture for Floor and other stuff
	m_FloorTextureSRV = m_ShaderUtility->CreateTextureFromFile(device, L"Textures/edited_floor.dds"); 

	return true;
}

bool MultiAgentDrawClass::createInputLayoutDesc(ID3D11Device* device)
{
	HRESULT result = true;

	ID3D10Blob* errorMessage;
	
	unsigned int numElements;

	const D3D11_INPUT_ELEMENT_DESC Basic32[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	// Get a count of the elements in the layout.
	numElements = sizeof(Basic32) / sizeof(Basic32[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(Basic32, numElements, g_vshader, sizeof(g_vshader), &m_layout);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			//	OutputShaderErrorMessage(errorMessage, hwnd, csFilename);

			char* compileErrors;
			unsigned long bufferSize, i;
			ofstream fout;


			// Get a pointer to the error message text buffer.
			compileErrors = (char*)(errorMessage->GetBufferPointer());

			// Get the length of the message.
			bufferSize = errorMessage->GetBufferSize();

			// Open a file to write the error message to.
			fout.open("shader-error.txt");

			// Write out the error message.
			for (i = 0; i < bufferSize; i++)
			{
				fout << compileErrors[i];
			}

			// Close the file.
			fout.close();

			// Release the error message.
			errorMessage->Release();
			errorMessage = 0;
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			//MessageBox(hwnd, csFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}
	return result;
}


bool MultiAgentDrawClass::createConstantBuffer_TextureBuffer(ID3D11Device* device)
{

	D3D11_BUFFER_DESC drawBufferDesc;
	HRESULT result = true;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	drawBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	drawBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	drawBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	drawBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	drawBufferDesc.MiscFlags = 0;
	drawBufferDesc.StructureByteStride = 0;

	drawBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&drawBufferDesc, NULL, &m_world_matrix_buffer);
	if (FAILED(result))
	{
		result = false;
	}


	
	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool MultiAgentDrawClass::InitVertextBuffers(ID3D11Device* device)
{
	HRESULT result;
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 6, 4, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.Vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].Tex = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].Tex = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].Tex = cylinder.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	result = device->CreateBuffer(&vbd, &vinitData, &mShapesVB);
	if (FAILED(result))
	{
		return false;
	}
	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	result = device->CreateBuffer(&ibd, &iinitData, &mShapesIB);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void MultiAgentDrawClass::RenderShader(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	XMFLOAT3 camEyePos)
{
	bool result;

	ID3D11Buffer* bufferArrayNull[1] = { 0 };
	//XMMATRIX* worldMatrix1 = &XMMatrixTranslation(camEyePos.x, camEyePos.y, camEyePos.z);
	D3DXMATRIX worldMatrix1 = *(new D3DXMATRIX);

	//D3DXMatrixTranslation(&worldMatrix1, camEyePos.x, camEyePos.y, camEyePos.z);

	//	D3DXMatrixMultiply(ViewProj, &viewMatrix, &projectionMatrix);

	D3DXMatrixTranspose(&worldMatrix1, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);



	SetShaderParameters(deviceContext, worldMatrix1, viewMatrix, projectionMatrix);

	UINT stride = sizeof(Basic32);
	UINT offset = 0;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
	deviceContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);

	deviceContext->GSSetShader(NULL, NULL, 0);
	// deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->SOSetTargets(1, bufferArrayNull, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

	return;
}

bool MultiAgentDrawClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	//unsigned int bufferNumber;
	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_world_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->gridScaling = XMMatrixScaling(6.0f, 8.0f, 1.0f); //TODO: Transform Floor

	//cout<< "CamPos||X:" <<camEyePos.x << "||Y:"<<camEyePos.y <<"||Z:"<<camEyePos.z<<"||\n";
	// Unlock the constant buffer.
	deviceContext->Unmap(m_world_matrix_buffer, 0);
	///////////////////////////////////////////////////////////////
	// Geometry Shader Constant Resources Set HERE
	deviceContext->VSSetConstantBuffers(0, 1, &m_world_matrix_buffer);
	//deviceContext->PSSetShaderResources(0, 1, &m_CubeMapSRV);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	/////////////////////////////////////////////////////////////////
	deviceContext->PSSetShaderResources(0, 1, &m_FloorTextureSRV);
	return true;
}




void MultiAgentDrawClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}





void MultiAgentDrawClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

void MultiAgentDrawClass::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	// Release the vertex shader.
	if (m_computeShader)
	{
		m_computeShader->Release();
		m_computeShader = 0;
	}

	return;
}