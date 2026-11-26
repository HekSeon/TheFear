///////////////////////////////////
// 
// debugproc.cpp
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#include <algorithm>
#include <stdio.h>
#include "debugproc.h"
#include "renderer.h"
#include "enemy.h"
#include "meshfield.h"
#include "player.h"

///////////////////////////////////
// makro area
///////////////////////////////////

///////////////////////////////////
// protota area
///////////////////////////////////

///////////////////////////////////
// global area
///////////////////////////////////
char		g_aStrDebug[1024] = { "\0" };


///////////////////////////////////
// debug area
///////////////////////////////////
void DrawDebugProc(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	DebugTextOut(g_aStrDebug, 0, 0);

	// èÓïÒÉNÉäÉA
	memset(g_aStrDebug, 0, sizeof g_aStrDebug);
}

///////////////////////////////////
// debug area
///////////////////////////////////
void PrintDebugProc(char* fmt, ...)
{
#if 0
	long* pParam;
	static char aBuf[256];

	pParam = (long*)&fmt;
	sprintf(aBuf, fmt, pParam[1], pParam[2], pParam[3], pParam[4],
		pParam[5], pParam[6], pParam[7], pParam[8],
		pParam[9], pParam[10], pParam[11], pParam[12]);
#else
	va_list list;		
	char* pCur;
	char aBuf[256] = { "\0" };
	char aWk[32];

	va_start(list, fmt);

	pCur = fmt;
	for (; *pCur; ++pCur)
	{
		if (*pCur != '%')
		{
			sprintf_s(aWk, "%c", *pCur);
		}
		else
		{
			pCur++;

			switch (*pCur)
			{
			case 'd':
				sprintf_s(aWk, "%d", va_arg(list, int));
				break;

			case 'f':
				sprintf_s(aWk, "%.2f", va_arg(list, double));
				break;

			case 's':
				sprintf_s(aWk, "%s", va_arg(list, char*));
				break;

			case 'c':
				sprintf_s(aWk, "%c", va_arg(list, char));
				break;

			default:
				sprintf_s(aWk, "%c", *pCur);
				break;
			}
		}
		strcat_s(aBuf, aWk);
	}

	va_end(list);

	if ((strlen(g_aStrDebug) + strlen(aBuf)) < ((sizeof g_aStrDebug) - 1))
	{
		strcat_s(g_aStrDebug, aBuf);
	}
#endif
}

