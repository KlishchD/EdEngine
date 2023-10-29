#include "Core/Engine.h"
#include "Editor.h"

int main(int argc, char* argv[])
{
    Engine& engine = Engine::Create();

    engine.Start();
    engine.Initialize();

    std::shared_ptr<Editor> editor = std::make_shared<Editor>();

    engine.AddManager(editor);

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
