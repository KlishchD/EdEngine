#include "EdEditor.h"

#if ENABLE_ED_TEST == 1
#   include "PlayRecorder.h"
#endif

i32 main(i32 arguments_count, ccstr8* arguments)
{
  Engine& engine = Engine::Create();

  engine.Start();
  engine.Initialize(arguments_count, arguments);

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
