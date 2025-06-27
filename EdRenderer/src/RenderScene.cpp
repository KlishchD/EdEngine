#include "EdRendererPrivate.h"
#include "RenderScene.h"

void StaticMeshElement::PackRootParameters(u32* parameters) const
{
    void* memory = static_cast<void*>(parameters);
    
    DescriptorHeap* heap = RenderingContext::Get().GetSRVHeap();

    glm::mat3x3 normal = glm::inverse(glm::transpose(WorldTransform->GetMatrixWithOutTranslationAndScale()));

    memory = PutValue(memory, normal[0]);
    memory = PutValue(memory, heap->GetDescriptorIndex(AlbedoTextureView));

    memory = PutValue(memory, normal[1]);
    memory = PutValue(memory, heap->GetDescriptorIndex(NormalTextureView));
    
    memory = PutValue(memory, normal[2]);
    memory = PutValue(memory, heap->GetDescriptorIndex(RoughnessTextureView));

    memory = PutValue(memory, Metalic);
    memory = PutValue(memory, Roughness);
    memory = PutValue(memory, Emission);
    memory = PutValue(memory, heap->GetDescriptorIndex(MetalicTextureView));
}
