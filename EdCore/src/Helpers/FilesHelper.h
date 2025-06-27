#pragma once

namespace Files
{
    void CreateDirectory(ccstr8 realtivePath);

    //static bool IsPlayRecordingExtension(ccstr8 extension) { return extension == PlayRecordingsExtension; }
    //static ccstr8 MakeRecordingsPath(ccstr8 name) { return PlayRecordingsFolderPath + "\\" + name + PlayRecordingsExtension; }
};
