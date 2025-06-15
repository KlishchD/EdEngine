#pragma once

class Path;
class ContentPath;

class Path
{
public:
    Path();
    Path(ccstr16 path);
    Path(ccstr8 path);
    Path(const Path& path);

    Path& operator=(const Path& path);
    Path& operator=(Path&& path);

    void Serialize(Archive<>& archive);

    ContentPath Content() const;

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

    bool IsRelative() const;
    bool IsAbsolute() const;

    bool Equal(const Path& other) const;

    bool operator==(const Path& other) const;
    bool operator!=(const Path& other) const;
protected:
    // TODO: Make custom string class and use it here.
    std::string m_Path;
};

class ContentPath : public Path
{
public:
    ContentPath();
    ContentPath(ccstr8 subpath);

    ContentPath(const ContentPath& path);
    ContentPath(ContentPath&& path);

    ContentPath& operator=(const ContentPath& path);
    ContentPath& operator=(ContentPath&& path);

    ContentPath& FromAbsolutePath(ccstr8 path);
};

namespace Files
{
    // TODO: Move all pathes here.

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
    const ContentPath& GetPlayRecordingsPath();
    const ContentPath& GetEditorLayoutPath();
    const ContentPath& GetShadersPath();
    const ContentPath& GetDefaultScenePath();
}