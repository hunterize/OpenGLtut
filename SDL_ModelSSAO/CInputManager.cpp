#include "CInputManager.h"



CInputManager::CInputManager() : m_cMouseCoord(-1.0f)
{
}


CInputManager::~CInputManager()
{
}

glm::vec2 CInputManager::GetMouseCoord() const 
{
	return m_cMouseCoord;
}

void CInputManager::SetMouseCoord(float x, float y)
{
	m_cMouseCoord.x = x;
	m_cMouseCoord.y = y;
}

void CInputManager::Update()
{
	for (auto& it : m_cKeyMap)
	{
		m_cPreviousKeyMap[it.first] = it.second;
	}
}

void CInputManager::PressKey(unsigned int key)
{
	m_cKeyMap[key] = true;
}
void CInputManager::Releasekey(unsigned int key)
{
	m_cKeyMap[key] = false;
}

//key's current status, like press and hold
bool CInputManager::IsKeyDown(unsigned int key) const
{
	return CheckKeyState(key, m_cKeyMap);
}

bool CInputManager::IsKeyUp(unsigned int key) const
{
	return !CheckKeyState(key, m_cKeyMap) && CheckKeyState(key, m_cPreviousKeyMap);
}

//key is pressed for the first time
bool CInputManager::IskeyPressed(unsigned int key) const 
{
	return CheckKeyState(key, m_cKeyMap) && !CheckKeyState(key, m_cPreviousKeyMap);
}

bool CInputManager::CheckKeyState(unsigned int key, const std::unordered_map<unsigned int, bool>& map) const
{
	auto it = map.find(key);
	if (it != map.end())
	{
		return it->second;
	}
	else
	{
		return false;
	}
}
