#include "EdEditor.h"

#if ENABLE_ED_TEST == 1
#   include "PlayRecorder.h"
#endif

i32 main(i32 argc, c8* argv[])
{
    Engine& engine = Engine::Create();

    engine.Start();
    engine.Initialize();

    {
        Editor editor;

        editor.Initialize(&engine);

#if ENABLE_ED_TEST == 1
        engine.CreateManager<PlayRecorder>();
#endif

        while (engine.IsRunning())
        {
            engine.Update();
        }
    }

    engine.Deinitialize();

    Engine::Delete();

    return 0;
}
