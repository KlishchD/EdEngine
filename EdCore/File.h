#pragma once

class Path
{
public:
    Path();
    Path(ccstr16 path);
    Path(ccstr8 path);
    Path(const Path& path);
    Path(Path&& path);

    Path& operator=(const Path& path);
    Path& operator=(Path&& path);

    void Serialize(Archive<>& archive);

    Path& Append(Path relative);
    Path& Append(ccstr8 subpath);
    Path& AppendExtension(ccstr8 extension);
    Path& AppendFullExtension(ccstr8 extension);
    Path& Pop();
    Path& PopFile();

    Path& ReplaceFile(ccstr8 filename);
    Path& ReplaceFile(const Path& path);
    Path& ReplaceExtension(ccstr8 exension);
    Path& ReplaceFullExtension(ccstr8 extension);

    ccstr8 GetExtension() const;
    ccstr8 GetFileName() const;
    ccstr8 GetFullFileName() const;

    const estd::stack_string_512* GetPtr() const { return &m_Path; }
    estd::stack_string_512* GetPtr() { return &m_Path; }
    ccstr8 Get() const { return m_Path.c_str(); }
    u32 GetSize() const { return m_Path.size(); }

    bool IsValid() const;
    bool IsFile() const;
    bool IsDirectory() const;
    bool HasSubdirectories() const;

    Array<Path> GetAllPaths(bool recursively) const;
    TemporaryArray<Path> GetAllPathsTemporary(bool recursively) const;

    Array<Pair<ccstr8, u32>> GetAllFolders() const;
    TemporaryArray<Pair<ccstr8, u32>> GetAllFoldersTemporary() const;

    void Remove() const;

    bool IsRelative() const;
    bool IsAbsolute() const;

    bool Equal(const Path& other) const;

    bool operator==(const Path& other) const;
    bool operator!=(const Path& other) const;

    virtual ~Path() = default;
protected:
    estd::stack_string_512 m_Path;
};

namespace Files
{
    // TODO: Move all paths here.

    constexpr ccstr8 ContentFolderName = "Resources";
    constexpr ccstr8 PlayRecordingsFolderName = "PlayRecordings";
    constexpr ccstr8 ScenesFolderName = "Scenes";
    constexpr ccstr8 ShadersFolderName = "Shaders\\hlsl";
    
    constexpr ccstr8 EditorLayoutFilename = "imgui.ini";
    constexpr ccstr8 DefaultSceneFilename = "Default.edscene";

    constexpr ccstr8 PlayRecordingsExtension = ".edprec";

    constexpr ccstr8 FullMeshAssetExtension = ".edmesh";
    constexpr ccstr8 FullTextureAssetExtension = ".edtexture";
    constexpr ccstr8 FullMaterialAssetExtension = ".edmaterial";
    constexpr ccstr8 FullPrefabAssetExtension = ".edprefab";
    constexpr ccstr8 FullSceneAssetExtension = ".edscene";

    constexpr ccstr8 MeshAssetExtension = "edmesh";
    constexpr ccstr8 TextureAssetExtension = "edtexture";
    constexpr ccstr8 MaterialAssetExtension = "edmaterial";
    constexpr ccstr8 PrefabAssetExtension = "edprefab";
    constexpr ccstr8 SceneAssetExtension = "edscene";

    const Path& GetContentPath();
    const Path& GetPlayRecordingsPath();
    const Path& GetEditorLayoutPath();
    const Path& GetShadersPath();
    const Path& GetDefaultScenePath();
    const Path& GetEditorIconPath();

    const Path& GetShadersReportPath();
}

template <typename PathSourceType>
class Subpath : public Path
{
public:
  Subpath() : Path(PathSourceType()()) {}
  Subpath(ccstr8 subpath) : Path(PathSourceType()())
  {
    ED_ASSERT(subpath, "Relative path for subpath must be not null.");
    Path::Append(subpath);
  }

  Subpath(const Path& path) : Path(path) { }
  Subpath(const Subpath& path) : Path(path) { }

  Subpath(Path&& path) : Path(std::move(path)) { }
  Subpath(Subpath&& path) : Path(std::move(path)) { }

  Subpath& operator=(const Path& path) { Path::operator=(path); return *this; }
  Subpath& operator=(const Subpath& path) { Path::operator=(path); return *this; }

  Subpath& operator=(Path&& path) { return Path::operator=(std::move(path)); }
  Subpath& operator=(Subpath&& path) { return Path::operator=(std::move(path)); }

  Subpath& operator=(ccstr8 subpath)
  {
    ED_ASSERT(subpath, "Relative path for subpath must be not null.");
    Path::operator=(PathSourceType()());
    Path::Append(subpath);
    return *this;
  }
};

struct ContentPathSource
{
  ccstr8 operator()() const
  {
    return Files::GetContentPath().Get();
  }
};

struct ShadersPathSource
{
  ccstr8 operator()() const
  {
    return Files::GetShadersPath().Get();
  }
};

using ContentPath = Subpath<ContentPathSource>;
using ShaderPath = Subpath<ShadersPathSource>;
