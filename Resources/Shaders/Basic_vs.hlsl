struct Input
{
    float4 pos : POSITION0;
    float2 uv : TEXCOORD0;
};

struct Output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// b0 représente le slot "buffer 0"
cbuffer CameraData : register(b0)
{
    Matrix View;
    Matrix Projection;
};

// b1 représente le slot "buffer 1"
cbuffer ModelData : register(b1)
{
    Matrix Model;
};

Output main(Input input)
{
	Output output = (Output)0;
    
    float4 pos = input.pos;
    pos = mul(pos, Model);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    
    output.pos = pos;
    output.uv = input.uv;
	return output;
}