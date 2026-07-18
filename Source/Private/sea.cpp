#include "main.h"
#include "renderer.h"
#include "sea.h"

#define MAX_SEA_TILE (64)

struct SEA_TILE
{
	ID3D11Buffer* VertexBuffer;
	XMFLOAT3		pos;
	VERTEX_3D* vertex;
	BOOL			use;
};

static SEA_TILE				g_SeaTile[MAX_SEA_TILE];
static int					g_nNumSeaTile = 0;
static ID3D11Buffer* g_SeaIndexBuffer = NULL;
static ID3D11ShaderResourceView* g_SeaTexture = NULL;

static int		g_nNumBlockX, g_nNumBlockZ;
static float	g_fBlockSizeX, g_fBlockSizeZ;
static int		g_nNumVertex, g_nNumIndex;

static float	g_Time = 0.0f;
static float	g_wave_frequency = 1.0f;
static float	g_wave_correction = 0.02f;
static float	g_wave_amplitude = 15.0f;

HRESULT InitSea(XMFLOAT3 centerPos, int nTilesX, int nTilesZ,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	g_nNumBlockX = nNumBlockX;
	g_nNumBlockZ = nNumBlockZ;
	g_fBlockSizeX = fBlockSizeX;
	g_fBlockSizeZ = fBlockSizeZ;
	g_nNumVertex = (nNumBlockX + 1) * (nNumBlockZ + 1);
	g_nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	D3DX11CreateShaderResourceViewFromFile(GetDevice(),
		"data/TEXTURE/sea.jpg",
		NULL, NULL, &g_SeaTexture, NULL);

	float tileWidth = nNumBlockX * fBlockSizeX;
	float tileDepth = nNumBlockZ * fBlockSizeZ;

	g_nNumSeaTile = 0;
	for (int tz = 0; tz < nTilesZ; tz++)
	{
		for (int tx = 0; tx < nTilesX; tx++)
		{
			if (g_nNumSeaTile >= MAX_SEA_TILE) break;

			SEA_TILE* tile = &g_SeaTile[g_nNumSeaTile];
			tile->pos = XMFLOAT3(
				centerPos.x + (tx - nTilesX / 2.0f) * tileWidth,
				centerPos.y,
				centerPos.z + (tz - nTilesZ / 2.0f) * tileDepth);

			tile->vertex = new VERTEX_3D[g_nNumVertex];

			for (int z = 0; z <= nNumBlockZ; z++)
			{
				for (int x = 0; x <= nNumBlockX; x++)
				{
					int idx = z * (nNumBlockX + 1) + x;
					tile->vertex[idx].Position.x = -(nNumBlockX / 2.0f) * fBlockSizeX + x * fBlockSizeX;
					tile->vertex[idx].Position.y = 0.0f;
					tile->vertex[idx].Position.z = (nNumBlockZ / 2.0f) * fBlockSizeZ - z * fBlockSizeZ;
					tile->vertex[idx].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
					tile->vertex[idx].Diffuse = XMFLOAT4(0.6f, 0.8f, 0.9f, 0.75f);  // yarı saydam mavi
					tile->vertex[idx].TexCoord = XMFLOAT2((float)x, (float)z);
				}
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertex;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			GetDevice()->CreateBuffer(&bd, NULL, &tile->VertexBuffer);

			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(tile->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			memcpy(msr.pData, tile->vertex, sizeof(VERTEX_3D) * g_nNumVertex);
			GetDeviceContext()->Unmap(tile->VertexBuffer, 0);

			tile->use = TRUE;
			g_nNumSeaTile++;
		}
	}

	// Tüm tile'lar aynı topolojiyi paylaştığı için index buffer TEK sefer oluşturuluyor
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumIndex;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_SeaIndexBuffer);

	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_SeaIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	unsigned short* pIdx = (unsigned short*)msr.pData;
	int nCntIdx = 0;
	for (int z = 0; z < nNumBlockZ; z++)
	{
		if (z > 0) { pIdx[nCntIdx++] = (z + 1) * (nNumBlockX + 1); }
		for (int x = 0; x <= nNumBlockX; x++)
		{
			pIdx[nCntIdx++] = (z + 1) * (nNumBlockX + 1) + x;
			pIdx[nCntIdx++] = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockZ - 1) { pIdx[nCntIdx++] = z * (nNumBlockX + 1) + nNumBlockX; }
	}
	GetDeviceContext()->Unmap(g_SeaIndexBuffer, 0);

	g_Time = 0.0f;
	return S_OK;
}

void UninitSea(void)
{
	for (int i = 0; i < g_nNumSeaTile; i++)
	{
		if (g_SeaTile[i].VertexBuffer) { g_SeaTile[i].VertexBuffer->Release(); g_SeaTile[i].VertexBuffer = NULL; }
		if (g_SeaTile[i].vertex) { delete[] g_SeaTile[i].vertex; g_SeaTile[i].vertex = NULL; }
		g_SeaTile[i].use = FALSE;
	}
	g_nNumSeaTile = 0;

	if (g_SeaIndexBuffer) { g_SeaIndexBuffer->Release(); g_SeaIndexBuffer = NULL; }
	if (g_SeaTexture) { g_SeaTexture->Release(); g_SeaTexture = NULL; }
}

void UpdateSea(void)
{
	float dt = 0.03f;

	for (int i = 0; i < g_nNumSeaTile; i++)
	{
		SEA_TILE* tile = &g_SeaTile[i];
		if (!tile->use) continue;

		for (int z = 0; z <= g_nNumBlockZ; z++)
		{
			for (int x = 0; x <= g_nNumBlockX; x++)
			{
				int idx = z * (g_nNumBlockX + 1) + x;
				float worldX = tile->vertex[idx].Position.x + tile->pos.x;
				float worldZ = tile->vertex[idx].Position.z + tile->pos.z;
				float len = sqrtf(worldX * worldX + worldZ * worldZ);

				tile->vertex[idx].Position.y =
					sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
			}
		}

		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(tile->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, tile->vertex, sizeof(VERTEX_3D) * g_nNumVertex);
		GetDeviceContext()->Unmap(tile->VertexBuffer, 0);
	}

	g_Time += dt;
}

void DrawSea(void)
{
	SetBlendState(BLEND_MODE_ALPHABLEND);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_SeaTexture);
	GetDeviceContext()->IASetIndexBuffer(g_SeaIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	for (int i = 0; i < g_nNumSeaTile; i++)
	{
		SEA_TILE* tile = &g_SeaTile[i];
		if (!tile->use) continue;

		GetDeviceContext()->IASetVertexBuffers(0, 1, &tile->VertexBuffer, &stride, &offset);

		XMMATRIX mtxWorld = XMMatrixTranslation(tile->pos.x, tile->pos.y, tile->pos.z);
		SetWorldMatrix(&mtxWorld);

		GetDeviceContext()->DrawIndexed(g_nNumIndex, 0, 0);
	}
}