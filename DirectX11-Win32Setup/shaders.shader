
cbuffer ConstantBuffer : register(b0)
{
	float RedLevel;
	float BlueLevel;
}

cbuffer ConstantOffset : register(b1)
{
	float3 Offset;
}

cbuffer FinalTransform : register(b2)
{
	float4x4 finalMat;
	float4x4 rotation;
	float4 lightvec;      // the light's vector
    float4 lightcol;      // the light's color
    float4 ambientcol;    // the ambient light's color
}

struct VOut
{
	  float4 color : COLOR;
    float2 texcoord : TEXCOORD;    // texture coordinates
    float4 position : SV_POSITION;
};

Texture2D Texture;
SamplerState ss;

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VOut output;
	//    output.position = position;
	//output.position.x += Offset.x;
	//output.position.y += Offset.y;
	//output.position.xy *= Offset.z;
   // output.color =  color;
	//output.color.r = RedLevel;
	//output.color.b = BlueLevel;

	output.position = mul(finalMat,position);
	output.color = ambientcol;

    float4 norm = normalize(mul(rotation, normal));
    float diffusebrightness = saturate(dot(norm, lightvec));
    output.color += lightcol * diffusebrightness;    // find the diffuse color and add7

	 output.texcoord = texcoord; 

    return output;
}



float4 PShader(float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{

    //position.xy *= 0.4f;
	//color.r = 1.0f;
	//float4 realColor = {1.0f,1.0f,1.0f,0.0f};
    return color * Texture.Sample(ss, texcoord);
}
