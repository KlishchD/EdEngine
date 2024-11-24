#include "PlayRecorderControlsWidget.h"
#include "Core/Engine.h"
#include "PlayRecorder.h"
#include <imgui.h>

void PlayRecorderControlsWidget::Initialize()
{
    m_Recorder = Engine::Get().GetManager<PlayRecorder>();
}

void PlayRecorderControlsWidget::Deinitialize()
{

}

void PlayRecorderControlsWidget::Tick(float DeltaTime)
{
    if (ImGui::Begin("Play Recordings"))
    {
        static char name[1024];
        static bool isNameValid;

        {
            ImGui::BeginDisabled(m_Recorder->IsRecordingActive() || m_Recorder->IsRecordingReplayActive());

            if (ImGui::InputText("Name", name, 1024, ImGuiInputTextFlags_CharsNoBlank))
            {
                isNameValid = name[0] != '\0' && m_Recorder->IsRecordingNameValid(name);
            }

            ImGui::EndDisabled();
        }

        {
            ImGui::BeginDisabled(m_Recorder->IsRecordingActive() || m_Recorder->IsRecordingReplayActive() || !isNameValid);

            ImGui::SameLine();
            if (ImGui::Button("Start Recording"))
            {
                m_Recorder->StartRecording(name);
            }

            ImGui::EndDisabled();
        }

        {
            ImGui::BeginDisabled(!m_Recorder->IsRecordingActive() || m_Recorder->IsRecordingReplayActive());

            ImGui::SameLine();
            if (ImGui::Button("Stop Recording"))
            {
                m_Recorder->StopRrecording();
            }

            ImGui::EndDisabled();
        }

        {
            ImVec2 newSize = ImGui::GetWindowSize();
            newSize.x -= 50.0;
            newSize.y -= 70.0;
            ImGui::SetNextWindowSize(newSize);

            ImGui::BeginDisabled(m_Recorder->IsRecordingActive() || m_Recorder->IsRecordingReplayActive());

            if (ImGui::BeginListBox("##recorings_list"))
            {
                const std::vector<PlayRecording> recordings = m_Recorder->GetRecorings();
                for (uint32_t i = 0; i < recordings.size(); ++i)
                {
                    const PlayRecording& recording = recordings[i];

                    ImGui::PushID(recording.Name.c_str());

                    {
                        ImGui::Text(recording.Name.c_str());

                        ImGui::SameLine(ImGui::GetWindowSize().x - 75.0f);

                        if (ImGui::Button("Play", { 75.0f, 20.0f }))
                        {
                            m_Recorder->ReplayRecording(i);
                        }

                        ImGui::SameLine(ImGui::GetWindowSize().x - 170.0f);

                        ImGui::Text("%f sec.", recording.DurationSeconds);

                        ImGui::SameLine(ImGui::GetWindowSize().x - 245.0f);

                        ImGui::Text("%d inputs", recording.Inputs.size());

                        ImGui::SameLine(ImGui::GetWindowSize().x - 340.0f);

                        ImGui::Text("%d frames", recording.Runs[0].Stats.size());
                    }

                    {
                        for (uint32_t j = 0; j < recording.Runs.size(); ++j)
                        {
                            const RunStats& run = recording.Runs[j];

                            ImGui::PushID(j);

                            if (ImGui::TreeNode("##stats", "Run stats %s", std::format("{:%Y/%m/%d %H:%M}", run.RunTime).c_str()))
                            {
                                for (uint32_t k = 0; k < run.Stats.size(); ++k)
                                {
                                    ImGui::PushID(k);

                                    if (ImGui::TreeNodeEx("##", ImGuiTreeNodeFlags_None, "Frame %d", k))
                                    {
                                        const StatRecord& stat = run.Stats[k];

                                        ImGui::Text("Frame Time: %f", stat.FrameTime);
                                        ImGui::Text("Static Meshes Drawn: %d", stat.StaticMeshesDrawn);

                                        ImGui::Text("Point Lights Shaded: %d", stat.PointLightsShaded);
                                        ImGui::Text("Point Lights Shadows Drawn: %d", stat.PointLightsShadowsDrawn);

                                        ImGui::Text("Spot Lights Shaded: %d", stat.SpotLightsShaded);
                                        ImGui::Text("Spot Lights Shadows Drawn: %d", stat.SpotLightsShadowsDrawn);

                                        ImGui::Text("Directional Lights Shaded: %d", stat.DirectionalLightsShaded);
                                        ImGui::Text("Directional Lights Shadows Drawn: %d", stat.DirectionalLightsShadowsDrawn);

                                        ImGui::Text("Camera Position: %f %f %f", stat.CameraPosition.x, stat.CameraPosition.y, stat.CameraPosition.z);
                                        ImGui::Text("Camera Rotation: %f %f %f", stat.CameraRotation.x, stat.CameraRotation.y, stat.CameraRotation.z);

                                        ImGui::TreePop();
                                    }

                                    ImGui::PopID();
                                }

                                ImGui::TreePop();
                            }

                            ImGui::PopID();
                        }
                    }
                    
                    ImGui::PopID();
                }

                ImGui::EndListBox();
            }

            ImGui::EndDisabled();
        }
    }

    ImGui::End();
}

void PlayRecorderControlsWidget::DisplayReplayStats(const StatRecord& stat)
{
    
}

