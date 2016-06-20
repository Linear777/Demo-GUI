#pragma once

#include <vector>
#include <d3d11.h>
#include <d3dx11.h>

#include <d3dx10.h>
#include "ShaderHelper.h"

class Rect
{
public:

	static Rect& getInstance()
	{
		static Rect instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void drawRect(int x1, int y1, int width, int height);
	void drawRect(int x1, int y1, int width, int height, D3DXVECTOR4 color);
	void flushToDrawBuffer(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void cleanup();
	void setScreenSize(int x, int y)
	{
		screenWidth = x;
		screenHeight = y;
	}
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

private:

	Rect() {};
	Rect(Rect const&);
	void operator=(Rect const&);

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	std::vector<VertexType> vertices;
	int screenWidth;
	int screenHeight;
	ID3D10Blob *VS, *PS;
	ID3D11VertexShader *pVS;
	ID3D11PixelShader *pPS;
	ID3D11InputLayout *pLayout;
	ID3D11Buffer *pVBuffer;
	ID3D11Buffer *pConstBuffer;
	D3D11_BUFFER_DESC matrixBufferDesc;
	
};
