#pragma once

class Device;
class Resource;
class Heap;
class DescriptorHeap;
class Shader;
class ShaderProgram;
class RootSignature;
class RootSignatureBuilder;
class GraphicsPipelineStateObjectBuilder;
class PipelineStateObject;
class CommandAllocator;
class CommandList;
class CommandQueue;
class SwapChain;

struct CustomResourceAllocator
{
public:
	void Initialize(Resource* resource);

	u64 Allocate(u64 size);
	void Deallocate(u64 offset);

	Resource* GetResource() const { return m_Resource; }
	operator Resource* () const { return m_Resource; }
protected:
	// I want to make it proper allocator in future but for time constraints I am using raw offset here.
	Resource* m_Resource;
	u64 m_Offset;
};

class RenderingContext final 
{
    constexpr static inline u64 UploadHeapSize = Memory::BytesToMB * 128;
    constexpr static inline u64 BuffersHeapSize = Memory::BytesToMB * 65;
    constexpr static inline u64 TexturesHeapSize = Memory::BytesToMB * 440;
    constexpr static inline u64 RenderTargetsHeapSize = Memory::BytesToMB * 110;
    constexpr static inline u64 UAVTargetsHeapSize = Memory::BytesToMB * 64;

    constexpr static inline u64 UploadBufferSize = Memory::BytesToMB * 120;
    constexpr static inline u64 GeometryVertexPoolSize = Memory::BytesToMB * 60;
    constexpr static inline u64 GeometryIndexPoolSize = Memory::BytesToMB * 4;

	// Could be a good idea to make it only allocated for the editor, probably
	// should be supplied by editor itself when needed but keeping it here
	// because want to make it easier for me to compile/debug shaders in build.
	constexpr static inline u32 ShaderCompilationResultsBufferSize = 4096;
protected:
	enum UploadDestinations : u8
	{
		UD_VertexPool = 0,
		UD_IndexPool,
		UD_TexturePool,
		UD_Count
	};

	struct UploadRequest
	{
		ResourceView* Destiantion;
		void* Data;
		u64 Offset;
		u32 Width;
		u32 Height;
		u32 Mips;
		u32 ItemSize;
		ResourceState State;

		// We do not copy mips at the moment.
		__forceinline u64 GetSize() const { return Width * Height * ItemSize; }
    };

	struct TransitionRequest
	{
		const ResourceView* View;
		ResourceState State;
	};

public:
	static inline RenderingContext& Get()
	{
		ED_ASSERT(s_Context, "Can not querry context as it was not yet created.");
        return *s_Context;
	}

	void Initialize(Window* window);
	void Deinitialize();

    void UploadModelDeferred(void* vertices, u32 vertexSize, u32 vertexCount, ResourceView& vertexBufferView, u32* indices, u32 indexCount, ResourceView& indexBufferView);
    void UploadModelImediate(void* vertices, u32 vertexSize, u32 vertexCount, ResourceView& vertexBufferView, u32* indices, u32 indexCount, ResourceView& indexBufferView);
	void FreeModel(ResourceView& vertexBufferView, ResourceView& indexBufferView);

	void UploadTextureDeferred(u8* data, u32 width, u32 height, u32 mips, PixelFormat format, ResourceView& view, ResourceState state, ccstr8 name = "DefaultTextureName", i32 nameSize = -1);
	void UploadTextureImediate(u8* data, u32 width, u32 height, u32 mips, PixelFormat format, ResourceView& view, ResourceState state, ccstr8 name = "DefaultTextureName", i32 nameSize = -1);
	void FreeTexture(ResourceView& view);

	ResourceView CreateRenderTarget(RenderTargetSizePolicy policy, PixelFormat format, ccstr8 name = "DefaultRenderTargetName", i32 nameSize = -1);
	ResourceView CreateRenderTarget(u32 width, u32 height, PixelFormat format, ccstr8 name = "DefaultRenderTargetName", i32 nameSize = -1);
	void FreeRenderTarget(ResourceView& view);

	Array<ResourceView> CreateUAVTarget(RenderTargetSizePolicy policy, u32 mips, PixelFormat format, ccstr8 name = "DefaultRenderTargetName", i32 nameSize = -1);
	Array<ResourceView> CreateUAVTarget(u32 width, u32 height, u32 mips, PixelFormat format, ccstr8 name = "DefaultRenderTargetName", i32 nameSize = -1);
	void FreeUAVTarget(ResourceView& view);
	void FreeUAVTargets(Array<ResourceView>& views);

	Shader* CreateShader(const ContentPath& path, ShaderType type);

	ShaderProgram* CreateShaderProgram();
	ShaderProgram* CreateShaderProgram(const ContentPath& path, ShaderType types);
	ShaderProgram* CreateShaderProgram(const ContentPath& vertex, const ContentPath& pixel);

