#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "globals.h"
#include "eventManager.h"

int windowWidth = 1280;
int windowHeight = 720;
int msaaRes = 4;
bool fullscreen = false;
GLFWwindow* window;

double deltaTime = 1.0e-10;

EventManager globalEventManager;

void loadKeyBindings()
{
	std::ifstream bindingsFile("keyBindings.ini");

	if (bindingsFile.is_open())
	{
		std::string line;
		while (std::getline(bindingsFile, line))
		{
			char* key = new char[line.length() + 1];
			memcpy(key, line.c_str(), line.length() + 1);
			char* value = NULL;

			for (unsigned int i = 0; i < strlen(key); ++i)
			{
				if (key[i] == '=')
				{
					value = &key[i + 1];
					key[i] = char(0);
					break;
				}
			}

			if (value == NULL)
				continue;

			std::stringstream strVal;
			strVal << value;
			int keyVal;
			strVal >> keyVal;
			globalEventManager.addBinding(key, keyVal);

			delete[] key;
		}

		bindingsFile.close();
	}
}

void loadSettings()
{
	std::ifstream settingsFile("engine.ini");

	if (settingsFile.is_open())
	{
		std::string line;
		while (std::getline(settingsFile, line))
		{
			char* key = new char[line.length() + 1];
			memcpy(key, line.c_str(), line.length() + 1);
			char* value = NULL;

			for (unsigned int i = 0; i < strlen(key); ++i)
			{
				if (key[i] == '=')
				{
					value = &key[i + 1];
					key[i] = char(0);
					break;
				}
			}

			if (value == NULL)
				continue;

			if (!strcmp(key, "resX"))
				windowWidth = atoi(value);
			else if (!strcmp(key, "resY"))
				windowHeight = atoi(value);
			else if (!strcmp(key, "msaa"))
				msaaRes = atoi(value);
			else if (!strcmp(key, "fullscreen"))
				fullscreen = atoi(value) != 0;

			delete[] key;
		}

		settingsFile.close();
	}

	loadKeyBindings();
}