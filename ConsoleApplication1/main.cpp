#include <iostream>
#include <d3d11.h>
#include <vector>
#include <fstream>
#include <d3dcompiler.h>
#include <Windows.h>
#include "FPSTimer.h"

#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__);

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

const UINT numElement = 1024;

struct BufType
{
	int s32;
	float f32;
};

BufType g_buf[numElement];

HRESULT CreateDevice(ID3D11Device** device, ID3D11DeviceContext** context)
{
	UINT createDeviceFlag = 0;

	const D3D_FEATURE_LEVEL flvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

	auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlag, flvl, sizeof(flvl) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, device, nullptr, context);

	return hr;
}

HRESULT CompileShaderFromFile(wchar_t* fileName, char* entryFunc, ID3DBlob** blob)
{

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	auto hr = D3DCompileFromFile(fileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryFunc, "cs_5_0", dwShaderFlags, 0, blob, nullptr);

	return hr;
}

HRESULT CreateComputeSahder(ID3D11Device* device, ID3D11ComputeShader** computeShader, ID3DBlob* blob)
{
	auto hr = device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, computeShader);

	return hr;
}

HRESULT CreateStructuredBuffer(ID3D11Device* device,ID3D11Buffer** buffer,UINT size,UINT numElement,void* data)
{
	*buffer = nullptr;

	//BufferÇÃçÏê¨
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = numElement * size;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = size;

	if (data)
	{
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = data;

		auto hr = device->CreateBuffer(&bufferDesc, &subResource, buffer);
		return hr;
	}

	auto hr = device->CreateBuffer(&bufferDesc, nullptr, buffer);
	return hr;
}

HRESULT CreateBufferShaderResourceView(ID3D11Device* device,ID3D11Buffer* buffer,ID3D11ShaderResourceView** resourceView)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	buffer->GetDesc(&bufferDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	ZeroMemory(&resourceViewDesc, sizeof(resourceViewDesc));

	resourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	resourceViewDesc.BufferEx.FirstElement = 0;
	resourceViewDesc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;
	
	auto hr = device->CreateShaderResourceView(buffer, &resourceViewDesc, resourceView);

	return hr;
}

HRESULT CraeteBufferUnorderedAccessView(ID3D11Device* device,ID3D11Buffer* buffer,ID3D11UnorderedAccessView** accessView)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	buffer->GetDesc(&bufferDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC accessViewDesc;
	ZeroMemory(&accessViewDesc, sizeof(accessViewDesc));
	accessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	accessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	accessViewDesc.Buffer.FirstElement = 0;
	accessViewDesc.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	auto hr = device->CreateUnorderedAccessView(buffer, &accessViewDesc, accessView);

	return hr;
}

void RunComputeShader(ID3D11DeviceContext* context, ID3D11ComputeShader* computeShader, UINT numViews, ID3D11ShaderResourceView** resourceViews, ID3D11UnorderedAccessView* accessView, UINT x, UINT y, UINT z)
{
	//Compute ShaderÇëñÇÁÇπÇÈ
	context->CSSetShader(computeShader, nullptr, 0);
	context->CSSetShaderResources(0, numViews,resourceViews);
	context->CSSetUnorderedAccessViews(0, 1, &accessView, nullptr);

	context->Dispatch(x, y, z);

	context->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* ppUAViewnullptr[1] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, ppUAViewnullptr, nullptr);

	ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
	context->CSSetShaderResources(0, 2, ppSRVnullptr);

	ID3D11Buffer* ppCBnullptr[1] = { nullptr };
	context->CSSetConstantBuffers(0, 1, ppCBnullptr);
}

