#include "Skybox.hlsli"

struct WorldTransform
{
    float32_t4x4 world;
    float32_t4x4 worldInverseTranspose;
};

struct Camera
{
    float32_t3 worldPosition;
    float32_t4x4 view;
    float32_t4x4 projection;
};

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gWorldTransform.world, mul(gCamera.view, gCamera.projection))).xyww;
    output.texcoord = input.position.xyz;
    return output;
}