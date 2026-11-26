#include "title.h"

#include "fade.h"
#include "input.h"
#include "renderer.h"
#include "sprite.h"
#include "sound.h"

namespace
{
ID3D11Buffer* vertices = nullptr;
ID3D11ShaderResourceView* background = nullptr;
int selected = 0;
bool settingsOpen = false;

constexpr RECT Buttons[] = {{76, 544, 634, 645}, {76, 664, 634, 765}, {76, 784, 634, 885}};
constexpr RECT VolumeBar = {148, 682, 562, 706};
constexpr RECT VolumeHit = {148, 655, 563, 730};
constexpr RECT LowerButton = {76, 655, 132, 730};
constexpr RECT RaiseButton = {578, 655, 634, 730};
constexpr RECT BackButton = {76, 784, 634, 885};
constexpr COLORREF Ivory = RGB(247, 235, 204);

void Fill(HDC dc, const RECT& rect, COLORREF color)
{
    SetDCBrushColor(dc, color);
    FillRect(dc, &rect, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

void Label(HDC dc, RECT rect, const wchar_t* label, int height, int weight,
           UINT alignment, const wchar_t* face = L"Segoe UI")
{
    HFONT font = CreateFontW(-height, 0, 0, 0, weight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_SWISS, face);
    HGDIOBJ old = font ? SelectObject(dc, font) : nullptr;
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, Ivory);
    DrawTextW(dc, label, -1, &rect, alignment | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    if (font)
    {
        SelectObject(dc, old);
        DeleteObject(font);
    }
}

void DrawSettings(HDC dc)
{
    Label(dc, {76, 166, 744, 461}, L"SETTINGS", 140, FW_HEAVY, DT_LEFT, L"Bahnschrift Condensed");
    Fill(dc, {76, 544, 634, 746}, RGB(24, 23, 22));
    Label(dc, {98, 560, 445, 641}, L"MASTER VOLUME", 42, FW_SEMIBOLD, DT_LEFT, L"Bahnschrift Condensed");
    wchar_t value[8];
    swprintf_s(value, L"%d%%", GetMasterVolumePercent());
    Label(dc, {450, 560, 612, 641}, value, 40, FW_SEMIBOLD, DT_RIGHT);

    Fill(dc, VolumeBar, RGB(70, 63, 57));
    const int thumb = VolumeBar.left + MulDiv(VolumeBar.right - VolumeBar.left, GetMasterVolumePercent(), 100);
    Fill(dc, {VolumeBar.left, VolumeBar.top, thumb, VolumeBar.bottom}, RGB(166, 42, 38));
    Fill(dc, {thumb - 3, VolumeBar.top - 5, thumb + 3, VolumeBar.bottom + 5}, Ivory);
    Fill(dc, LowerButton, RGB(80, 31, 28));
    Fill(dc, RaiseButton, RGB(80, 31, 28));
    // Draw the marks geometrically so font metrics cannot shift their centers.
    constexpr LONG markHalfWidth = 11;
    constexpr LONG markHalfStroke = 2;
    const LONG centerY = (LowerButton.top + LowerButton.bottom) / 2;
    const LONG lowerX = (LowerButton.left + LowerButton.right) / 2;
    const LONG raiseX = (RaiseButton.left + RaiseButton.right) / 2;
    Fill(dc, {lowerX - markHalfWidth, centerY - markHalfStroke,
        lowerX + markHalfWidth, centerY + markHalfStroke}, Ivory);
    Fill(dc, {raiseX - markHalfWidth, centerY - markHalfStroke,
        raiseX + markHalfWidth, centerY + markHalfStroke}, Ivory);
    Fill(dc, {raiseX - markHalfStroke, centerY - markHalfWidth,
        raiseX + markHalfStroke, centerY + markHalfWidth}, Ivory);

    Fill(dc, BackButton, RGB(105, 25, 23));
    Label(dc, BackButton, L"BACK  [ENTER]", 45, FW_SEMIBOLD, DT_CENTER, L"Bahnschrift Condensed");
    Label(dc, {76, 916, 700, 982}, L"\u2190 \u2192  Adjust volume", 29, FW_NORMAL, DT_LEFT);
}

void DrawMenu(HDC dc)
{
    Label(dc, {76, 166, 744, 461}, L"THE FEAR", 166, FW_HEAVY, DT_LEFT, L"Bahnschrift Condensed");

    const wchar_t* labels[] = {L"START", L"SETTINGS", L"QUIT"};
    for (int i = 0; i < _countof(Buttons); ++i)
    {
        const RECT button = Buttons[i];
        const bool active = i == selected;
        Fill(dc, button, active ? RGB(142, 42, 40) : RGB(99, 90, 79));
        Fill(dc, {button.left + 3, button.top + 3, button.right - 3, button.bottom - 3},
            active ? RGB(105, 25, 23) : RGB(24, 23, 22));
        Fill(dc, {button.left, button.top, button.left + 9, button.bottom},
            active ? RGB(232, 43, 43) : RGB(99, 90, 79));
        Label(dc, {button.left + 50, button.top, button.right - 90, button.bottom},
            labels[i], 52, FW_SEMIBOLD, DT_LEFT, L"Bahnschrift Condensed");
        if (active)
            Label(dc, {button.right - 100, button.top, button.right - 20, button.bottom},
                L"\u2192", 53, FW_NORMAL, DT_CENTER);
    }
    Label(dc, {76, 916, 650, 982}, L"\u2191 \u2193  Select       ENTER  Confirm", 29, FW_NORMAL, DT_LEFT);
}

void ActivateSelection()
{
    if (selected == 0)
        SetFade(FADE_OUT, MODE_GAME);
    else if (selected == 1)
        settingsOpen = true;
    else
        PostQuitMessage(0);
}
}

HRESULT InitTitle(void)
{
    selected = 0;
    settingsOpen = false;
    HRESULT result = LoadGameTexture("data/TEXTURE/bg1.jpg", &background);
    if (FAILED(result)) return result;

    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(VERTEX_3D) * 4;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    result = GetDevice()->CreateBuffer(&desc, nullptr, &vertices);
    if (FAILED(result)) UninitTitle();
    return result;
}

void UninitTitle(void)
{
    if (vertices) vertices->Release();
    if (background) background->Release();
    vertices = nullptr;
    background = nullptr;
}

void UpdateTitle(void)
{
    if (GetFade() != FADE_NONE) return;
    if (settingsOpen)
    {
        if (GetKeyboardTrigger(DIK_LEFT))
            SetMasterVolumePercent(GetMasterVolumePercent() - 5);
        else if (GetKeyboardTrigger(DIK_RIGHT))
            SetMasterVolumePercent(GetMasterVolumePercent() + 5);
        if (GetKeyboardTrigger(DIK_RETURN))
            settingsOpen = false;
        return;
    }

    constexpr int count = _countof(Buttons);
    if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S))
        selected = (selected + 1) % count;
    else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W))
        selected = (selected + count - 1) % count;
    if (GetKeyboardTrigger(DIK_RETURN))
        ActivateSelection();
}

