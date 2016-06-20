#include "Line.h"
#include "Memtrack.h"


void Line::init(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
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

void Line::drawLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DXVECTOR4 color)
{
	float left, right, top, bottom;
	VertexType vert;
	HRESULT result;

	vert.position = D3DXVECTOR3(p1.x, p1.y, 0.0f);
	vert.color = color;
	vertices.push_back(vert);

	vert.position = D3DXVECTOR3(p2.x, p2.y, 0.0f);
	vert.color = color;
	vertices.push_back(vert);

}

void Line::catmullRomSpline(std::vector<D3DXVECTOR2> keyframes, int numPoints)
{
	if (keyframes.size() < 4)
	{
		MessageBox(NULL, L"Cannot evaluate Catmull-Rom spline for less than 4 keyframes", L"Not enough points", IDOK);
		return;
	}

	for (int i = 0; i < keyframes.size() - 3; i++)
	{
		for (int j = 0; j < numPoints; j++)
		{
			PointOnCurve(keyframes[i], keyframes[i + 1], keyframes[i + 2], keyframes[i + 3], (numPoints * 0.01f) * j);
		}
	}

	VertexType vert;
	vert.position = D3DXVECTOR3(keyframes[keyframes.size() - 2].x, keyframes[keyframes.size() - 2].y, 0.0f); 
	vert.color = D3DXVECTOR4(1,1,1,1);
	vertices.push_back(vert);

}

double cubicInterpolate(double y0, double y1, double y2, double y3,	double mu)
{
	double a0, a1, a2, a3, mu2;

	mu2 = mu*mu;
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;

	return(a0*mu*mu2 + a1*mu2 + a2*mu + a3);
}

float catmull2(float v0, float v1, float v2, float v3, float t) 
{
	float P = -v0 + 3 * v1 - 3 * v2 + v3;
	float Q = 2 * v0 - 5 * v1 + 4 * v2 - v3;
	float R = -v0 + v2;
	float S = 2 * v1;
	float r = ((P*t*t*t) + (Q*t*t) + (R*t) + S)*0.5f;
	return r;
}

void Line::PointOnCurve(D3DXVECTOR2 p0, D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DXVECTOR2 p3, float t)
{
	VertexType ret;

	float t2 = t * t;
	float t3 = t2 * t;

	double x = catmull2(p0.x, p1.x, p2.x, p3.x, t);

	double y = catmull2(p0.y, p1.y, p2.y, p3.y, t);

	ret.position = D3DXVECTOR3(x, y, 0.0f);
	ret.color = D3DXVECTOR4(1, 1, 1, 1);
	vertices.push_back(ret);
}

bool Line::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
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

void Line::flushToDrawBuffer(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// draw the vertex buffer to the back buffer
	deviceContext->Draw(vertices.size(), 0);

	// select which primtive type we are using
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// draw the vertex buffer to the back buffer
	deviceContext->Draw(vertices.size(), 0);

	vertices.clear();
	pVBuffer->Release();
}

void Line::cleanup()
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
