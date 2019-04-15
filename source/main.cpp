#include "engine/Platform.h"
#include "engine/Application.h"
#include "engine/Exception.h"
#include "engine/AppMessageBox.h"
#include "editor/EditorWorld.h"
#include "game/GameWorld.h"

#define WORLD_EDITOR

#ifdef WINDOWS
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int iCmdShow)
	{
		try
		{
			Application app;
			platform::InitializationApi initializationApi;

			initializationApi.hInstance = hInstance;

			#ifdef WORLD_EDITOR
				app.Initialize( initializationApi, new EditorWorld );
			#else
				app.Initialize( initializationApi, new GameWorld );
			#endif
			
			app.Run();
			app.Shutdown();
		}
		catch (const std::exception& e)
		{
			AppMessageBox::Error(e.what(), "Unknwon error!");
		}

		return 0;
	}

#endif
