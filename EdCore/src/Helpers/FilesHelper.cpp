#include "EdCore.h"
#include "FilesHelper.h"

void Files::CreateDirectory(ccstr8 path)
{
    std::filesystem::create_directories(path);
}