#include <string>
#include <vector>
#include <d3d11.h>
#include <d3dx11.h>

#include <d3dx10.h>

typedef struct pass_detail
{
	int passCount;
	ID3D10Blob *pVertexShaderBlob;
	ID3D10Blob *pPixelShaderBlob;
	ID3D10Blob *pGeometryShaderBlob;
	ID3D11VertexShader *pVertexShader;
	ID3D11PixelShader *pPixelShader;
	ID3D11GeometryShader *pGeometryShader;
}pass_t;

typedef struct technique_detail
{
	std::string techniqueName;
	std::vector<pass_t> passes;
}technique_t;

class Effect
{
public:
	void setVertexShader();
	void setPixelShader();
	void setGeometryShader();

private:
	std::vector<technique_t> techniques;
	

	
};