#pragma warning(disable: 4010)
#pragma warning(disable: 4819)
#pragma warning(disable: 4828)
#include "../../../tnl/tnl_math.h"
#include "../dxe_mesh.h"
#include "../../OBJLoader.h"

namespace dxe {

	//----------------------------------------------------------------------------------------
	std::vector<Shared<Mesh>> Mesh::CreateFromFileObj(const std::wstring& file_path, const float base_scale)
	{
		OBJMESH* obj = new OBJMESH();
		std::vector<Shared<Mesh>> create_meshs;

		// 1. 必要となるUTF-8のバッファサイズを取得
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, file_path.c_str(), -1, NULL, 0, NULL, NULL);
		std::vector<char> utf8_filename(size_needed);

		// 2. wchar_t から UTF-8 へ変換
		WideCharToMultiByte(CP_UTF8, 0, file_path.c_str(), -1, &utf8_filename[0], size_needed, NULL, NULL);

		obj->LoadFile(&utf8_filename[0]);

		std::tuple<std::wstring, std::wstring, std::wstring> det_paths;
		det_paths = tnl::DetachmentFilePath(file_path);

		for (unsigned int i = 0; i < obj->GetNumSubsets(); i++) {
			OBJMATERIAL* materials = &obj->GetMaterials()[obj->GetSubsets()[i].materialIndex];
			unsigned int* indexs = &obj->GetIndices()[obj->GetSubsets()[i].faceStart];
			OBJVERTEX* vertexs = obj->GetVertices();

			unsigned int vertex_num = obj->GetSubsets()[i].faceCount;
			unsigned int index_num = vertex_num;
			unsigned int triangle_num = vertex_num / 3;

			if (0 == vertex_num) continue;

			Shared<Mesh> mesh = Shared<Mesh>(new Mesh());
			mesh->vtxs_.resize(vertex_num);
			mesh->idxs_.resize(index_num);
			mesh->shape_type_ = Mesh::eShapeType::RESOUCE;
			mesh->mv1_base_scale_ = base_scale;
			mesh->desc_ = std::make_shared<MeshDescResouce>(file_path, 1.0f);

			OBJMATERIAL* mtl = &obj->GetMaterials()[obj->GetSubsets()[i].materialIndex];

			mesh->render_param_.dxlib_mtrl_.Ambient = { mtl->ambient.x, mtl->ambient.y, mtl->ambient.z };
			mesh->render_param_.dxlib_mtrl_.Diffuse = { mtl->diffuse.x, mtl->diffuse.y, mtl->diffuse.z };
			mesh->render_param_.dxlib_mtrl_.Specular = { mtl->specular.x, mtl->specular.y, mtl->specular.z };


			if (!std::string(mtl->ambientMapName).empty()) {
				size_t converted_chars;
				wchar_t uText[256] = { 0 };
				mbstowcs_s(&converted_chars, uText, 256, mtl->ambientMapName, _TRUNCATE);
				Shared<dxe::Texture> tex_ambi = dxe::Texture::CreateFromFile((std::get<0>(det_paths) + L"/" + std::wstring(uText)).c_str());
				mesh->setTexture(tex_ambi, dxe::Mesh::eTextureSlot::AMIBENT);
			}
			if (!std::string(mtl->specularMapName).empty()) {
				size_t converted_chars;
				wchar_t uText[256] = { 0 };
				mbstowcs_s(&converted_chars, uText, 256, mtl->specularMapName, _TRUNCATE);
				Shared<dxe::Texture> tex_spec = dxe::Texture::CreateFromFile((std::get<0>(det_paths) + L"/" + std::wstring(uText)).c_str());
				mesh->setTexture(tex_spec, dxe::Mesh::eTextureSlot::SPECULAR);
			}
			if (!std::string(mtl->bumpMapName).empty()) {
				size_t converted_chars;
				wchar_t uText[256] = { 0 };
				mbstowcs_s(&converted_chars, uText, 256, mtl->bumpMapName, _TRUNCATE);
				Shared<dxe::Texture> tex_bump = dxe::Texture::CreateFromFile((std::get<0>(det_paths) + L"/" + std::wstring(uText)).c_str());
				mesh->setTexture(tex_bump, dxe::Mesh::eTextureSlot::BUMP);
			}
			if (!std::string(mtl->diffuseMapName).empty()) {
				size_t converted_chars;
				wchar_t uText[256] = { 0 };
				mbstowcs_s(&converted_chars, uText, 256, mtl->diffuseMapName, _TRUNCATE);
				Shared<dxe::Texture> tex_diff = dxe::Texture::CreateFromFile((std::get<0>(det_paths) + L"/" + std::wstring(uText)).c_str());
				mesh->setTexture(tex_diff, dxe::Mesh::eTextureSlot::DIFFUSE);
			}
			else {
				Shared<dxe::Texture> tex_diff = dxe::Texture::CreateFromFile(L"graphics/white.bmp");
				mesh->setTexture(tex_diff, dxe::Mesh::eTextureSlot::DIFFUSE);
			}
			if (!std::string(mtl->name).empty()) {
				size_t converted_chars;
				wchar_t uText[256] = { 0 };
				mbstowcs_s(&converted_chars, uText, 256, mtl->name, _TRUNCATE);
				mesh->mesh_name_ = std::wstring(uText);
			}

			int tw, th;
			int bw, bh;
			GetGraphSize(mesh->getTexture()->getDxLibGraphHandle(), &tw, &th);
			tnl::MostBit32(tw, bw);
			tnl::MostBit32(th, bh);
			int tw2 = 1 << (bw + 1);
			int th2 = 1 << (bh + 1);

			float ru = (float)tw / (float)tw2;
			float rv = (float)th / (float)th2;

			for (unsigned int k = 0; k < triangle_num; ++k) {
				for (unsigned int n = 0; n < 3; ++n) {
					int a = (k * 3) + n;
					float px = vertexs[indexs[a]].position.x ;
					float py = vertexs[indexs[a]].position.y ;
					float pz = vertexs[indexs[a]].position.z ;
					float nx = vertexs[indexs[a]].normal.x;
					float ny = vertexs[indexs[a]].normal.y;
					float nz = vertexs[indexs[a]].normal.z;
					float u = vertexs[indexs[a]].texcoord.x;
					float v = 1.0f - vertexs[indexs[a]].texcoord.y;
					mesh->idxs_[a] = a;
					mesh->vtxs_[a].pos = { px, py, pz };
					mesh->vtxs_[a].norm = { nx, ny, nz };
					mesh->vtxs_[a].su = 0 ;
					mesh->vtxs_[a].sv = 0 ;
					mesh->vtxs_[a].u = u * ru;
					mesh->vtxs_[a].v = v * rv;
					mesh->vtxs_[a].dif = GetColorU8(255, 255, 255, 255);
					mesh->vtxs_[a].spc = GetColorU8(255, 255, 255, 255);
				}
			}

			mesh->bd_sphere_radius_ = obj->GetSphere().radius * base_scale;
			mesh->bd_box_size_.x = (obj->GetBox().maximum.x - obj->GetBox().minimum.x) * base_scale;
			mesh->bd_box_size_.y = (obj->GetBox().maximum.y - obj->GetBox().minimum.y) * base_scale;
			mesh->bd_box_size_.z = (obj->GetBox().maximum.z - obj->GetBox().minimum.z) * base_scale;

			mesh->createVBO();
			create_meshs.emplace_back(mesh);
		}

		delete obj;

		return create_meshs;
	}

	std::vector<Shared<Mesh>> Mesh::CreateFromFileObjMV(const std::wstring& file_path, const float base_scale) {
		std::vector<Shared<Mesh>> objs = CreateFromFileObj(file_path, base_scale);
		std::vector<Shared<Mesh>> meshs;
		std::vector<tnl::Matrix> mat;
		mat.emplace_back(tnl::Matrix());
		meshs.resize(objs.size());
		for (size_t i = 0; i < objs.size(); ++i) {
			meshs[i] = CreateStaticMeshGroupMV(objs[i], mat);
			meshs[i]->setTexture(objs[i]->getTexture());
			meshs[i]->shape_type_ = eShapeType::RESOUCE;
			meshs[i]->desc_ = objs[i]->getCreateDesc()->createClone() ;
			meshs[i]->mv1_base_scale_ = base_scale;
		}
		return meshs;
	}

}
