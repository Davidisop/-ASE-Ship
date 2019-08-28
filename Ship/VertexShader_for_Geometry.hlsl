/--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cb0
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
};

struct VS_INPUT
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
};


struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};


VS_OUTPUT VS_NoMatrix(VS_INPUT v)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;
	vOut.p = float4(v.p, 1.0f);// float4(v.p, 1.0f);
	vOut.n = v.n;
	vOut.c = v.c;// g_Color;
	vOut.t = v.t;
	return vOut;
}


