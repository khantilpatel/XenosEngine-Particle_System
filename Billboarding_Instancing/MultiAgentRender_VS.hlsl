

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4x4 gTexTransform;
};


float4 main( float3 posIn : POSITION ) : SV_POSITION
{

	float4 pos = float4(posIn, 1.0f);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	pos = mul(pos, worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);


	
	return pos;
}