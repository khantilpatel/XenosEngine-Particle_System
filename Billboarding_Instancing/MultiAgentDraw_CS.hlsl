

//Texture2D<float3> m_map : register(t0);
StructuredBuffer<float3> g_GridCenterListInput : register(t0);
RWStructuredBuffer<float3> bufferOut: register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint counter = 1;
	for (uint i = 0; i < 8; i++)
	{
		for (uint j = 0; j < 8; j++)
		{
			bufferOut[counter] = g_GridCenterListInput[counter];
			counter++;
		}
	}
	bufferOut[0].x = counter;
}