struct Input {
	float3 pos : POSITION0;
};

struct Output {
	float4 pos : SV_POSITION;
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
    
    float4 pos = float4(input.pos, 1);
    pos = mul(pos, Model);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    output.pos = pos;
    
	return output;
}