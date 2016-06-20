///////////////////////////////////////////////////////////////////////////////
// Filename: Text.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Text.h"
#include "Memtrack.h"

void Text::init(ID3D11Device *pDevice, ID3D11DeviceContext *pContext)
{
	TCHAR pwd[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pwd);

	//TODO: add fonts programmatically
	ShellExecuteW(GetActiveWindow(), L"close", pwd, NULL, NULL, SW_SHOWNORMAL);

	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	hResult = pFW1Factory->CreateFontWrapper(pDevice, L"uni 05_53", &pFontWrapper);
	initPhrases();
	devcon = pContext;
}

void Text::initPhrases()
{
	phrases.push_back(std::wstring(L"float4"));
	phrases.push_back(std::wstring(L"return"));
	phrases.push_back(std::wstring(L"struct"));
	phrases.push_back(std::wstring(L"cbuffer"));
	phrases.push_back(std::wstring(L"matrix"));
	phrases.push_back(std::wstring(L"case"));
	phrases.push_back(std::wstring(L"column_major"));
	phrases.push_back(std::wstring(L"const"));
	phrases.push_back(std::wstring(L"continue"));
	phrases.push_back(std::wstring(L"default"));
	phrases.push_back(std::wstring(L"discard"));
	phrases.push_back(std::wstring(L"double"));
	phrases.push_back(std::wstring(L"dword"));
	phrases.push_back(std::wstring(L"else"));
	phrases.push_back(std::wstring(L"export"));
	phrases.push_back(std::wstring(L"extern"));
	phrases.push_back(std::wstring(L"false"));
	phrases.push_back(std::wstring(L"float"));
	phrases.push_back(std::wstring(L"half"));
	phrases.push_back(std::wstring(L"if"));
	phrases.push_back(std::wstring(L"in"));
	phrases.push_back(std::wstring(L"inline"));
	phrases.push_back(std::wstring(L"inout"));
	phrases.push_back(std::wstring(L"int"));
	phrases.push_back(std::wstring(L"interface"));
	phrases.push_back(std::wstring(L"do"));
	phrases.push_back(std::wstring(L"NULL"));
	phrases.push_back(std::wstring(L"out"));
	phrases.push_back(std::wstring(L"register"));
	phrases.push_back(std::wstring(L"while"));

	//line, lineadj, linear, LineStream
	// min16float, min10float, min16int, min12int, min16uint
	//namespace, nointerpolation, noperspective
	//	out, OutputPatch
	//	packoffset, pass, pixelfragment, PixelShader, point, PointStream, precise
	//	RasterizerState, RenderTargetView, , row_major, RWBuffer, RWByteAddressBuffer, RWStructuredBuffer, RWTexture1D, RWTexture1DArray, RWTexture2D, RWTexture2DArray, RWTexture3D
	//	sample, sampler, SamplerState, SamplerComparisonState, shared, snorm, stateblock, stateblock_state, static, string, struct, switch, StructuredBuffer
	//	tbuffer, technique, technique10, technique11, texture, Texture1D, Texture1DArray, Texture2D, Texture2DArray, Texture2DMS, Texture2DMSArray, Texture3D, TextureCube, TextureCubeArray, true, typedef, triangle, triangleadj, TriangleStream
	//	uint, uniform, unorm, unsigned
	//	vector, vertexfragment, VertexShader, void, volatile
}

void Text::findAllPhrases(std::wstring text)
{
	textRanges.clear();
	for (auto &phrase : phrases)
	{
		size_t pos = text.find(phrase, 0);
		while (pos != std::string::npos)
		{
			DWRITE_TEXT_RANGE range = { pos, phrase.length()};

			textRanges.push_back(range);

			pos = text.find(phrase, pos + 1);
		}
	}
}

unsigned long Text::createABGR(int r, int g, int b, int a)
{
	return ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8)
		+ (a & 0xff);
}

unsigned long Text::createRGBA(int r, int g, int b, int a)
{
	return ((a & 0xff) << 24) + ((b & 0xff) << 16) + ((g & 0xff) << 8)
		+ (r & 0xff);
}

void Text::updateDevCon(ID3D11DeviceContext *pContext)
{
	devcon = pContext;
}

void Text::drawSyntaxHighlightedText(ID3D11Device *pDevice, float fontsize, float x, float y, std::wstring text, unsigned int flags, int cursorPos)
{

	std::wstring tex = text;
	/*tex =	L"float4 main(float4 position matrix nigga cbuffer shit and structured bullcrap : SV_POSITION, float4 color : COLOR) : SV_TARGET\n"
			L"{\n"
			L"\treturn color;\n"
			L"}\n";*/
	// get the DirectWrite factory used by the font-wrapper
	IDWriteFactory *pDWriteFactory;
	HRESULT hResult = pFontWrapper->GetDWriteFactory(&pDWriteFactory);

	// create an IDWriteTextFormat and set line spacing
	IDWriteTextFormat *pTextFormat;
	hResult = pDWriteFactory->CreateTextFormat(L"uni 05_53", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"", &pTextFormat);

	pTextFormat->SetIncrementalTabStop(40);

	IDWriteTextLayout *pTextLayout;
	unsigned int stringLength = tex.size();
	hResult = pDWriteFactory->CreateTextLayout(
		tex.c_str(),
		stringLength,
		pTextFormat,
		1000.0f,
		100.0f,
		&pTextLayout);
	DWRITE_TEXT_RANGE allText3 = { 0, stringLength };
	IFW1ColorRGBA *rgba;
	IFW1ColorRGBA *cursorColor;

	pFW1Factory->CreateColor(createRGBA(255, 0, 0, 255), &rgba);
	pFW1Factory->CreateColor(createRGBA(0, 255, 0, 255), &cursorColor);

	findAllPhrases(tex);
	for (auto &range : textRanges)
	{
		pTextLayout->SetDrawingEffect(rgba, range);
	}

	DWRITE_TEXT_RANGE cursorRange = { cursorPos, 1 };
	
	pTextLayout->SetDrawingEffect(cursorColor, cursorRange);

	pTextLayout->SetFontSize(14, allText3);
	
	pFontWrapper->DrawTextLayout(devcon, pTextLayout, x, y, 0xffffffff, NULL, NULL, FW1_RESTORESTATE | flags);
}


void Text::addText(float fontsize, float x, float y, wchar_t *text, UINT32 color, unsigned int flags)
{
	TextParams tp;
	tp.fontsize = fontsize;
	tp.x = x;
	tp.y = y;
	tp.text = text;
	tp.color = color;
	tp.flags = flags;

	textData.push_back(tp);
}

void Text::drawText()
{
	for (auto &texts : textData) // access by reference to avoid copying
	{
		pFontWrapper->DrawString(
			devcon,
			texts.text,// String
			texts.fontsize,// Font size
			texts.x,// X position
			texts.y,// Y position
			texts.color,// Text color, 0xAaBbGgRr
			FW1_RESTORESTATE | texts.flags
			);
		
	}
	textData.clear();
}
