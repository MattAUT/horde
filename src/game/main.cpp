// COMP710 GP Framework 2022
#include <SDL.h>
#include <crtdbg.h> 
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <logmanager.h>
//#include <rapidjson/document.h>

#include "game.h"


int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//std::ifstream file("assets\\sprites\\samus\\anim.json");
	//std::stringstream stream;
	//stream << file.rdbuf();
	//std::string jsonString = stream.str();

	//rapidjson::Document jsonDocument;
	//jsonDocument.Parse(jsonString.c_str());

	//LogManager::GetInstance().Log(jsonDocument[0]["name"].GetString());

    Game& gameInstance = Game::GetInstance();
    if (!gameInstance.Initialise())
    {
        LogManager::GetInstance().Log("Game initialise failed!");

        return 1;
    }

    while (gameInstance.DoGameLoop())
    {
        // No body.
    }

    Game::DestroyInstance();
    LogManager::DestroyInstance();

    return 0;
}
