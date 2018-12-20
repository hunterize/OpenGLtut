#include "CInputManager.h"



CInputManager::CInputManager()
{
}


CInputManager::~CInputManager()
{
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
bool CInputManager::IsKeyDown(unsigned int key)
{
	return CheckKeyState(key, m_cKeyMap);
}

//key is pressed for the first time
bool CInputManager::IskeyPressed(unsigned int key)
{
	return CheckKeyState(key, m_cKeyMap) && !CheckKeyState(key, m_cPreviousKeyMap);
}

bool CInputManager::CheckKeyState(unsigned int key, std::unordered_map<unsigned int, bool>& map)
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
