struct BufType
{
    int i;
    float f;
};

struct CSInput
{
    uint3 groupThread : SV_GroupThreadID;
    uint3 group : SV_GroupID;
    uint groupIndex : SV_GroupIndex;
    uint3 dispatch : SV_DispatchThreadID;
};

StructuredBuffer<BufType> input : register(t0);
RWStructuredBuffer<BufType> data : register(u0);

[numthreads(1, 1, 1)]
void main( const CSInput GSInput )
{
    int index = GSInput.dispatch.x;

    data[index].i = input[index].i;
    data[index].f = input[index].f;
}