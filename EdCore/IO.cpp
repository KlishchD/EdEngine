#include "EdCore.h"

// #include "IO.h"
// 
// namespace IO
// {
//     struct ReadRequest
//     {
//         Path Source;
//         void* Buffer;
//         u64 Start;
//         u64 Size;
//         ReadCallback Callback;
//         bool Binary;
//     };
// 
//     struct WriteRequest
//     {
//         Path Destination;
//         void* Data;
//         u64 Size;
//         WriteCallback Callback;
//     };
// 
//     template <typename T>
//     struct ScopedRequest
//     {
//         T* Request;
// 
//         ScopedRequest(T* request) : Request(request) {}
//         
//         operator bool() const { return !!Request; }
//         
//         T* operator->() const { return Request; }
//         T* operator->() { return Request; }
// 
//         ~ScopedReadRequest() { Registry.RemoveRequest(Request); }
//     };
// 
// 
//     using ScopedReadRequest = ScopedRequest<ReadRequest>;
//     using ScopedWriteequest = ScopedRequest<WriteRequest>;
// 
//     class IORegistry
//     {
//     public:
//         ScopedReadRequest PullScopedReadRequest() const
//         {
//             return PullReadRequest();
//         }
// 
//         ReadRequest* PullReadRequest() const
//         {
//             return *m_ReadRequests.begin();
//         }
// 
//         ReadRequest* AddReadRequest()
//         {
//             std::scoped_lock _(m_Lock);
//             return m_ReadRequests.Allocate();
//         }
// 
//         void RemoveRequest(ReadRequest* request)
//         {
//             std::scoped_lock _(m_Lock);
//             m_ReadRequests.Deallocate(request);
//         }
// 
//         bool IsActive() const
//         {
//             return m_Active;
//         }
// 
//     public:
//         std::thread m_Thread;
//         std::mutex m_Lock;
//         bool m_Active;
// 
//         PoolAllocator<ReadRequest, 128> m_ReadRequests;
//         PoolAllocator<WriteRequest, 128> m_WriteRequests;
//     } Registry;
// 
// 
//     void AsyncReadFromFile(const Path& path, void* data, u64 start, u64 size, bool binary, ReadCallback callback)
//     {
//         ReadRequest* request = Registry.AddReadRequest();
//         request->Source = path;
//         request->Buffer = data;
//         request->Start = start;
//         request->Size = size;
//         request->Binary = binary;
//         request->Callback = callback;
//     }
// 
//     void AsyncWriteToFile(const Path& path, void* data, u64 size, WriteCallback callback)
//     {
// 
//     }
// 
//     void Initialize()
//     {
// 
//     }
// 
//     void Deinitialize()
//     {
// 
//     }
// 
//     void Process()
//     {
//         while (Registry.IsActive())
//         {
//             if (ScopedReadRequest request = Registry.PullScopedReadRequest())
//             {
//                 ccstr8 mode = request->Binary ? "rb" : "r";
//                 FILE* file;
// 
//                 ReadStates state = ReadStates::Success;
//                 u64 size;
// 
//                 do 
//                 {
//                     file = fopen(request->Source.Get(), mode);
// 
//                     bool foundFile = file;
//                     if (!foundFile)
//                     {
//                         state = ReadStates::FileNotFound;
//                         break;
//                     }
// 
//                     bool hasRequestedData = fseek(file, request->Start + request->Size, SEEK_SET);
//                     if (!hasRequestedData)
//                     {
//                         state = ReadStates::RequestedDataNotFound;
//                         break;
//                     }
// 
//                     bool canStartReading = fseek(file, request->Start, SEEK_SET);
//                     if (!canStartReading)
//                     {
//                         state = ReadStates::FailedToStartReading;
//                         break;
//                     }
// 
//                     size = fread(request->Buffer, 1, request->Size, file);
// 
//                     bool readAllData = size == request->Size;
//                     if (!readAllData)
//                     {
//                         state = ReadStates::FailedToReadWholeData;
//                         break;
//                     }
//                 } while (false);
// 
//                 if (file)
//                 {
//                     fclose(file);
//                 }
// 
//                 request->Callback(state, request->Buffer, request->Start, size);
//             }
//         }
//     }
// }
// 
// 
// 
// class Manager
// {
//     struct ReadResponse
//     {
//         enum States {
//             P,
// 
//         };
// 
//         void* Data;
//         u32 Size;
// 
//         bool Ready;
//         bool Consumed;
//     };
// 
//     struct ReadRequest
//     {
//         u32 ID;
//         Path Origin;
//         ReadRequest Result;
//     };
// 
// public:
//     static Manager& Get()
//     {
//         static Manager manager;
//         return manager;
//     }
// 
//     void Initialize();
//     void Deinitialize();
//     void Process();
// 
//     RequestID Read(const Path& path);
// 
// private:
//     Manager() = default;
// 
// protected:
//     std::thread m_Thread;
//     std::mutex m_RequestLock;
// 
//     Array<ReadRequest> m_ReadRequests;
// 
//     bool m_Active;
// };
// 
// 
// void Manager::Initialize()
// {
//     m_Active = true;
//     m_Thread = std::thread(this, &Manager::Process);
// }
// 
// void Manager::Deinitialize()
// {
//     m_Active = false;
//     m_Thread.join();
// }
// 
// void Manager::Process()
// {
//     while (m_Active)
//     {
//         ReadRequest& request = m_ReadRequests.
//     }
// }
// 
// Manager::ReadRequest& Manager::Read(const Path& path)
// {
//     ReadRequest& request = m_ReadRequests.Add();
//     request.Origin = path;
//     return request.Result;
// }
