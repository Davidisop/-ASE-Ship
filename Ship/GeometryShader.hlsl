//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cb0
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
};

struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};


struct GS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};




GS_OUTPUT NormalizeVertex(GS_OUTPUT Vertex)
{
	GS_OUTPUT newvertex = (GS_OUTPUT)0;
	newvertex.p = float4(normalize(Vertex.p.xyz), 1);
	// 정점이 원점을 중심으로 하는 노말 벡터가 된다.
	newvertex.n = normalize(Vertex.n);
	newvertex.c = Vertex.c;
	newvertex.t = Vertex.t;

	float4 vWorld = mul(float4(newvertex.p.xyz, 1.0f), g_matWorld);
	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);
	newvertex.p = vProj;

	return newvertex;
}


void TriAppend(GS_OUTPUT V0, GS_OUTPUT V1, GS_OUTPUT V2, inout TriangleStream<GS_OUTPUT> TriStream)
{
	TriStream.Append(NormalizeVertex(V0));
	TriStream.Append(NormalizeVertex(V1));
	TriStream.Append(NormalizeVertex(V2));
	// 삼각형 단위로 스트립으로 구성토록 한다.		
	TriStream.RestartStrip();
}



[maxvertexcount(200)] // 정점 *3

void GS(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> triStream)
{
	// 페이스의 중점을 구한다.
	GS_OUTPUT vInput[3];

	vInput[0].p = input[0].p;
	vInput[0].n = input[0].n;
	vInput[0].c = input[0].c;
	vInput[0].t = input[0].t;

	vInput[1].p = input[1].p;
	vInput[1].n = input[1].n;
	vInput[1].c = input[1].c;
	vInput[1].t = input[1].t;

	vInput[2].p = input[2].p;
	vInput[2].n = input[2].n;
	vInput[2].c = input[2].c;
	vInput[2].t = input[2].t;

	vInput[0] = NormalizeVertex(vInput[0]);
	vInput[1] = NormalizeVertex(vInput[1]);
	vInput[2] = NormalizeVertex(vInput[2]);



	//

	GS_OUTPUT Center0, Center1, Center2;

	Center0.p = float4((input[0].p.xyz + input[1].p.xyz) / 2.0, 1.0f);
	Center0.n = (input[0].n + input[1].n) / 2.0;
	Center0.c = (input[0].c + input[1].c) / 2.0;
	Center0.t = (input[0].t.xy + input[1].t.xy) / 2.0;

	Center1.p = float4((input[1].p.xyz + input[2].p.xyz) / 2.0, 1.0f);
	Center1.n = (input[1].n + input[2].n) / 2.0;
	Center1.c = (input[1].c + input[2].c) / 2.0;
	Center1.t = (input[1].t.xy + input[2].t.xy) / 2.0;

	Center2.p = float4((input[0].p.xyz + input[2].p.xyz) / 2.0, 1.0f);
	Center2.n = (input[0].n + input[2].n) / 2.0;
	Center2.c = (input[0].c + input[2].c) / 2.0;
	Center2.t = (input[0].t.xy + input[2].t.xy) / 2.0;


	//

	GS_OUTPUT Der0, Der1, Der2;


	GS_OUTPUT Der3, Der4, Der5;


	GS_OUTPUT Der6, Der7, Der8;

	//
	
	Der0.p = float4((vInput[0].p.xyz + Center0.p.xyz) / 2.0, 1.0f);
	Der0.n = (vInput[0].n + Center0.n) / 2.0;
	Der0.c = (vInput[0].c + Center0.c) / 2.0;
	Der0.t = (vInput[0].t.xy + Center0.t.xy) / 2.0;

	Der1.p = float4((Center0.p.xyz + Center2.p.xyz) / 2.0, 1.0f);
	Der1.n = (Center0.n + Center2.n) / 2.0;
	Der1.c = (Center0.c + Center2.c) / 2.0;
	Der1.t = (Center0.t.xy + Center2.t.xy) / 2.0;

	Der2.p = float4((vInput[0].p.xyz + Center2.p.xyz) / 2.0, 1.0f);
	Der2.n = (vInput[0].n + Center2.n) / 2.0;
	Der2.c = (vInput[0].c + Center2.c) / 2.0;
	Der2.t = (vInput[0].t.xy + Center2.t.xy) / 2.0;

	//
	
	
	Der3.p = float4((Center0.p.xyz + vInput[1].p.xyz) / 2.0, 1.0f);
	Der3.n = (Center0.n + vInput[1].n) / 2.0;
	Der3.c = (Center0.c + vInput[1].c) / 2.0;
	Der3.t = (Center0.t.xy + vInput[1].t.xy) / 2.0;

	Der4.p = float4((vInput[1].p.xyz + Center1.p.xyz) / 2.0, 1.0f);
	Der4.n = (vInput[1].n + Center1.n) / 2.0;
	Der4.c = (vInput[1].c + Center1.c) / 2.0;
	Der4.t = (vInput[1].t.xy + Center1.t.xy) / 2.0;

	Der5.p = float4((Center0.p.xyz + Center1.p.xyz) / 2.0, 1.0f);
	Der5.n = (Center0.n + Center1.n) / 2.0;
	Der5.c = (Center0.c + Center1.c) / 2.0;
	Der5.t = (Center0.t.xy + Center1.t.xy) / 2.0;
	
	//

	Der6.p = float4((Center1.p.xyz + vInput[2].p.xyz) / 2.0, 1.0f);
	Der6.n = (Center1.n + vInput[2].n) / 2.0;
	Der6.c = (Center1.c + vInput[2].c) / 2.0;
	Der6.t = (Center1.t.xy + vInput[2].t.xy) / 2.0;

	Der7.p = float4((Center2.p.xyz + vInput[2].p.xyz) / 2.0, 1.0f);
	Der7.n = (Center2.n + vInput[2].n) / 2.0;
	Der7.c = (Center2.c + vInput[2].c) / 2.0;
	Der7.t = (Center2.t.xy + vInput[2].t.xy) / 2.0;

	Der8.p = float4((Center1.p.xyz + Center2.p.xyz) / 2.0, 1.0f);
	Der8.n = (Center1.n + Center2.n) / 2.0;
	Der8.c = (Center1.c + Center2.c) / 2.0;
	Der8.t = (Center1.t.xy + Center2.t.xy) / 2.0;


	//


	TriAppend(vInput[0], Der0, Der2, triStream);
	TriAppend(Der0, Der1, Der2, triStream);
	TriAppend(Der0, Center0, Der1, triStream);
	TriAppend(Der1, Center2, Der2, triStream);

	TriAppend(Center0, Der5, Der1, triStream);
	TriAppend(Der1, Der5, Der8, triStream);
	TriAppend(Der5, Center1, Der8, triStream);
	TriAppend(Der1, Der8, Center2, triStream);

	TriAppend(Center0, Der3, Der5, triStream);
	TriAppend(Der3, Der4, Der5, triStream);
	TriAppend(Der3, vInput[1], Der4, triStream);
	TriAppend(Der5, Der4, Center1, triStream);

	TriAppend(Center1, Der6, Der8, triStream);
	TriAppend(Der6, Der7, Der8, triStream);
	TriAppend(Der6, vInput[2], Der7, triStream);
	TriAppend(Der8, Der7, Center2, triStream);


}

