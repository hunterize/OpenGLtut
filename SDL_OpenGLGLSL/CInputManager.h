#pragma once
#include <unordered_map>

class CInputManager
{
public:
	CInputManager();
	~CInputManager();

	void Update();

	void PressKey(unsigned int key);
	void Releasekey(unsigned int key);

	bool IsKeyDown(unsigned int key);
	bool IskeyPressed(unsigned int key);

private:
	std::unordered_map<unsigned int, bool> m_cKeyMap;
	std::unordered_map<unsigned int, bool> m_cPreviousKeyMap;
	bool CheckKeyState(unsigned int key, std::unordered_map<unsigned int, bool>& map);
};