// Gameplay diagnostics use the same collision centers and radii as simulation.
namespace
{
constexpr int PanelX = SCREEN_WIDTH - 280;
constexpr int PanelY = 20;
constexpr int ButtonTop = PanelY + 42;
constexpr int ButtonHeight = 32;
constexpr int ButtonStep = 38;
constexpr int RingSegments = 24;
constexpr unsigned MaxDebugVertices = MAX_ENEMY * (4 * RingSegments * 3 + 96) + 256;

bool debugOpen = false;
bool showHitboxes = true;
bool showChase = true;
bool showGround = false;
ID3D11Buffer* debugBuffer = nullptr;

RECT ButtonRect(int index)
{
    const int top = ButtonTop + index * ButtonStep;
    return {PanelX + 12, top, SCREEN_WIDTH - 32, top + ButtonHeight};
}

void Fill(HDC dc, const RECT& rect, COLORREF color)
{
    HBRUSH brush = static_cast<HBRUSH>(GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, color);
    FillRect(dc, &rect, brush);
}

void Label(HDC dc, RECT rect, const char* text, UINT align = DT_LEFT)
{
    DrawTextA(dc, text, -1, &rect, align | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}
}

bool IsGameDebugOpen(void) { return debugOpen; }
void SetGameDebugOpen(bool open) { debugOpen = open; }

void HandleGameDebugClick(HWND window, int x, int y)
{
    if (!debugOpen) return;
    RECT client;
    if (!GetClientRect(window, &client) || client.right <= 0 || client.bottom <= 0) return;

    const POINT cursor = {MulDiv(x, SCREEN_WIDTH, client.right), MulDiv(y, SCREEN_HEIGHT, client.bottom)};
    bool* toggles[] = {&showHitboxes, &showChase, &showGround};
    for (int i = 0; i < _countof(toggles); ++i)
    {
        const RECT rect = ButtonRect(i);
        if (PtInRect(&rect, cursor))
            *toggles[i] = !*toggles[i];
    }
}

void DrawGameOverlay(HDC dc)
{
    const PLAYER& player = *GetPlayer();
    const int hp = static_cast<int>(ceilf(player.health));
    const int remaining = GetAliveEnemyCount();
    char value[64];

    HFONT font = CreateFontA(-21, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HGDIOBJ previousFont = SelectObject(dc, font ? font : GetStockObject(DEFAULT_GUI_FONT));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(245, 247, 250));

    const COLORREF panel = RGB(22, 27, 32);
    Fill(dc, {20, 20, 340, 92}, panel);
    Label(dc, {34, 26, 324, 53}, "HEALTH");
    sprintf_s(value, "%d / 100", hp);
    Label(dc, {34, 26, 324, 53}, value, DT_RIGHT);
    Fill(dc, {34, 56, 326, 75}, RGB(64, 53, 55));
    Fill(dc, {34, 56, 34 + MulDiv(292, hp, 100), 75}, RGB(249, 107, 100));

    const int center = SCREEN_WIDTH / 2;
    Fill(dc, {center - 122, 20, center + 122, 92}, panel);
    sprintf_s(value, "WAVE %02d", GetEnemyWave());
    Label(dc, {center - 122, 24, center + 122, 53}, value, DT_CENTER);
    if (remaining > 0)
        sprintf_s(value, "%d %s LEFT", remaining, remaining == 1 ? "ENEMY" : "ENEMIES");
    else
        sprintf_s(value, "NEXT WAVE IN %.1fs", GetWaveRestFrames() / 60.0f);
    Label(dc, {center - 122, 52, center + 122, 83}, value, DT_CENTER);

    Fill(dc, {SCREEN_WIDTH - 174, SCREEN_HEIGHT - 63, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20}, panel);
    Label(dc, {SCREEN_WIDTH - 174, SCREEN_HEIGHT - 63, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20},
        "LMB   ATTACK", DT_CENTER);

    if (debugOpen)
    {
        Fill(dc, {PanelX, PanelY, SCREEN_WIDTH - 20, ButtonTop + 3 * ButtonStep + 4}, panel);
        Label(dc, {PanelX + 12, PanelY + 4, SCREEN_WIDTH - 32, ButtonTop}, "DEBUG   F2");
        const char* labels[] = {"HITBOXES", "CHASE LINES", "GROUND"};
        const bool states[] = {showHitboxes, showChase, showGround};
        for (int i = 0; i < _countof(labels); ++i)
        {
            const RECT button = ButtonRect(i);
            Fill(dc, button, RGB(49, 58, 67));
            Fill(dc, {button.left + 8, button.top + 8, button.left + 29, button.bottom - 8},
                states[i] ? RGB(62, 172, 109) : RGB(87, 93, 100));
            RECT label = button;
            label.left += 39;
            Label(dc, label, labels[i]);
        }
    }
    SelectObject(dc, previousFont);
    if (font) DeleteObject(font);
}

void ReleaseGameDebugGeometry(void)
{
    if (!debugBuffer) return;
    debugBuffer->Release();
    debugBuffer = nullptr;
}

void DrawGameDebugGeometry(void)
{
    if (!debugOpen || (!showHitboxes && !showChase && !showGround)) return;
    ID3D11DeviceContext* context = GetDeviceContext();
    if (!debugBuffer)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(VERTEX_3D) * MaxDebugVertices;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(GetDevice()->CreateBuffer(&desc, nullptr, &debugBuffer))) return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(debugBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    VERTEX_3D* vertices = static_cast<VERTEX_3D*>(mapped.pData);
    unsigned count = 0;
    auto line = [&](XMFLOAT3 a, XMFLOAT3 b, XMFLOAT4 color)
    {
        if (count + 2 > MaxDebugVertices) return;
        vertices[count++] = {a, {0, 1, 0}, color, {}};
        vertices[count++] = {b, {0, 1, 0}, color, {}};
    };
    auto sphere = [&](XMFLOAT3 center, float radius, XMFLOAT4 color)
    {
        for (int i = 0; i < RingSegments; ++i)
        {
            const float a = XM_2PI * i / RingSegments;
            const float b = XM_2PI * (i + 1) / RingSegments;
            const float x0 = radius * cosf(a), z0 = radius * sinf(a);
            const float x1 = radius * cosf(b), z1 = radius * sinf(b);
            line({center.x + x0, center.y, center.z + z0}, {center.x + x1, center.y, center.z + z1}, color);
            line({center.x + x0, center.y + z0, center.z}, {center.x + x1, center.y + z1, center.z}, color);
            line({center.x, center.y + x0, center.z + z0}, {center.x, center.y + x1, center.z + z1}, color);
        }
    };
    auto groundProbe = [&](XMFLOAT3 position)
    {
        XMFLOAT3 hit, normal;
        if (!RayHitField(position, &hit, &normal)) return;
        const XMFLOAT4 color = {0.3f, 1.0f, 0.5f, 1.0f};
        line(position, hit, color);
        line(hit, {hit.x + normal.x * 12, hit.y + normal.y * 12, hit.z + normal.z * 12}, color);
    };

    const PLAYER& player = *GetPlayer();
    if (showHitboxes)
        sphere(GetPlayerAttackCenter(), GetPlayerAttackRadius(),
            IsPlayerAttackActive() ? XMFLOAT4{0.2f, 1, 1, 1} : XMFLOAT4{0.25f, 0.45f, 0.85f, 1});
    if (showGround) groundProbe(player.pos);

    const ENEMY* enemies = GetEnemy();
    for (int i = 0; i < MAX_ENEMY; ++i)
    {
        const ENEMY& enemy = enemies[i];
        if (!enemy.use) continue;
        if (showHitboxes)
        {
            sphere(enemy.pos, EnemyBodyRadius, {0.4f, 0.7f, 1.0f, 1.0f});
            sphere(GetEnemyAttackCenter(enemy), EnemyAttackRadius,
                IsEnemyAttackActive(enemy) ? XMFLOAT4{1, 0.16f, 0.1f, 1} : XMFLOAT4{1, 0.5f, 0.15f, 1});
        }
        if (showGround) groundProbe(enemy.pos);
        if (showChase)
        {
            const XMVECTOR start = XMLoadFloat3(&enemy.pos);
            const XMVECTOR end = XMLoadFloat3(&player.pos);
            const int segments = std::min(48, std::max(1, static_cast<int>(XMVectorGetX(XMVector3Length(end - start)) / 8.0f)));
            for (int segment = 0; segment < segments; ++segment)
            {
                XMFLOAT3 a, b;
                XMStoreFloat3(&a, XMVectorLerp(start, end, static_cast<float>(segment) / segments));
                XMStoreFloat3(&b, XMVectorLerp(start, end, (segment + 0.55f) / segments));
                line(a, b, {1.0f, 0.78f, 0.2f, 1.0f});
            }
        }
    }
    context->Unmap(debugBuffer, 0);
    if (!count) return;

    XMMATRIX world = XMMatrixIdentity();
    SetWorldMatrix(&world);
    SetDepthEnable(FALSE);
    SetLightEnable(FALSE);
    SetFogEnable(FALSE);
    MATERIAL material = {};
    material.Diffuse = {1, 1, 1, 1};
    material.noTexSampling = 1;
    SetMaterial(material);

    const UINT stride = sizeof(VERTEX_3D);
    const UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &debugBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->Draw(count, 0);
    SetLightEnable(TRUE);
    SetDepthEnable(TRUE);
}
