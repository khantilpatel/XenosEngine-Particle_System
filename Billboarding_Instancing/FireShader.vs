
cbuffer cbPerFrame
{
	//float3 gEyePosW;
	
	// for when the emit position/direction is varying
	//float3 gEmitPosW;
	//float3 gEmitDirW;

	float gTimeStep;
	float gGameTime;	
	float3 gEmitPosW;
	//float4x4 gViewProj; 
};

// Array of textures for texturing the particles.
//Texture2DArray gTexArray;

// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex;
SamplerState samLinear;

//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	// project onto unit sphere
	return normalize(v);
}

//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_EMITTER 0
#define PT_FLARE 1
 
struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age          : AGE;
	uint Type          : TYPE;
};


Particle StreamOutVS(Particle vin)
{
	return vin;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(2)]
void StreamOutGS(point Particle gin[1], 
                 inout PointStream<Particle> ptStream)
{	
	gin[0].Age += gTimeStep;
	
	if( gin[0].Type == PT_EMITTER )
	{	
		// time to emit a new particle?
		if( gin[0].Age > 0.005f )
		{
			float3 vRandom = RandUnitVec3(0.0f);
			vRandom.x *= 0.5f;
			vRandom.z *= 0.5f;
			
			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = 4.0f*vRandom;
			p.SizeW       = float2(3.0f, 3.0f);
			p.Age         = 0.0f;
			p.Type        = PT_FLARE;
			
			ptStream.Append(p);
			
			// reset the time to emit
			gin[0].Age = 0.0f;
		}
		
		// always keep emitters
		ptStream.Append(gin[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if( gin[0].Age <= 1.0f )
			ptStream.Append(gin[0]);
	}		
}

