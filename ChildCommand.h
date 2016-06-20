#pragma once

class ChildCommand
{
public:
	char desc[100];
	char bonus[100];

	struct ChildCommand *nextChild;

	int getPixels()
	{
		int pom = 0;
		char *p = NULL;

		if (isPixel())
		{
			if ((p = strchr(bonus, '+')) != NULL)
			{
				sscanf(p + 1, "%d", &pom);
				return pom;
			}

			sscanf(bonus, "%d", &pom);
		}
		return pom;
	}

	int getPercents()
	{
		int pom = 0;

		if (isPercent())
		{

			sscanf(bonus, "%d", &pom);
			return pom;
		}
		return pom;
	}

	float * getRGBA()
	{
		if (this == NULL)
		{
			return 0;
		}
		float *rgbaVector = NULL;
		char *r, *g, *b, *a;

		if (strstr(bonus, "rgb"))
		{
			rgbaVector = new float[4];

			r = strchr(bonus, '(') + 1;
			g = strchr(bonus, ',') + 1;
			b = strchr(g, ',') + 1;

			if (!strchr(bonus, '.'))
			{
				int pomFloat = 0;

				sscanf(r, "%d", &pomFloat);
				rgbaVector[0] = (float)(pomFloat) / 255;

				sscanf(g, "%d", &pomFloat);
				rgbaVector[1] = (float)(pomFloat) / 255;

				sscanf(b, "%d", &pomFloat);
				rgbaVector[2] = (float)(pomFloat) / 255;

				if (strchr(bonus, 'a'))
				{
					a = strchr(b, ',') + 1;
					sscanf(a, "%d", &pomFloat);
					rgbaVector[3] = (float)(pomFloat) / 255;

				}
				else
				{
					rgbaVector[3] = 1.0f;
				}
			}
			else
			{
				sscanf(r, "%f", &rgbaVector[0]);
				sscanf(g, "%f", &rgbaVector[1]);
				sscanf(b, "%f", &rgbaVector[2]);

				if (strchr(bonus, 'a'))
				{
					a = strchr(b, ',') + 1;
					sscanf(a, "%f", &rgbaVector[3]);
				}
				else
				{
					rgbaVector[3] = 1.0f;
				}
			}
		}
		return rgbaVector;
	}

	bool isPercent()
	{
		if (this == NULL)
		{
			return 0;
		}
		if (strchr(bonus, '%'))
		{
			return 1;
		}

		return 0;
	}

	bool isPixel()
	{
		if (this == NULL)
		{
			return 0;
		}
		if (strstr(bonus, "px"))
		{
			return 1;
		}

		return 0;
	}

};