void HandleTitleClick(HWND window, int x, int y)
{
    if (GetFade() != FADE_NONE) return;
    RECT client;
    if (!GetClientRect(window, &client) || client.right <= 0 || client.bottom <= 0) return;
    const POINT point = {MulDiv(x, SCREEN_WIDTH, client.right), MulDiv(y, SCREEN_HEIGHT, client.bottom)};
    if (settingsOpen)
    {
        if (PtInRect(&BackButton, point))
            settingsOpen = false;
        else if (PtInRect(&LowerButton, point))
            SetMasterVolumePercent(GetMasterVolumePercent() - 5);
        else if (PtInRect(&RaiseButton, point))
            SetMasterVolumePercent(GetMasterVolumePercent() + 5);
        else if (PtInRect(&VolumeHit, point))
            SetMasterVolumePercent(MulDiv(point.x - VolumeBar.left, 100, VolumeBar.right - VolumeBar.left));
        return;
    }

    for (int i = 0; i < _countof(Buttons); ++i)
    {
        if (!PtInRect(&Buttons[i], point)) continue;
        selected = i;
        ActivateSelection();
        return;
    }
}

void DrawTitle(void)
{
    if (vertices && background)
    {
        auto context = GetDeviceContext();
        const UINT stride = sizeof(VERTEX_3D), offset = 0;
        context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        SetWorldViewProjection2D();

        MATERIAL material{};
        material.Diffuse = {1, 1, 1, 1};
        SetMaterial(material);
        context->PSSetShaderResources(0, 1, &background);
        SetSpriteLeftTop(vertices, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 1);
        context->Draw(4, 0);

        D3D11_MAPPED_SUBRESOURCE mapping;
        if (SUCCEEDED(context->Map(vertices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapping)))
        {
            auto quad = static_cast<VERTEX_3D*>(mapping.pData);
            XMFLOAT3 normal = {0, 0, -1};
            XMFLOAT4 dark = {0, 0, 0, 0.76f};
            XMFLOAT4 clear = {0, 0, 0, 0};
            quad[0] = {{0, 0, 0}, normal, dark, {}};
            quad[1] = {{900, 0, 0}, normal, clear, {}};
            quad[2] = {{0, SCREEN_HEIGHT, 0}, normal, dark, {}};
            quad[3] = {{900, SCREEN_HEIGHT, 0}, normal, clear, {}};
            context->Unmap(vertices, 0);
            material.noTexSampling = 1;
            SetMaterial(material);
            context->Draw(4, 0);
        }
    }
    DrawBackBufferOverlay(settingsOpen ? DrawSettings : DrawMenu);
}
