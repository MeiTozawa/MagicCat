#pragma warning(disable: 4010)
#include "../dxe_prototype.h"
#include "dxe_mesh_base.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace dxe {

	DXE_PROTOTYPE_GET_OPEN_SAVE_FILE_PATH;
	DXE_PROTOTYPE_GET_OPEN_READ_FILE_PATH;

	//------------------------------------------------------------------------------------------------------------------------
	void MeshBase::loadMaterial(const std::wstring& file_path) {
		FILE* fp = nullptr;
		_wfopen_s(&fp, file_path.c_str(), L"rb");
		if (fp) {
			fread(&render_param_.dxlib_mtrl_.Diffuse.a, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Diffuse.r, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Diffuse.g, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Diffuse.b, sizeof(float), 1, fp);

			fread(&render_param_.dxlib_mtrl_.Ambient.a, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Ambient.r, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Ambient.g, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Ambient.b, sizeof(float), 1, fp);

			fread(&render_param_.dxlib_mtrl_.Emissive.a, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Emissive.r, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Emissive.g, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Emissive.b, sizeof(float), 1, fp);

			fread(&render_param_.dxlib_mtrl_.Specular.a, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Specular.r, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Specular.g, sizeof(float), 1, fp);
			fread(&render_param_.dxlib_mtrl_.Specular.b, sizeof(float), 1, fp);

			fread(&render_param_.dxlib_mtrl_.Power, sizeof(float), 1, fp);
			fclose(fp);
			setRenderParam(render_param_);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void MeshBase::saveMaterial(const std::wstring& file_path) {
		FILE* fp = nullptr;
		_wfopen_s(&fp, file_path.c_str(), L"wb");
		if (fp) {
			fwrite(&render_param_.dxlib_mtrl_.Diffuse.a, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Diffuse.r, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Diffuse.g, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Diffuse.b, sizeof(float), 1, fp);

			fwrite(&render_param_.dxlib_mtrl_.Ambient.a, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Ambient.r, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Ambient.g, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Ambient.b, sizeof(float), 1, fp);

			fwrite(&render_param_.dxlib_mtrl_.Emissive.a, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Emissive.r, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Emissive.g, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Emissive.b, sizeof(float), 1, fp);

			fwrite(&render_param_.dxlib_mtrl_.Specular.a, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Specular.r, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Specular.g, sizeof(float), 1, fp);
			fwrite(&render_param_.dxlib_mtrl_.Specular.b, sizeof(float), 1, fp);

			fwrite(&render_param_.dxlib_mtrl_.Power, sizeof(float), 1, fp);
			fclose(fp);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void MeshBase::drawImGuiMaterialController(const tnl::Vector2i& pos, bool is_window_opened, const char* window_label) {

		std::string label = "Mesh Material Setting";
		if (window_label) label += "( " + std::string(window_label) + " )";

		bool adoption = false;

		//  新しいウィンドウの作成
		ImGui::StyleColorsClassic();
		ImGui::SetNextWindowCollapsed(is_window_opened, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2((float)pos.x, (float)pos.y), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(400, 200));
		ImGui::Begin(label.c_str(), nullptr, ImGuiWindowFlags_NoResize);

		if (ImGui::Button("save"))
		{
			std::wstring path = dxe::GetOpenSaveFilePath(L"mtrl", L"material.mtrl");
			if (!path.empty()) {
				saveMaterial(path);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("load")) {
			std::wstring path = dxe::GetOpenReadFilePath(L"*.mtrl");
			if (!path.empty()) {
				loadMaterial(path);
			}
		}

		drawImGuiMaterialControllerParameters();

		ImGui::End();

	}

	//------------------------------------------------------------------------------------------------------------------------
	void MeshBase::drawImGuiMaterialControllerParameters() {
		float diff[3] = { TNL_DEP_V3(getMtrlDiffuse()) };
		float anmb[3] = { TNL_DEP_V3(getMtrlAmbient()) };
		float emis[3] = { TNL_DEP_V3(getMtrlEmissive()) };
		float spec[3] = { TNL_DEP_V3(getMtrlSpecular()) };
		float spec_pow = getMtrlSpecPower();

		ImGui::ColorEdit3("Diffuse", diff);
		ImGui::ColorEdit3("Amibient", anmb);
		ImGui::ColorEdit3("Emissive", emis);
		ImGui::ColorEdit3("Specular", spec);
		ImGui::SliderFloat("SpecularPower", &spec_pow, 0, 100);

		setMtrlDiffuse({ diff[0], diff[1], diff[2] });
		setMtrlAmbient({ anmb[0], anmb[1], anmb[2] });
		setMtrlEmissive({ emis[0], emis[1], emis[2] });
		setMtrlSpecular({ spec[0], spec[1], spec[2] });
		setMtrlSpecPower(spec_pow);
	}


}