#include "EdEditor.h"
#include "EditorCamera.h"

CameraElement EditorCamera::ToElement() const
{
    CameraElement element;
    element.Position = CameraObject.GetPosition();
    element.Rotation = CameraObject.GetRotation();
    element.Fov = 90.0f;
    element.Near = 1.0f;
    element.Far = 10000.0f;
    return element;
}
