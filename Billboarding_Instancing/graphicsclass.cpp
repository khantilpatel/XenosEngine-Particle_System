////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
	m_SimpleShader =0;

	m_ParticleShader = 0;
	m_ParticleSystem = 0;

	m_FireParticleShader=0;
	m_RainParticleSystem = 0;

	m_skyBox = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
	m_ParticleShader = 0;
	m_ParticleSystem = 0;
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	m_skyBox = new SkyBox ;
		if(!m_skyBox)
	{
		return false;
	}

	result = m_skyBox->Initiaize(m_D3D->GetDevice(), hwnd);
			if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the skybox object.", L"Error", MB_OK);
		return false;
	}
	// Set the initial position of the camera.
	//m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), L"seafloor.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	
	// Create the texture shader object.
	m_SimpleShader = new SimpleShaderClass;
	if(!m_SimpleShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_SimpleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the simple shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the particle shader object.
	m_ParticleShader = new ParticleShaderClass;
	if(!m_ParticleShader)
	{
		return false;
	}

	// Initialize the particle shader object.
	result = m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the particle system object.
	m_ParticleSystem = new ParticleSystemClass;
	if(!m_ParticleSystem)
	{
		return false;
	}

	// Initialize the particle system object.
	result = m_ParticleSystem->Initialize(m_D3D->GetDevice(), L"star.dds");
	if(!result)
	{

		MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
		return false;
	}

	m_FireParticleShader = new FireParticalSystemShader;
		// Initialize the particle system object.
	result = m_FireParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{

		MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
		return false;
	}

	m_RainParticleSystem = new RainParticleSystem;
		// Initialize the particle system object.
	result = m_RainParticleSystem->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{

		MessageBox(hwnd, L"Could not initialize the m_RainParticleSystem  object", L"Error", MB_OK);
		return false;
	}
			// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f,-8.0f,-10.0f);// -2.0f, -10.0f);


	return true;
}


bool GraphicsClass::UpdateFrame(float frameTime, float totalTime, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	bool result;
	m_frameTime = frameTime;
	m_TotalTime = totalTime;

	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

		// Run the frame processing for the particle system.
	m_ParticleSystem->UpdateFrame((frameTime), m_D3D->GetDeviceContext());

	// Redsfnder the graphics scene.
	result = Render();

	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result =true;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);


	/////////////////////////////////////////////////////////////////////
	// Render Diamonds Particle System
	//m_D3D->EnableAlphaBlending();

	//	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//m_ParticleSystem->Render(m_D3D->GetDeviceContext());

	//// Render the model using the texture shader.
	//result = m_ParticleShader->Render(m_D3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), 
	//	worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem->GetTexture());
	//if(!result)
	//{
	//	return false;
	//}	
	//m_D3D->DisableAlphaBlending();
	///////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////

	m_D3D->SetDepthStencilState_Less_Equal();
	m_D3D->SetRasterState_Nocull();

	m_skyBox->RenderShader(m_D3D->GetDeviceContext(),	worldMatrix, viewMatrix, 
	projectionMatrix, m_Camera->GetPosition_XM());

	m_D3D->EnableDepthStencilState();
	m_D3D->SetRasterState_Default();

	// Render Fire Particle System
	m_FireParticleShader->setEmit_Position(XMFLOAT3(-0.26f, -0.6f, -10.0f)); 	

	m_D3D->DisableDepthStencilState();
	m_FireParticleShader->Render(m_D3D->GetDeviceContext(),	worldMatrix, viewMatrix, 
	projectionMatrix, m_frameTime/1000, m_TotalTime/1000, m_Camera->GetPosition_XM());

	m_D3D->EnableAdditiveBlending();
	m_D3D->NoDepthWriteStencilState();
	m_D3D->EnableAlphaBlending();
	m_FireParticleShader->RenderShader_Draw(m_D3D->GetDeviceContext());
	m_D3D->EnableDepthStencilState();
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// Render Rain Particle System
	m_RainParticleSystem->setEmit_Position(m_Camera->GetPosition_XM());

	m_RainParticleSystem->Render(m_D3D->GetDeviceContext(),	worldMatrix, viewMatrix, 
	projectionMatrix, m_frameTime/1000, m_TotalTime/1000, m_Camera->GetPosition_XM());

	m_RainParticleSystem->RenderShader_Draw(m_D3D->GetDeviceContext());
	/////////////////////////////////////////////////////////////////////


		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//m_Model->Render(m_D3D->GetDeviceContext());



	// Render the model using the texture shader.
	///result = m_TextureShader-> Render(m_D3D->GetDeviceContext(), m_Model->GetVertexCount(), m_Model->GetInstanceCount(),
	//	worldMatrix, viewMatrix, projectionMatrix,m_Model->getVertexBuffer(), m_Model->getInstanceBuffer(), 
	//	m_Model->getStreamOutBuffer(), m_Model->GetTexture());

	//m_SimpleShader->Render(m_D3D->GetDeviceContext(), m_Model->GetVertexCount(), m_Model->GetInstanceCount(),
	//		m_FireParticleShader->m_DrawBuffer, m_Model->GetTexture());
	if(!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}


void GraphicsClass::Shutdown()
{
		// Release the particle system object.
	if(m_ParticleSystem)
	{
		m_ParticleSystem->Shutdown();
		delete m_ParticleSystem;
		m_ParticleSystem = 0;
	}

	// Release the particle shader object.
	if(m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = 0;
	}

	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the simple shader object.
	if(m_SimpleShader)
	{
		m_SimpleShader->Shutdown();
		delete m_SimpleShader;
		m_SimpleShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

