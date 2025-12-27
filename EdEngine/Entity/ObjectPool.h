#pragma once

// 
// template <typename ObjectType, u32 InvalidHandleValue, u32 Size>
// class ObjectPool
// {
// public:
//     ObjectPool(const c8* name)
//     {
//         Objects = Memory::Get().RequestStaticMemory(sizeof(ObjectType) * Size, name);
// 
// 
//         u32 occupancySize = CalculateOccupancyArraySize();
//         Occupancy = Memory::Get().RequestStaticMemory(sizeof(u32) * CalculateOccupancyArraySize(), name);
//         ZeroMemory(Occupancy, occupancySize);
//     }
// 
//     inline void SetOccupancy(ObjectHandle handle, bool status)
//     {
//         ED_ASSERT(handle != InvalidHandleValue, "Expected valid handle");
//         Occupancy[handle / 32] = (u32(status) << (handle % 32));
//     }
// 
//     inline bool IsOccupied(ObjectHandle handle) const
//     {
//         ED_ASSERT(handle != InvalidHandleValue, "Expected valid handle");
//         ED_ASSERT(handle < Size, "Attempeted to access a non existing object.");
//         return Occupancy[handle / 32] & (1 << (handle % 32));
//     }
// 
//     inline ObjectType& GetObject(ObjectHandle handle)
//     {
//         ED_ASSERT(handle != InvalidHandleValue, "Expected valid handle");
//         ED_ASSERT(handle < Size && IsOccupied(handle), "Attempeted to access a non existing object.");
//         return Objects[handle];
//     }
// 
//     inline void FreeObject(ObjectHandle handle)
//     {
//         ED_ASSERT(handle != InvalidHandleValue, "Expected valid handle");
//         SetOccupancy(handle, false);
//     }
// 
//     inline ObjectHandle AllocateObject()
//     {
//         ED_ASSERT(FarthestActiveSlot > Size, "Attempted to allocate more objects than pool's size.");
//         // TODO: Add some kind of logic to allocate in objects at the start of the array
//         // or better yet use allocators ???)
//         SetOccupancy(FarthestActiveSlot, true);
//         return FarthestActiveSlot++;
//     }
// 
//     inline ObjectHandle GetFarthestActiveSlot() const
//     {
//         return FarthestActiveSlot;
//     }
// private:
//     static constexpr u32 CalculateOccupancyArraySize()
//     {
//         if (Size < 32) return 1;
//         return Size / 32 + 1;
//     }
// private:
//     ObjectType* Objects;
//     u32* Occupancy;
//     ObjectHandle FarthestActiveSlot;
// };