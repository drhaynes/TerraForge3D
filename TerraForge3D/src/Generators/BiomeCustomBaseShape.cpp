#include "Generators/BiomeCustomBaseShape.h"
#include "Data/ApplicationState.h"
#include "Utils/Utils.h"
#include "Profiler.h"

BiomeCustomBaseShape::BiomeCustomBaseShape(ApplicationState* appState)
{
	m_AppState = appState;

	m_WorkingDataBuffer = std::make_shared<GeneratorData>();
	
	// this is just a preview texture, it is not used for anything else
	// so its ok for it to be low resolution
	m_PreviewTexture = std::make_shared<GeneratorTexture>(512, 512);

	m_RequireBaseShapeUpdate = true;
	m_RequireUpdation = true;
	m_Enabled = false;
}

BiomeCustomBaseShape::~BiomeCustomBaseShape()
{
}

bool BiomeCustomBaseShape::ShowShettings()
{
	if (ImGui::CollapsingHeader("Statistics"))
	{
		ImGui::Text("Time Taken: %f ms", m_CalculationTime);
	}

	BIOME_UI_PROPERTY(ImGui::Checkbox("Enabled", &m_Enabled));

	if (ImGui::Button("Reload Base Shape"))
	{
		m_WorkingDataBuffer->SaveToFile("asd.txt");
		m_RequireBaseShapeUpdate = true;
		m_RequireUpdation = true;
	}

	if (ImGui::BeginTabBar("Core Settings Type"))
	{
		if (ImGui::BeginTabItem("Draw"))
		{
			ImGui::PushID("BiomeCustomBaseShape Edit Mode -> Draw");
			BIOME_UI_PROPERTY(ShowDrawEditor());
			ImGui::PopID();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	return m_RequireUpdation;
}

void BiomeCustomBaseShape::Update(GeneratorData* sourceBuffer, GeneratorData* targetBuffer, GeneratorData* swapBuffer)
{
	START_PROFILER();

	if (m_RequireBaseShapeUpdate)
	{
		if (sourceBuffer)
		{
			sourceBuffer->CopyTo(m_WorkingDataBuffer.get());
		}
		else
		{
			Log("Source buffer found to be null when base shape load was requested!");
		}
		m_RequireBaseShapeUpdate = false;
	}

	m_WorkingDataBuffer->CopyTo(sourceBuffer);

	END_PROFILER(m_CalculationTime);

	m_RequireUpdation = false;
}

SerializerNode BiomeCustomBaseShape::Save()
{
	return CreateSerializerNode();
}

void BiomeCustomBaseShape::Load(SerializerNode node)
{
	
}

void BiomeCustomBaseShape::Resize()
{
	auto size = m_AppState->mainMap.tileResolution * m_AppState->mainMap.tileResolution * sizeof(float);
	m_WorkingDataBuffer->Resize(size);
	m_RequireBaseShapeUpdate = true;
	m_RequireUpdation = true;
}

bool BiomeCustomBaseShape::ShowDrawEditor()
{
	ViewportManager* activeViewport = nullptr;
	for (auto editor : m_AppState->viewportManagers)
	{
		if (editor->IsActive())
		{
			activeViewport = editor;
			break;
		}
	}

	if (activeViewport)
	{
		auto posOnTerrain = activeViewport->GetPositionOnTerrain();
		m_DrawSettings.m_BrushPositionX = posOnTerrain.x;
		m_DrawSettings.m_BrushPositionY = posOnTerrain.y;
		m_AppState->rendererManager->GetObjectRenderer()->SetCustomBaseShapeDrawSettings(&m_DrawSettings);
		// m_AppState->rendererManager->GetObjectRenderer()->Set
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			//Log("Left mouse button is down");
		}

		if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
		{
			activeViewport->SetControlEnabled(false);
			if (ImGui::IsKeyDown(ImGuiKey_S))
			{
				m_DrawSettings.m_BrushSize += ImGui::GetIO().MouseWheel * 0.1f;
				m_DrawSettings.m_BrushSize = glm::clamp(m_DrawSettings.m_BrushSize, 0.0f, 2.0f);
			}
			else if (ImGui::IsKeyDown(ImGuiKey_F))
			{
				m_DrawSettings.m_BrushFalloff += ImGui::GetIO().MouseWheel * 0.1f;
				m_DrawSettings.m_BrushFalloff = glm::clamp(m_DrawSettings.m_BrushFalloff, 0.0f, 1.0f);
			}
		}
		 
	}

	ImGui::SliderFloat("Brush Size", &m_DrawSettings.m_BrushSize, 0.0f, 2.0f);
	ImGui::SliderFloat("Brush Strength", &m_DrawSettings.m_BrushStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("Brush Fall Off", &m_DrawSettings.m_BrushFalloff, 0.0f, 1.0f);

	return m_RequireUpdation;
}

