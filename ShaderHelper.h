#pragma once
#include <Windows.h>
#include <stdio.h>



class ShaderHelper
{
public:
	
	struct shaderData
	{
		char *shaderCode;
		UINT size;
	};

	static struct shaderData *GetShaderByteCode(char *compiledShader)
	{
		FILE *pFile = 0;
		char *shaderCode = 0;
		UINT fSize = 0;
		UINT numRead = 0;
		struct shaderData *sData = (struct shaderData *)malloc(sizeof(struct shaderData));

		pFile = fopen(compiledShader, "rb");

		fseek(pFile, 0, SEEK_END);
		fSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		shaderCode = (char *)malloc(fSize);

		while (numRead != fSize)
		{
			numRead = fread(&shaderCode[numRead], 1, fSize, pFile);
		}

		fclose(pFile);

		sData->shaderCode = shaderCode;
		sData->size = fSize;

		return sData;
	}
};