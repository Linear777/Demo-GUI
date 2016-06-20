////////////////////////////////////////////////////////////////////////////////
// Filename: Text.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Text_H_
#define _Text_H_

#include <FW1FontWrapper.h>
#include <vector>
#pragma comment(lib, "FW1FontWrapper.lib")


////////////////////////////////////////////////////////////////////////////////
// Class name: Text
////////////////////////////////////////////////////////////////////////////////
class Text
{
private:
	Text() {};
	Text(Text const&);
	void operator=(Text const&);
	IFW1Factory *pFW1Factory;
	IFW1FontWrapper *pFontWrapper;
	std::vector<std::wstring> phrases;
	std::vector<DWRITE_TEXT_RANGE> textRanges;

	struct TextParams
	{
		float fontsize;
		float x;
		float y;
		wchar_t *text;
		UINT32 color;
		unsigned int flags;
	};

	std::vector<TextParams> textData;

	ID3D11DeviceContext *devcon;
public:
	static Text& getInstance()
	{
		static Text instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	void init(ID3D11Device *pDevice, ID3D11DeviceContext *pContext);
	void updateDevCon(ID3D11DeviceContext *pContext);
	void drawSyntaxHighlightedText(ID3D11Device *pDevice, float fontsize, float x, float y, std::wstring text, unsigned int flags, int cursorPos);
	void addText(float fontsize, float x, float y, wchar_t *text, UINT32 color, unsigned int flags);
	void drawText();
	void initPhrases();
	void findAllPhrases(std::wstring text);
	unsigned long createRGBA(int r, int g, int b, int a);
	unsigned long createABGR(int r, int g, int b, int a);

};

#endif