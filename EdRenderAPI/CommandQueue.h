#pragma once

class CommandList;

class CommandQueue
{
public:
  CommandQueue(CommandListType type, bool hightPriority, ccstr8 name = "DefaultCommandQueueName");

  void SetDebugName(ccstr8 name);
  void GetDebugName(ccstr8& name);

  void Execute(CommandList* list);

  u32 Signal() const;
  void GPUWait(CommandQueue* queue, u32 value);
  void CPUWait();

  // TODO: Needs push and pop count checks
  void BeginEvent(ccstr8 name);
  void EndEvent();

  template <typename T>
  __forceinline T* GetNativeHandle() const
  {
    return (T*)m_NativeHandle;
  }

  __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

  ~CommandQueue();

protected:
  uptr m_NativeHandle;
  uptr m_FenceNativeHandle;
  HANDLE m_Handle;
};

struct GPUScopedEvent
{
  CommandQueue* Queue;

  GPUScopedEvent(CommandQueue* queue, ccstr8 name) : Queue(queue)
  {
    Queue->BeginEvent(name);
  }

  ~GPUScopedEvent()
  {
    Queue->EndEvent();
  }
};