	RootSignature* CreateRootSignature(const RootSignatureBuilder& builder);
	PipelineStateObject* CreatePipelineStateObject(ccstr8 name, const GraphicsPipelineStateObjectBuilder& builder);
	PipelineStateObject* CreatePipelineStateObject(ccstr8 name, RootSignature* signature, ShaderProgram* program);
	PipelineStateObject* CreatePipelineStateObject(ccstr8 name, RootSignature* signature, const ContentPath& path);

    void PreFrameUpdate();
    void PostFrameUpdate();
    void Present();

	Device* GetDevice() const { return m_Device; }
    DescriptorHeap* GetCBVHeap() const { return m_CBVHeap; }
    DescriptorHeap* GetSRVHeap() const { return m_SRVHeap; }
    DescriptorHeap* GetUAVHeap() const { return m_UAVHeap; }
    DescriptorHeap* GetRTVHeap() const { return m_RTVHeap; }
	DescriptorHeap* GetDSVHeap() const { return m_DSVHeap; }

	CommandList* CreateCommandList(CommandListType type, ccstr8 name = "DefaultCommandListName", i32 size = -1);

    CommandList* GetCopyCommandList() const { return m_CopyCommandList; }
    CommandList* GetGraphicsCommandList() const { return m_GraphicsCommandList; }
	CommandList* GetComputeCommandList() const { return m_ComputeCommandList; }

	CommandQueue* GetCopyCommandQueue() const { return m_CopyCommandQueue; };
    CommandQueue* GetGraphicsCommandQueue() const { return m_GraphicsCommandQueue; }
	CommandQueue* GetComputeCommandQueue() const { return m_ComputeCommandQueue; }

    ResourceView GetDisplayBufferRTV() const;
    ResourceView GetDisplayBufferSRV() const;

	Heap* GetUploadBuffersHeap() const { return m_UploadBuffersHeap; }
	Heap* GetBuffersHeap() const { return m_BuffersHeap; }
	Heap* GetTexturesHeap() const { return m_TexturesHeap; }

	Resource* GetGeometryVertexPool() const { return m_GeometryVertexPool.GetResource(); }
	Resource* GetGeometryIndexPool() const { return m_GeometryIndexPool.GetResource(); }

	void RequsetDefferedTransition(const ResourceView& view, ResourceState state);

	void Wait();

	void RequestShadersReloading();
	const Array<ccstr8>& GetShadersCompilationResults() const { return m_ShaderCompilationResults; }

	Window* GetWindow() const { return m_Window; }

	~RenderingContext();
protected:
	friend class Renderer;

    RenderingContext();

    class UploadRequestProcessor
    {
	public:
		UploadRequestProcessor(const Array<UploadRequest>& requests, Resource* uploadBuffer);

		__forceinline bool HasUnprocessedRequests() const { return m_Pointer < m_Requests.GetSize(); }
		void CollectAndUpload();
		void Copy(CommandList* list);

		~UploadRequestProcessor();	

	protected:
		const Array<UploadRequest>& m_Requests;
		Resource* m_UploadBuffer;
		u8* m_UploadBufferData;
        u32 m_Pointer;
		u32 m_PendingPointer;
    };

	void ProcessUploads();

protected:
	static inline RenderingContext* s_Context;

	Window* m_Window;

	Device* m_Device;

	SwapChain* m_SwapChain;

    Array<CommandList*> m_Lists;
    CommandList* m_CopyCommandList;
	CommandList* m_GraphicsCommandList;
	CommandList* m_ComputeCommandList;

	CommandQueue* m_CopyCommandQueue;
	CommandQueue* m_GraphicsCommandQueue;
	CommandQueue* m_ComputeCommandQueue;

	Heap* m_UploadBuffersHeap;
	Heap* m_BuffersHeap;
	Heap* m_TexturesHeap;
	Heap* m_RenderTargetsHeap;
	Heap* m_UAVTargetsHeap;

    Resource* m_UploadBuffer;

    CustomResourceAllocator m_GeometryVertexPool;
    CustomResourceAllocator m_GeometryIndexPool;

    DescriptorHeap* m_CBVHeap;
    DescriptorHeap* m_SRVHeap;
    DescriptorHeap* m_UAVHeap;
    DescriptorHeap* m_RTVHeap;
    DescriptorHeap* m_DSVHeap;

	Array<ResourceView> m_DisplayBuffersRTVs;
	Array<ResourceView> m_DisplayBuffersSRVs;

	StaticArray<Array<UploadRequest>, UD_Count> m_UploadRequests;

	Array<RootSignature*> m_RootSignatures;
	Array<Shader*> m_Shaders;
	Array<ShaderProgram*> m_ShaderPrograms;
	Array<PipelineStateObject*> m_PSOs;

	Array<TransitionRequest> m_TransitionRequests;

	bool m_PendingShadersReloading;
	Array<ccstr8> m_ShaderCompilationResults;
	cstr8 m_ShaderCompilationResultsBuffer;
};