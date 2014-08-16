

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


struct VertexOut
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;	
};


[maxvertexcount(24)]
void main(
	point float4 input[1] : SV_POSITION, 
	inout TriangleStream< VertexOut > output
)
{
		//http://faculty.ycp.edu/~dbabcock/PastCourses/cs470/labs/lab04.html
		float scale = 0.5;
		//float halfscale = scale *0.5;
		VertexOut element0;
		
		element0.position.x = input[0].x - scale;
		element0.position.y = input[0].y - scale;
		element0.position.z = input[0].z - scale;
		element0.position.w = input[0].w;
		element0.position = mul(element.position, worldMatrix);
		element0.position = mul(element.position, viewMatrix);
		element0.position = mul(element.position, projectionMatrix);
		element0.tex.x = 1.0f;
		element0.tex.y = 0.0f;		
		
		VertexOut element1;
		element1.position.x = input[0].x - scale;
		element1.position.y = input[0].y + scale;
		element1.position.z = input[0].z - scale;
		element1.position.w = input[0].w;
		element1.position = mul(element.position, worldMatrix);
		element1.position = mul(element.position, viewMatrix);
		element1.position = mul(element.position, projectionMatrix);
		element1.tex.x = 0.0f;
		element1.tex.y = 0.0f;
		output.Append(element);
		//Top-left
		element.position.x = input[0].x - halfscale;
		element.position.y = input[0].y + scale;
		element.position.z = input[0].z - halfscale;
		element.position.w = input[0].w;
		element.position = mul(element.position, worldMatrix);
		element.position = mul(element.position, viewMatrix);
		element.position = mul(element.position, projectionMatrix);
		element.tex.x = 1.0f;
		element.tex.y = 1.0f;
		output.Append(element);

		//Right
		element.position.x = input[0].x + halfscale;
		element.position.y = input[0].y ;
		element.position.z = input[0].z - halfscale;
		element.position.w = input[0].w;
		element.position = mul(element.position, worldMatrix);
		element.position = mul(element.position, viewMatrix);
		element.position = mul(element.position, projectionMatrix);
		element.tex.x = 0.0f;
		element.tex.y = 0.0f;
		output.Append(element);
		// Top-Right 
		element.position.x = input[0].x + halfscale;
		element.position.y = input[0].y + scale;
		element.position.z = input[0].z - halfscale;
		element.position.w = input[0].w;
		element.position = mul(element.position, worldMatrix);
		element.position = mul(element.position, viewMatrix);
		element.position = mul(element.position, projectionMatrix);
		element.tex.x = 1.0f;
		element.tex.y = 0.0f;
		output.Append(element);
		//Top-left
		element.position.x = input[0].x - halfscale;
		element.position.y = input[0].y + scale;
		element.position.z = input[0].z - halfscale;
		element.position.w = input[0].w;
		element.position = mul(element.position, worldMatrix);
		element.position = mul(element.position, viewMatrix);
		element.position = mul(element.position, projectionMatrix);
		element.tex.x = 1.0f;
		element.tex.y = 1.0f;
		output.Append(element);
		/////////////////////////////////////////////////////////////////
		//// Front
		////Right
		//element.position.x = input[0].x + 0.5;
		//element.position.y = input[0].y;
		//element.position.z = input[0].z + halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 1.0f;
		//element.tex.y = 0.0f;
		//output.Append(element);
		//// Left		
		//element.position.x = input[0].x - halfscale;
		//element.position.y = input[0].y;
		//element.position.z = input[0].z + halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 0.0f;
		//element.tex.y = 0.0f;
		//output.Append(element);
		////Top-Right
		//element.position.x = input[0].x + halfscale;
		//element.position.y = input[0].y + scale;
		//element.position.z = input[0].z + halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 1.0f;
		//element.tex.y = 1.0f;
		//output.Append(element);
		////Top-Left
		//element.position.x = input[0].x - halfscale;
		//element.position.y = input[0].y + scale;
		//element.position.z = input[0].z + halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 0.0f;
		//element.tex.y = 1.0f;
		//output.Append(element);

		//////////////////////////////////////////////////////////////////////////
		//// Right
		//element.position.x = input[0].x + 0.5;
		//element.position.y = input[0].y;
		//element.position.z = input[0].z - halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 1.0f;
		//element.tex.y = 0.0f;
		//output.Append(element);
		//// Left		
		//element.position.x = input[0].x + 0.5;
		//element.position.y = input[0].y;
		//element.position.z = input[0].z + halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 1.0f;
		//element.tex.y = 0.0f;
		//output.Append(element);
		////Top-Right
		//element.position.x = input[0].x + halfscale;
		//element.position.y = input[0].y + scale;
		//element.position.z = input[0].z - halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 1.0f;
		//element.tex.y = 1.0f;
		//output.Append(element);
		////Top-Left
		//element.position.x = input[0].x + halfscale;
		//element.position.y = input[0].y + scale;
		//element.position.z = input[0].z + halfscale;
		//element.position.w = input[0].w;
		//element.position = mul(element.position, worldMatrix);
		//element.position = mul(element.position, viewMatrix);
		//element.position = mul(element.position, projectionMatrix);
		//element.tex.x = 1.0f;
		//element.tex.y = 1.0f;
		//output.Append(element);
}