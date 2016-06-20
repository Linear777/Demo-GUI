#include "Rect.h"
#include "Memtrack.h"



void Rect::init(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
	
	//D3DX11CompileFromFile(L"rect_vs.hlsl", 0, 0, "main", "vs_5_0", 0, 0, 0, &VS, 0, 0);
	//D3DX11CompileFromFile(L"rect_ps.hlsl", 0, 0, "main", "ps_5_0", 0, 0, 0, &PS, 0, 0);

	struct ShaderHelper::shaderData *vsData = ShaderHelper::GetShaderByteCode("shaders/rect_vs.cso");
	struct ShaderHelper::shaderData *psData = ShaderHelper::GetShaderByteCode("shaders/rect_ps.cso");

	// encapsulate both shaders into shader objects
	device->CreateVertexShader(vsData->shaderCode, vsData->size, NULL, &pVS);
	device->CreatePixelShader(psData->shaderCode, psData->size, NULL, &pPS);

	// set the shader objects
	deviceContext->VSSetShader(pVS, 0, 0);
	deviceContext->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	device->CreateBuffer(&matrixBufferDesc, NULL, &pConstBuffer);

	device->CreateInputLayout(ied, 2, vsData->shaderCode, vsData->size, &pLayout);
	deviceContext->IASetInputLayout(pLayout);
}

void Rect::drawRect(int x, int y, int width, int height)
{
	drawRect(x, y, width, height, D3DXVECTOR4(1.0, 1.0, 1.0, 1.0f));
}

void Rect::drawRect(int x, int y, int width, int height, D3DXVECTOR4 color)
{
	float left, right, top, bottom;
	VertexType vert;
	HRESULT result;

	// Calculate the screen coordinates of the left side of the Quad.
	left = (float)((screenWidth / 2) * -1) + (float)x;

	// Calculate the screen coordinates of the right side of the Quad.
	right = left + (float)width;

	// Calculate the screen coordinates of the top of the Quad.
	top = (float)(screenHeight / 2) - (float)y;

	// Calculate the screen coordinates of the bottom of the Quad.
	bottom = top - (float)height;

	// First triangle.
	vert.position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vert.color = color;
	vertices.push_back(vert);

	vert.position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vert.color = color;
	vertices.push_back(vert);

	vert.position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vert.color = color;
	vertices.push_back(vert);

	// Second triangle.
	vert.position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vert.color = color;
	vertices.push_back(vert);

	vert.position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vert.color = color;
	vertices.push_back(vert);

	vert.position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vert.color = color;
	vertices.push_back(vert);
	
}

bool Rect::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(pConstBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &pConstBuffer);

	return true;
}

void Rect::flushToDrawBuffer(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
	if (vertices.size() == 0)
	{
		return;
	}
	
	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VertexType) * vertices.size();             // size is the VERTEX struct * N
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	device->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	deviceContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // lock buffer for writing
	memcpy(ms.pData, vertices.data(), sizeof(VertexType) * vertices.size());   // copy the data
	deviceContext->Unmap(pVBuffer, NULL);									   // unlock

	// select which vertex buffer to display
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	deviceContext->IASetInputLayout(pLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(pVS, NULL, 0);
	deviceContext->PSSetShader(pPS, NULL, 0);

	deviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	deviceContext->Draw(vertices.size(), 0);

	vertices.clear();
	pVBuffer->Release();
}

void Rect::cleanup()
{
	if (VS)
	{
		VS->Release();
	}
	if (PS)
	{
		PS->Release();
	}
	if (pVS)
	{
		pVS->Release();
	}
	if (pPS)
	{
		pPS->Release();
	}
	if (pLayout)
	{
		pLayout->Release();
	}
	if (pVBuffer)
	{
		pVBuffer->Release();
	}
	if (pConstBuffer)
	{
		pConstBuffer->Release();
	}
}
