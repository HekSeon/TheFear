
//============================================================================
//
// BAYAR SEMIH
// 
// 
// shader.hlsl
// 
// encoding error!
//============================================================================

// マトリクスバッファ
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

cbuffer DissolveBuffer : register(b8)
{
    float DissolveThreshold;
    float MinY;
    float MaxY;
    int DissolveEnable;
}



//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0,
						  out float4 outLocalPos	: POSITION1)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

    outLocalPos = inPosition;
	
	outDiffuse = inDiffuse;
}



//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PixelShaderPolygon(in float4 inPosition : SV_POSITION,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,
						 in float4 inWorldPos : POSITION0,
						 in float4 inLocalPos : POSITION1,

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}

        if (Light.Enable == 0)
        {
            color = color * Material.Diffuse;
        }
        else
        {
            float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
            float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

            for (int i = 0; i < 5; i++)
            {
                float3 lightDir;
                float light;

                if (Light.Flags[i].y == 1)
                {
                    if (Light.Flags[i].x == 1)
                    {
                        float3 lightDir = normalize(-Light.Direction[i].xyz);
                        float NdotL = max(dot(inNormal.xyz, lightDir), 0.0f);
                        outColor += color * Material.Diffuse * NdotL * Light.Diffuse[i];
                    }
                    else if (Light.Flags[i].x == 2)
                    {
                        lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
                        light = dot(lightDir, inNormal.xyz);

                        tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

                        float distance = length(inWorldPos - Light.Position[i]);

                        float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
                        tempColor *= att;
                    }
                    else
                    {
                        tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
                    }

                    outColor += tempColor;
                }
            }

            color = outColor;
            color.a = inDiffuse.a * Material.Diffuse.a;
        }

	//フォグ
	if (Fog.Enable == 1)
	{
		float z = length(inWorldPos.xyz - Camera.xyz);
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

	//縁取り
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if ((angle < 0.5f)&&(angle > -0.5f))
		if (angle > -0.3f)
		{
			outDiffuse.rb  = 1.0f;
			outDiffuse.g = 0.0f;			
		}
	}
	
    if (DissolveEnable==1)
    {
        float heightFraction = saturate((inLocalPos.y - MinY) / (MaxY - MinY));
        if (heightFraction < DissolveThreshold)
        {
            clip(-1);

        }
    }
}

struct SwordFragmentVertex
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL0;
    float2 texCoord : TEXCOORD0;
    float4 diffuse : COLOR0;
    float4 worldPosition : POSITION0;
    float4 localPosition : POSITION1;
};

[maxvertexcount(3)]
void SwordFragmentGeometry(triangle SwordFragmentVertex vertices[3],
                           inout TriangleStream<SwordFragmentVertex> stream)
{
    float3 center = (vertices[0].localPosition.xyz + vertices[1].localPosition.xyz +
                     vertices[2].localPosition.xyz) / 3.0;
    // A stable seed lets each triangle retrace its motion during reconstruction.
    float3 seed = frac(sin(float3(dot(center, float3(13.1, 8.3, 17.7)),
                                  dot(center, float3(7.4, 15.9, 3.1)),
                                  dot(center, float3(1.3, 19.6, 8.6)))) * 43758.5453);
    float progress = smoothstep(seed.x * 0.32, 0.92 + seed.y * 0.07, DissolveThreshold);
    if (progress >= 1.0)
        return;

    float3 drift = (seed * 2.0 - 1.0) * float3(110.0, 90.0, 110.0);
    drift.y += 48.0;
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        SwordFragmentVertex fragment = vertices[i];
        float3 local = center + (fragment.localPosition.xyz - center) *
            (1.0 - progress) + drift * progress;
        fragment.localPosition = float4(local, 1.0);
        fragment.worldPosition = mul(fragment.localPosition, World);
        fragment.position = mul(mul(fragment.worldPosition, View), Projection);
        stream.Append(fragment);
    }
    stream.RestartStrip();
}

cbuffer SkyRayBuffer : register(b9)
{
    float4 SkyRight;
    float4 SkyUp;
    float4 SkyForward;
};

struct SkyVertexOutput
{
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD0;
};

SkyVertexOutput SkyVertex(uint vertex : SV_VertexID)
{
    float2 corner = float2((vertex << 1) & 2, vertex & 2);
    float2 ndc = corner * float2(2.0, -2.0) + float2(-1.0, 1.0);
    SkyVertexOutput result;
    result.position = float4(ndc, 1.0, 1.0);
    result.direction = SkyForward.xyz + ndc.x * SkyRight.xyz + ndc.y * SkyUp.xyz;
    return result;
}

float4 SkyPixel(SkyVertexOutput input) : SV_Target
{
    float3 direction = normalize(input.direction);
    float longitude = atan2(direction.x, direction.z) * 0.159154943;
    // Mirror the square photo at azimuth boundaries so it has no wrap seam.
    float u = 1.0 - abs(frac(longitude + 0.25) * 2.0 - 1.0);
    float elevation = asin(clamp(direction.y, -1.0, 1.0)) * 0.636619772;
    float v = saturate(0.91 - elevation * 0.91);
    float2 uv = clamp(float2(u, v), 0.002, 0.998);
    return float4(g_Texture.Sample(g_SamplerState, uv).rgb, 1.0);
}
