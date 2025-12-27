// #pragma once
// 
// namespace IO
// {
//     enum class ReadStates
//     {
//         FileNotFound,
//         RequestedDataNotFound,
//         FailedToStartReading,
//         FailedToReadWholeData,
//         Success
//     };
// 
//     using ReadCallback = void(*)(ReadStates state, void* data, u64 start, u64 size);
//     using WriteCallback = void(*)(void* data, u64 size);
// 
//     void AsyncReadFromFile(const Path& path, void* data, u64 start, u64 size, bool binary, ReadCallback callback);
//     void AsyncWriteToFile(const Path& path, void* data, u64 size, WriteCallback callback);
// 
//     void Initialize();
//     void Deinitialize();
// };