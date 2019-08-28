
struct PS_INPUT
{
	float4 p : SV_POSITION;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

Texture2D g_txDiffuse  : register (t0);
SamplerState sample0 : register (s0);


float4 PS(PS_INPUT input) : SV_Target
{
	float4 yang;
yang = g_txDiffuse.Sample(sample0, input.t);
return yang * input.c;
}



float4 PSLine(PS_INPUT v) : SV_Target
{
	return v.c;
}