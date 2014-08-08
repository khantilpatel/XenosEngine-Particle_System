#define PT_EMITTER 0
#define PT_FLARE 1


Texture1D gRandomTex;

SamplerState samLinear;

cbuffer cbPerFrame
{
	float1 gFrameTime;
	float1 gGameTime;
	float3 gEmitPosWT;	
};


struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age          : AGE;
	uint Type          : TYPE;
};


float3 RandUnitVec3(float offset)
{
	float u = (gGameTime + offset);
	
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	return normalize(v);
}

float3 RandVec3(float offset)
{
	float u = (gGameTime + offset);
	
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	return v;
}

[maxvertexcount(6)]
void main(
	point Particle input[1],
	inout PointStream<Particle> output)
{
	input[0].Age +=gFrameTime;
	
//	float3 v = gRandomTex.SampleLevel(samLinear, 5.0f, 0).xyz;

	float3 test = gEmitPosWT.xyz;

	if( input[0].Type == PT_EMITTER )
	{	
		if( input[0].Age > 0.002f )
		{
			for(int i = 0; i < 5; ++i)
			{

				float3 vRandom = 35.0f*RandVec3((float)i/5.0f);
				vRandom.y = 20.0f;
			
				Particle p;
				p.InitialPosW.x = gEmitPosWT.x; // + vRandom;
				p.InitialPosW.y = gEmitPosWT.y;
				p.InitialPosW.z = gEmitPosWT.z;
				float3 pos = gEmitPosWT;
					pos.z = pos.z + 10;
			//	p.InitialPosW = gEyePosWT;
				p.InitialVelW = float3(0.0f, 0.0f, 0.0f);
				p.SizeW       = float2(1.0f, 1.0f);
				p.Age         = 0.0f;
				p.Type        = PT_FLARE;
			
				output.Append(p);
			}
			// reset the time to emit
			input[0].Age = 0.0f;
		}
		
		// always keep emitters
		output.Append(input[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if( input[0].Age <= 3.0f )
			output.Append(input[0]);
	}		
}

Particle StreamOutVS(Particle vin)
{
	return vin;
}