#include "EdCore.h"

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "INVALID"
#endif

Path::Path()
{

}

Path::Path(ccstr16 path)
{
    ED_ASSERT(path, "Path provided to path struct can not be nullptr.");
    m_Path = Strings::Convert(path, true);
}

Path::Path(ccstr8 path)
{
    ED_ASSERT(path, "Path provided to path struct can not be nullptr.");
    m_Path = path;
}

Path::Path(const Path& path) : m_Path(path.Get())
{
}

Path::Path(Path&& path) : m_Path(std::move(path.m_Path))
{
}

void Path::Serialize(Archive<>& archive)
{
    archive & m_Path;
}

Path& Path::Append(ccstr8 subpath)
{
    if (subpath)
    {
        while (m_Path.size() && m_Path.back() == '\\') m_Path.pop_back();
        m_Path.append("\\");
        m_Path.append(subpath);
    }

    return *this;
}

Path& Path::Append(Path relative)
{
    ED_ASSERT(relative.IsRelative(), "Can only append relative path.");
    Append(relative.Get());

    return *this;
}

Path& Path::AppendExtension(ccstr8 extension)
{
    m_Path.append(".");
    m_Path.append(extension);

    return *this;
}

Path& Path::AppendFullExtension(ccstr8 extension)
{
    m_Path.append(extension);

    return *this;
}

Path& Path::Pop()
{
    m_Path = std::filesystem::path(m_Path).parent_path().string();

    return *this;
}

Path& Path::PopFile()
{
    if (IsFile())
    {
        Pop();
    }

    return *this;
}

Path& Path::ReplaceFile(ccstr8 filename)
{
    PopFile();
    Append(filename);

    return *this;
}

Path& Path::ReplaceFile(const Path& path)
{
    ED_ASSERT(path.IsRelative(), "Can only replace file with relative path.");
    
    PopFile();
    Append(path.Get());

    return *this;
}

Path& Path::ReplaceExtension(ccstr8 extension)
{
    if (IsFile())
    {
        m_Path = std::filesystem::path(m_Path).replace_extension(extension).string().c_str();
    }
 
    return *this;
}

Path& Path::ReplaceFullExtension(ccstr8 extension)
{
    if (IsFile())
    {
        m_Path = std::filesystem::path(m_Path).replace_extension(extension).string().c_str();
    }
 
    return *this;
}

ccstr8 Path::GetExtension() const
{
    std::string result = std::filesystem::path(m_Path).extension().string();
    return Strings::RequestString(result.c_str(), result.size(), true);
}

ccstr8 Path::GetFileName() const
{
    std::string result = std::filesystem::path(m_Path).filename().replace_extension().string();
    return Strings::RequestString(result.c_str(), result.size(), true);
}

ccstr8 Path::GetFullFileName() const
{
    std::string result = std::filesystem::path(m_Path).filename().string();
    return Strings::RequestString(result.c_str(), result.size(), true);
}

bool Path::IsValid() const
{
    return std::filesystem::exists(m_Path);
}

bool Path::IsFile() const
{
    return !std::filesystem::is_directory(m_Path.c_str());
}

bool Path::IsDirectory() const
{
    return std::filesystem::is_directory(m_Path.c_str());
}

bool Path::HasSubdirectories() const
{
    for (const std::filesystem::directory_entry& subentry : std::filesystem::directory_iterator(m_Path))
    {
        if (subentry.is_directory())
        {
            return true;
        }
    }

    return false;
}

template <typename CollectionType>
CollectionType GetAllPathsInternal(bool recursively, const std::string& path)
{
    CollectionType paths;

    if (recursively)
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(path))
        {
            paths.Add(entry.path().string().c_str());
        }
    }
    else
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
        {
            paths.Add(entry.path().string().c_str());
        }
    }

    return paths;
}

Array<Path> Path::GetAllPaths(bool recursively) const
{
    return GetAllPathsInternal<Array<Path>>(recursively, m_Path);
}

TemporaryArray<Path> Path::GetAllPathsTemporary(bool recursively) const
{
    return GetAllPathsInternal<TemporaryArray<Path>>(recursively, m_Path);
}

template <typename CollectionType>
CollectionType GetAllFoldersInternal(const std::string& path)
{
    CollectionType folders;
    
    i32 index = 0;
    do
    {
        i32 next = path.find_first_of("\\", index);

        if (next == std::string::npos)
        {
            next = path.size();
        }

        folders.Add({ &path[index], (u32)(next - index) });

        while (next < path.size() && path[next] == '\\') ++next;

        index = next;
    } while (index < path.size());

    return folders;
}

Array<Pair<ccstr8, u32>> Path::GetAllFolders() const
{
    return GetAllFoldersInternal<Array<Pair<ccstr8, u32>>>(m_Path);
}

TemporaryArray<Pair<ccstr8, u32>> Path::GetAllFoldersTemporary() const
{
    return GetAllFoldersInternal<TemporaryArray<Pair<ccstr8, u32>>>(m_Path);
}

bool Path::IsRelative() const
{
    return std::filesystem::path(m_Path).is_relative();
}

bool Path::IsAbsolute() const
{
    return std::filesystem::path(m_Path).is_absolute();
}

bool Path::Equal(const Path& other) const
{
    return m_Path == other.m_Path;
}

Path& Path::operator=(const Path& path)
{
    m_Path = path.m_Path;
    return *this;
}

Path& Path::operator=(Path&& path)
{
    m_Path = std::move(path.m_Path);
    return *this;
}

bool Path::operator!=(const Path& other) const
{
    std::error_code error;
    return !std::filesystem::equivalent(m_Path, other.m_Path, error);
}

bool Path::operator==(const Path& other) const
{
    std::error_code error;
    return std::filesystem::equivalent(m_Path, other.m_Path, error);
}

const Path& Files::GetPlayRecordingsPath()
{
    static ContentPath path = PlayRecordingsFolderName;
    return path;
}

const Path& Files::GetEditorLayoutPath()
{
    static ContentPath path = EditorLayoutFilename;
    return path;
}

const Path& Files::GetDefaultScenePath()
{
    static ContentPath path = []() {
        ContentPath result = ScenesFolderName;
        result.Append(DefaultSceneFilename);
        return result;
    }();

    return path;
}

const Path& Files::GetEditorIconPath()
{
  static ContentPath path = []() {
    ContentPath result;
    result.Append("Editor\\Icons\\Logo.ico");
    return result;
  }();

  return path;
}

const Path& Files::GetShadersReportPath()
{
  static Path path = []() {
    Path result = GetContentPath();
    result.Pop();
    result.Append("Reports\\shaders.txt");
    return result;
  }();

  return path;
}
