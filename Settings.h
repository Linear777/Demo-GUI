#ifndef _SETTINGS_H_
#define _SETTINGS_H_


////////////////////////////////////////////////////////////////////////////////
// Class name: Text
////////////////////////////////////////////////////////////////////////////////
class Settings
{
public:
	static Settings& getInstance()
	{
		static Settings instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}

	int getScreenWidth()
	{
		return screenWidth;
	}

	int getScreenHeight()
	{
		return screenHeight;
	}

	void setScreenWidth(int width)
	{
		screenWidth = width;
	}

	void setScreenHeight(int height)
	{
		screenHeight = height;
	}

	bool isVsync()
	{
		return vsync;
	}

	void setVsync(bool vs)
	{
		vsync = vs;
	}

	bool isFullscreen()
	{
		return fullscreen;
	}

	void setFullscreen(bool fullscr)
	{
		fullscreen = fullscr;
	}

private:
	Settings() {};
	Settings(Settings const&);
	void operator=(Settings const&);

	int screenWidth;
	int screenHeight;
	bool vsync;
	bool fullscreen;

};

#endif