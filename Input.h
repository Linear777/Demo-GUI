#ifndef _Input_H_
#define _Input_H_


////////////////////////////////////////////////////////////////////////////////
// Class name: Input
////////////////////////////////////////////////////////////////////////////////
class Input
{
public:
	Input();
	Input(const Input&);
	~Input();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};

#endif