#include "Core/Engine.h"
#include "PlayRecorder.h"
#include "Editor.h"

int main(int argc, char* argv[])
{
    Engine& engine = Engine::Create();

    engine.Start();
    engine.Initialize();

    engine.AddManager<Editor>();

#if ENABLE_ED_TEST == 1
    engine.AddManager<PlayRecorder>();
#endif

    while (true) 
    {
        if (engine.IsRunning())
        {
            engine.Update();
        }
        else
        {
            engine.Deinitialize();
            break;
        }
    }

    Engine::Delete();

    return 0;
}
