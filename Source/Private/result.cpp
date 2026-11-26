#include "result.h"
#include "fade.h"
#include "input.h"
#include "renderer.h"

namespace
{
constexpr int Center = SCREEN_WIDTH / 2;
constexpr COLORREF Ink = RGB(233, 230, 218);
constexpr COLORREF Muted = RGB(163, 173, 160);
constexpr COLORREF Accent = RGB(224, 107, 91);
constexpr RECT RetryButton = {Center - 320, 744, Center - 10, 818};
constexpr RECT MenuButton = {Center + 10, 744, Center + 320, 818};
int finalWave = 0;
int finalScore = 0;

void Fill(HDC dc, const RECT& rect, COLORREF color)
{
    SetDCBrushColor(dc, color);
    FillRect(dc, &rect, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

void Text(HDC dc, RECT rect, const char* label, int size, COLORREF color, bool bold = false)
{
    HFONT font = CreateFontA(-size, 0, 0, 0, bold ? FW_SEMIBOLD : FW_NORMAL,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HGDIOBJ previous = font ? SelectObject(dc, font) : nullptr;
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextA(dc, label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    if (font)
    {
        SelectObject(dc, previous);
        DeleteObject(font);
    }
}

void Ridge(HDC dc, const POINT* points, int count, COLORREF color)
{
    HGDIOBJ pen = SelectObject(dc, GetStockObject(NULL_PEN));
    HGDIOBJ brush = SelectObject(dc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, color);
    Polygon(dc, points, count);
    SelectObject(dc, brush);
    SelectObject(dc, pen);
}

void Backdrop(HDC dc)
{
    constexpr int bandHeight = 24;
    for (int y = 0; y < SCREEN_HEIGHT; y += bandHeight)
    {
        const int shade = y * 18 / SCREEN_HEIGHT;
        Fill(dc, {0, y, SCREEN_WIDTH, y + bandHeight}, RGB(14 - shade / 2, 25 - shade, 23 - shade));
    }

    const POINT farHills[] = {{0, 710}, {230, 621}, {435, 691}, {650, 630},
        {876, 735}, {1120, 700}, {1450, 572}, {1710, 669}, {SCREEN_WIDTH, 606},
        {SCREEN_WIDTH, SCREEN_HEIGHT}, {0, SCREEN_HEIGHT}};
    const POINT nearHills[] = {{0, 817}, {255, 730}, {540, 839}, {890, 875},
        {1222, 822}, {1580, 742}, {SCREEN_WIDTH, 824},
        {SCREEN_WIDTH, SCREEN_HEIGHT}, {0, SCREEN_HEIGHT}};
    Ridge(dc, farHills, _countof(farHills), RGB(14, 21, 18));
    Ridge(dc, nearHills, _countof(nearHills), RGB(9, 14, 13));
}

void DrawLoading(HDC dc)
{
    Backdrop(dc);
    Text(dc, {Center - 360, 390, Center + 360, 502}, "THE FEAR", 88, Ink, true);
    Fill(dc, {Center - 24, 533, Center + 24, 536}, Accent);
    Text(dc, {Center - 360, 561, Center + 360, 620}, "LOADING ARENA...", 24, Muted, true);
}

void Stat(HDC dc, const RECT& rect, const char* label, int value)
{
    Fill(dc, rect, RGB(20, 28, 24));
    Text(dc, {rect.left, rect.top + 10, rect.right, rect.top + 48}, label, 20, Muted, true);
    char number[24];
    sprintf_s(number, "%d", value);
    Text(dc, {rect.left, rect.top + 46, rect.right, rect.bottom - 8}, number, 48, Ink, true);
}
}

void SetResultStats(int wave, int score)
{
    finalWave = wave;
    finalScore = score;
}

void UpdateResult(void)
{
    if (GetFade() != FADE_NONE) return;
    if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START))
        SetFade(FADE_OUT, MODE_GAME);
    else if (GetKeyboardTrigger(DIK_M) || IsButtonTriggered(0, BUTTON_B))
        SetFade(FADE_OUT, MODE_TITLE);
}

void HandleResultClick(HWND window, int x, int y)
{
    if (GetFade() != FADE_NONE) return;
    RECT client;
    if (!GetClientRect(window, &client) || client.right <= 0 || client.bottom <= 0) return;
    const POINT cursor = {MulDiv(x, SCREEN_WIDTH, client.right), MulDiv(y, SCREEN_HEIGHT, client.bottom)};
    if (PtInRect(&RetryButton, cursor))
        SetFade(FADE_OUT, MODE_GAME);
    else if (PtInRect(&MenuButton, cursor))
        SetFade(FADE_OUT, MODE_TITLE);
}

void DrawResult(HDC dc)
{
    Backdrop(dc);
    Fill(dc, {Center - 24, 272, Center + 24, 275}, Accent);
    Text(dc, {Center - 420, 294, Center + 420, 429}, "YOU DIED", 112, Ink, true);
    Stat(dc, {Center - 320, 527, Center - 10, 670}, "WAVE REACHED", finalWave);
    Stat(dc, {Center + 10, 527, Center + 320, 670}, "SCORE", finalScore);

    Fill(dc, RetryButton, Accent);
    Fill(dc, MenuButton, RGB(28, 40, 34));
    Text(dc, RetryButton, "RETRY  [ENTER]", 25, RGB(18, 20, 17), true);
    Text(dc, MenuButton, "MAIN MENU  [M]", 25, Ink, true);
}

void ShowLoadingScreen(void)
{
    Clear();
    DrawBackBufferOverlay(DrawLoading);
    Present();
}