int main()
{
	FPSTimer timer(60);

	LARGE_INTEGER freq;

	QueryPerformanceCounter(&freq);

	//ÉÅÉÇÉäÉäÅ[ÉNåüèo
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);

	LARGE_INTEGER start, end;
	QueryPerformanceCounter(&start);

	for (int i = 0; i < numElement; i++)
	{
		g_buf[i].s32 = i;
		g_buf[i].f32 = (float)(i) * 0.25f;
	}
	QueryPerformanceCounter(&end);

	//std::cout << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << std::endl;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;;

	auto hr = CreateDevice(&device, &context);
	if (FAILED(hr))
	{
		std::cout << "ÉfÉoÉCÉXê∂ê¨ÇÃé∏îs" << std::endl;
		getchar();
		return 0;
	}

	ID3DBlob* blob = nullptr;
	hr = CompileShaderFromFile(L"ComputeShader.hlsl", "main", &blob);
	if (FAILED(hr))
	{
		std::cout << "shader compileÇÃé∏îs" << std::endl;
		device->Release();
		getchar();
		return 0;
	}

	//shaderÇÃçÏê¨
	ID3D11ComputeShader* computeShader = nullptr;
	hr = CreateComputeSahder(device, &computeShader, blob);
	if (FAILED(hr))
	{
		std::cout << "Compute Shaderê∂ê¨ÇÃé∏îs" << std::endl;
		device->Release();
		context->Release();
		getchar();
		return 0;
	}


	//BufferÇÃçÏê¨
	ID3D11Buffer* buffer = nullptr;
	hr = CreateStructuredBuffer(device, &buffer, sizeof(BufType), numElement,g_buf);
	if (FAILED(hr))
	{
		std::cout << "Bufferê∂ê¨ÇÃé∏îs";
		device->Release();
		context->Release();
		computeShader->Release();
		getchar();
		return 0;
	}

	//åãâ ópBufferÇÃçÏê¨
	ID3D11Buffer* resultBuffer = nullptr;
	hr = CreateStructuredBuffer(device, &resultBuffer, sizeof(BufType), numElement, nullptr);
	if (FAILED(hr))
	{
		std::cout << "Result Bufferê∂ê¨ÇÃé∏îs";
		device->Release();
		context->Release();
		computeShader->Release();
		buffer->Release();
		getchar();
		return 0;
	}

	//ShaderResourceVIewÇÃçÏê¨
	ID3D11ShaderResourceView* resourceView = nullptr;
	hr = CreateBufferShaderResourceView(device, buffer, &resourceView);
	if (FAILED(hr))
	{
		std::cout << "Resource Viewê∂ê¨ÇÃé∏îs" << std::endl;
		device->Release();
		context->Release();
		computeShader->Release();
		buffer->Release();
		getchar();
		return 0;
	}

	//AccessViewÇÃçÏê¨
	ID3D11UnorderedAccessView* accessView = nullptr;
	hr = CraeteBufferUnorderedAccessView(device, resultBuffer, &accessView);
	if (FAILED(hr))
	{
		std::cout << "Unordered Access Viewê∂ê¨ÇÃé∏îs" << std::endl;
		device->Release();
		context->Release();
		computeShader->Release();
		buffer->Release();
		resourceView->Release();
		getchar();
		return 0;
	}

	ID3D11ShaderResourceView* shaderResourceViews[] = { resourceView };

	QueryPerformanceCounter(&start);

	//Compute ShaderÇëñÇÁÇπÇÈ
	RunComputeShader(context, computeShader, 1, shaderResourceViews, accessView, numElement, 1, 1);

	//ÉfÅ[É^ÇÃéÊÇËèoÇµ
	ID3D11Buffer* cloneBuf = nullptr;
	D3D11_BUFFER_DESC resultDesc;
	ZeroMemory(&resultDesc, sizeof(resultDesc));
	resultBuffer->GetDesc(&resultDesc);
	resultDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	resultDesc.Usage = D3D11_USAGE_STAGING;
	resultDesc.BindFlags = 0;
	resultDesc.MiscFlags = 0;

	hr = device->CreateBuffer(&resultDesc, nullptr, &cloneBuf);

	if (SUCCEEDED(hr))
	{
		context->CopyResource(cloneBuf, resultBuffer);
	}
	else if (FAILED(hr))
	{
		std::cout << "Bufferê∂ê¨ÇÃé∏îs";
		device->Release();
		context->Release();
		computeShader->Release();
		buffer->Release();
		getchar();
		return 0;
	}


	D3D11_MAPPED_SUBRESOURCE subRes;
	BufType* bufType = nullptr;

	hr = context->Map(cloneBuf, 0, D3D11_MAP_READ, 0, &subRes);

	if (FAILED(hr))
	{
		std::cout << "MapÇÃé∏îs" << std::endl;
		device->Release();
		context->Release();
		computeShader->Release();
		buffer->Release();
		resultBuffer->Release();
		resourceView->Release();
		accessView->Release();
		return 0;
	}

	bufType = (BufType*)subRes.pData;

	for (int i = 0; i < numElement; i++)
	{
		std::cout << "CPU = " << g_buf[i].s32 << std::endl;
		std::cout << "compute = " <<  bufType[i].s32 << std::endl;

		std::cout << "CPU = " << g_buf[i].f32 << std::endl;
		std::cout << "compute = " << bufType[i].f32 << std::endl;
		std::cout << std::endl;

	}

	context->Unmap(cloneBuf, 0);

	QueryPerformanceCounter(&end);

	//std::cout << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << std::endl;

	//å„èàóù
	device->Release();
	context->Release();
	computeShader->Release();
	buffer->Release();
	resultBuffer->Release();
	resourceView->Release();
	accessView->Release();

	std::cout << "ê≥èÌèIóπ" << std::endl;
	getchar();
	return 0;
}