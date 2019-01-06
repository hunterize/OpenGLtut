#pragma once
#include <unordered_map>

#include <glm.hpp>

class CInputManager
{
public:
	CInputManager();
	~CInputManager();

	void Update();

	void PressKey(unsigned int key);
	void Releasekey(unsigned int key);

	bool IsKeyDown(unsigned int key) const;
	bool IskeyPressed(unsigned int key) const;

	glm::vec2 GetMouseCoord() const;
	void SetMouseCoord(float x, float y);

private:
	std::unordered_map<unsigned int, bool> m_cKeyMap;
	std::unordered_map<unsigned int, bool> m_cPreviousKeyMap;
	glm::vec2 m_cMouseCoord;
	bool CheckKeyState(unsigned int key, const std::unordered_map<unsigned int, bool>& map) const;
};

