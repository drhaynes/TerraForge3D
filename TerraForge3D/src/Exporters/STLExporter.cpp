#include "Exporters/STLExporter.h"
#include "Base/BinaryFileWriter.h"
#include "Utils/Utils.h"

#include <string>
#include <sstream>

STLExporter::STLExporter()
{
}

STLExporter::~STLExporter()
{
}

bool STLExporter::ExportBinary(const std::string& path, Mesh* mesh, float* progress)
{
	if (!progress) progress = &m_Progress; *progress = 0.0f;
	BinaryFileWriter writer(path);
	buffer[0] = buffer[1] = 0;
	if (!writer.IsOpen()) return false;
	memset(buffer, 0, 80); sprintf(buffer, "Generated by TerraForge3D Gen 2");
	writer.Write(buffer, 80);
	uint32_t facet_count = (uint32_t)(mesh->GetFaceCount());
	writer.Write(facet_count);
	float tmpp = 1.0f / mesh->GetIndexCount();
	for (int i = 0; i < mesh->GetFaceCount(); i++)
	{
		const auto& face = mesh->GetFace(i);
		const auto& pa = mesh->GetPosition(face.a);
		const auto& pb = mesh->GetPosition(face.b);
		const auto& pc = mesh->GetPosition(face.c);
		const auto normal = glm::normalize(glm::cross(glm::vec3(pc - pb), glm::vec3(pa - pb)));
		const float data[12] = { normal.x, normal.y, normal.z, pa.x, pa.y, pa.z, pb.x, pb.y, pb.z, pc.x, pc.y, pc.z };
		writer.Write(data, sizeof(float) * 12);
		writer.Write(buffer, 2);
		*progress = tmpp * i * 0.95f + 0.01f;
	}
	*progress = 1.0f;
	return true;
}

bool STLExporter::ExportASCII(const std::string& path, Mesh* mesh, float* progress)
{
	if (!progress) progress = &m_Progress; *progress = 0.0f;
	BinaryFileWriter writer(path);
	if (!writer.IsOpen()) return false;
	std::stringstream out_strm;
	out_strm << "solid main_mesh_object\n";
	float tmpp = 1.0f / mesh->GetIndexCount();
	for (int i = 0; i < mesh->GetFaceCount(); i++)
	{
		const auto& face = mesh->GetFace(i);
		const auto& pa = mesh->GetPosition(face.a);
		const auto& pb = mesh->GetPosition(face.b);
		const auto& pc = mesh->GetPosition(face.c);
		const auto normal = glm::normalize(glm::cross(glm::vec3(pc - pb), glm::vec3(pa - pb)));
		sprintf(buffer,
			"    facet normal % f % f % f" "\n"
			"        outer loop" "\n"
			"		     vertex %f %f %f" "\n"
			"		     vertex %f %f %f" "\n"
			"		     vertex %f %f %f" "\n"
			"        endloop" "\n"
			"    endfacet" "\n",
			normal.x, normal.y, normal.z,
			pa.x, pa.y, pa.z,
			pb.x, pb.y, pb.z,
			pc.x, pc.y, pc.z
		);
		out_strm << buffer;
		*progress = tmpp * i * 0.90f + 0.01f;
	}
	out_strm << "endsolid\n";
	const std::string out_str = out_strm.str();
	writer.Write(out_str.data(), out_str.size());
	*progress = 1.0f;
	return true;
}