cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
  float4 Color : COLOR;
};

//VertexOut VS( float4 position : POSITION, float4 color : COLOR )
VertexOut VS( float3 position : POSITION, float4 color : COLOR )
{
	VertexOut vout;
	
	//vout.PosH = mul( position, gWorldViewProj );
	vout.PosH = mul( float4(position, 1.0f), gWorldViewProj );
  vout.Color = color;
  
  return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
  return pin.Color;
}

