#include "sky.h"
#include "camera.h"
#include "renderer.h"

#include <wrl/client.h>

namespace
{
using Microsoft::WRL::ComPtr;
ComPtr<ID3D11ShaderResourceView> skyTexture;
ComPtr<ID3D11VertexShader> skyVertex;
ComPtr<ID3D11PixelShader> skyPixel;
ComPtr<ID3D11Buffer> skyRayBuffer;

struct SkyRayData
{
    XMFLOAT4 right;
    XMFLOAT4 up;
    XMFLOAT4 forward;
};
}

void InitSky(void)
{
    if (FAILED(LoadGameTexture("data/TEXTURE/sky.jpg", skyTexture.GetAddressOf()))) return;

    ComPtr<ID3DBlob> vertexCode, pixelCode;
    constexpr UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    if (FAILED(CompileGameShader("SkyVertex", "vs_4_0", flags, vertexCode.GetAddressOf())) ||
        FAILED(CompileGameShader("SkyPixel", "ps_4_0", flags, pixelCode.GetAddressOf()))) return;

    auto device = GetDevice();
    if (FAILED(device->CreateVertexShader(vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(),
            nullptr, skyVertex.GetAddressOf())) ||
        FAILED(device->CreatePixelShader(pixelCode->GetBufferPointer(), pixelCode->GetBufferSize(),
            nullptr, skyPixel.GetAddressOf()))) return;

    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = sizeof(SkyRayData);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&desc, nullptr, skyRayBuffer.GetAddressOf());
}

void UninitSky(void)
{
    skyRayBuffer.Reset();
    skyPixel.Reset();
    skyVertex.Reset();
    skyTexture.Reset();
}

void DrawSky(void)
{
    if (!skyRayBuffer) return;

    const CAMERA& camera = *GetCamera();
    const XMMATRIX inverseView = XMLoadFloat4x4(&camera.mtxInvView);
    SkyRayData rays;
    XMStoreFloat4(&rays.right, inverseView.r[0] / camera.mtxProjection._11);
    XMStoreFloat4(&rays.up, inverseView.r[1] / camera.mtxProjection._22);
    XMStoreFloat4(&rays.forward, inverseView.r[2]);

    auto context = GetDeviceContext();
    context->UpdateSubresource(skyRayBuffer.Get(), 0, nullptr, &rays, 0, 0);
    context->VSSetConstantBuffers(9, 1, skyRayBuffer.GetAddressOf());
    context->PSSetShaderResources(0, 1, skyTexture.GetAddressOf());
    context->IASetInputLayout(nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(skyVertex.Get(), nullptr, 0);
    context->PSSetShader(skyPixel.Get(), nullptr, 0);

    SetDepthEnable(FALSE);
    SetCullingMode(CULL_MODE_NONE);
    context->Draw(3, 0);
    SetCullingMode(CULL_MODE_BACK);
    SetDepthEnable(TRUE);
    SetSceneShaders();
}
