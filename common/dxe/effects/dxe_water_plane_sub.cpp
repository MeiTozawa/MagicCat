#pragma warning(disable: 4010)
#pragma warning(disable: 4819)
#pragma warning(disable: 4828)
#include <DxLib.h>
#include "../../tnl/tnl_input.h"
#include "../dxe_build_setting.h"
#include "../dxe_prototype.h"
#include "dxe_water_plane.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"


namespace dxe {

    DXE_PROTOTYPE_GET_WINDOW_WIDTH;
    DXE_PROTOTYPE_GET_WINDOW_WIDTH_F;
    DXE_PROTOTYPE_GET_WINDOW_HEIGHT;
    DXE_PROTOTYPE_GET_WINDOW_HEIGHT_F;
    DXE_PROTOTYPE_GET_OPEN_READ_FILE_PATH;
    DXE_PROTOTYPE_GET_OPEN_SAVE_FILE_PATH;


#define SLIDER(ady, field, type, get, set, mmin, mmax, lavel, tips) \
        y += ady ;\
        if (!field) {\
        field = Shared<GuiValueSlider< WaterPlane, type >>(\
            new GuiValueSlider< WaterPlane, type >\
            (this\
                , &WaterPlane::##get\
                , &WaterPlane::##set\
                , { pos.x, pos.y + y }\
                , mmin\
                , mmax\
        , lavel));\
        field->setDescription([&]() {\
            description_str_ = tips;\
            });\
        }\

#define INPUT_BOX(type, field, func, box_str, lavel, tips)\
        y += height;\
        if (!field) {\
            field = Shared<GuiInputBox>(\
                new GuiInputBox\
                (GuiInputBox::eInputType::##type, func, 5));\
            field->setPosition({ pos.x + x, pos.y + y });\
            field->setBoxString(box_str);\
            field->setLabel(lavel);\
            field->setSize({ 250, 20 });\
            field->setBoxWidth(101);\
            field->setDescription([&]() {\
                description_str_ = tips;\
                });\
        }\


#define IMGUI_COMBO(item_width, imgui_type, origin_type, label, menu, get_func, set_func, desc)\
{\
    imgui_type value = (imgui_type)get_func();\
    const char* menus = menu;\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::Combo(label, &value, menus);\
    set_func((origin_type)value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
}\

#define IMGUI_SLIDER_INT(item_width, label, vmin, vmax, get_func, set_func, desc)\
{\
    int value = get_func();\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::SliderInt(label, &value, vmin, vmax);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func(value);\
}\

#define IMGUI_SLIDER_FLOAT(item_width, label, vmin, vmax, get_func, set_func, desc)\
{\
    float value = get_func();\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::SliderFloat(label, &value, vmin, vmax);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func(value);\
}\

#define IMGUI_INPUT_FLOAT(item_width, label, get_func, set_func, desc)\
{\
    float value = get_func();\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::InputFloat(label, &value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func(value);\
}\

#define IMGUI_INPUT_FLOAT2(item_width, label, get_func, set_func, desc)\
{\
    float value[2] = { TNL_DEP_V2(get_func()) };\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::InputFloat2(label, value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func({ value[0], value[1] });\
}\

#define IMGUI_INPUT_FLOAT3(item_width, label, get_func, set_func, desc)\
{\
    float value[3] = { TNL_DEP_V3(get_func()) };\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::InputFloat3(label, value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func({ value[0], value[1], value[2] });\
}\

#define IMGUI_INPUT_INT(item_width, label, get_func, set_func, desc)\
{\
    int value = get_func();\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::InputInt(label, &value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func(value);\
}\

#define IMGUI_INPUT_INT2(item_width, label, get_func, set_func, desc)\
{\
    int value[2] = { TNL_DEP_V2(get_func()) };\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::InputInt2(label, value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func({ value[0], value[1] });\
}\

#define IMGUI_INPUT_INT3(item_width, label, get_func, set_func, desc)\
{\
    int value[3] = { TNL_DEP_V3(get_func()) };\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::InputInt3(label, value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func({ value[0], value[1], value[2] });\
}\

#define IMGUI_COLOR_EDIT3(item_width, label, get_func, set_func, desc)\
{\
    float value[3] = { TNL_DEP_V3(get_func()) };\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::ColorEdit3(label, value);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func({ value[0], value[1], value[2] });\
}\

#define IMGUI_CHECK_BOX(item_width, label, get_func, set_func, desc)\
{\
    bool flag = get_func();\
    ImGui::SetNextItemWidth(item_width);\
    ImGui::Checkbox(label, &flag);\
    if (ImGui::IsItemHovered()) {\
        description_str_ = desc;\
    }\
    set_func(flag);\
}\


    void WaterPlane::drawImGuiController(const tnl::Vector2i& pos, bool is_window_opened, const char* window_label) {

        std::string label = "Parteicle Setting";
        if (window_label) label += "( " + std::string(window_label) + " )";

        float item_width = 180.0f;
        bool adoption = false;
        description_str_.clear();

        //  新しいウィンドウの作成
        ImGui::StyleColorsClassic();
        ImGui::SetNextWindowCollapsed(is_window_opened, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2((float)pos.x, (float)pos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(470, 690));
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin(label.c_str(), nullptr, ImGuiWindowFlags_NoResize);

        if (ImGui::Button("save"))
        {
            std::wstring path = dxe::GetOpenSaveFilePath(L"wpln", L"water_plane.wpln", save_path_);
            if (!path.empty()) {
                saveStatus(path);
                auto tpl = tnl::DetachmentFilePath(path);
                save_path_ = std::get<0>(tpl);
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("load")) {
            std::wstring path = dxe::GetOpenReadFilePath(L"*.wpln", load_path_);
            if (!path.empty()) {
                loadStatus(path);
                auto tpl = tnl::DetachmentFilePath(path);
                load_path_ = std::get<0>(tpl);
            }
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 60);
        if (ImGui::Button("LoadPatternTexture")) {
            std::wstring path = dxe::GetOpenReadFilePath(L"*.bmp;*.png;*.jpg;*.tga", pat_tex_path_);
            if (!path.empty()) {
                loadPatternTexture(path);
                auto tpl = tnl::DetachmentFilePath(path);
                pat_tex_path_ = std::get<0>(tpl);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("LoadDecorationTexture")) {
            std::wstring path = dxe::GetOpenReadFilePath(L"*.bmp;*.png;*.jpg;*.tga", dec_tex_path_);
            if (!path.empty()) {
                loadDecorationTexture(path);
                auto tpl = tnl::DetachmentFilePath(path);
                dec_tex_path_ = std::get<0>(tpl);
            }
        }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 150);
        auto srv = tex_pattern_->getShaderResourceView().Get();
        ImGui::Image((ImTextureID)(intptr_t)(srv), ImVec2(130, 100));

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
        srv = tex_decoration_->getShaderResourceView().Get();
        ImGui::Image((ImTextureID)(intptr_t)(srv), ImVec2(130, 100));

        ImGui::SetCursorPosY(50);

        // "乗算合成されるパターン柄のテクスチャ ※ UVスクロールされます"
        IMGUI_CHECK_BOX(item_width, "PatternTexture", getUsePatternTexture, setUsePatternTexture, L"乗算合成されるパターン柄のテクスチャを使用するか ※ UVスクロールされます");

        //"装飾テクスチャ"
        IMGUI_CHECK_BOX(item_width, "DecorationTexture", getUseDecorationTexture, setUseDecorationTexture, L"装飾テクスチャを使用するか");

        //"内部生成されるオブジェクトの接地テクスチャ　※ 適用されるオブジェクトの登録処理が必要です"
        IMGUI_CHECK_BOX(item_width, "LandingProjection", getUseLandingProjectionTexture, setUseLandingProjectionTexture, L"内部生成されるオブジェクトの接地テクスチャを使用するか　※ 適用されるオブジェクトの登録処理が必要です");

        //"内部生成されるオブジェクト接地によるくり抜きテクスチャ　※ 適用されるオブジェクトの登録処理が必要です"
        IMGUI_CHECK_BOX(item_width, "LandingCutOut", getUseLandingCutOutTexture, setUseLandingCutOutTexture, L"内部生成されるオブジェクト接地によるくり抜きテクスチャを使用するか　※ 適用されるオブジェクトの登録処理が必要です");

        //"内部生成される環境マップテクスチャ　※ 適用するにはキューブマップへの書き込み処理が必要です";
        IMGUI_CHECK_BOX(item_width, "CubeMapTexture", getUseCubeMapTexture, setUseCubeMapTexture, L"内部生成される環境マップテクスチャ　※ 適用するにはキューブマップへの書き込み処理が必要です");

        // "頂点を動的に動かす最大高"
        IMGUI_SLIDER_FLOAT(item_width, "VertexMoveHeight", 0.0f, 10.0f, getHeightMax, setHeightMax, L"頂点を動的に動かす最大高");

        // "パターンテクスチャの色合い係数"
        IMGUI_SLIDER_FLOAT(item_width, "PatternColorFactor", 0.0f, 3.0f, getPatternColFactor, setPatternColFactor, L"パターンテクスチャの色合い係数");

        // "装飾テクスチャの色合い係数"
        IMGUI_SLIDER_FLOAT(item_width, "DecoratColorFactor", 0.0f, 1.0f, getDecorationColFactor, setDecorationColFactor, L"装飾テクスチャの色合い係数");

        // "接地テクスチャの色合い係数"
        IMGUI_SLIDER_FLOAT(item_width, "LandingColorFactor", 0.0f, 3.0f, getLandingProjectionColFactor, setLandingProjectionColFactor, L"接地テクスチャの色合い係数");

        // "接地テクスチャの色合い絞り係数"
        IMGUI_SLIDER_FLOAT(item_width, "LandingColorPow", 0.0f, 2.0f, getLandingProjectionColPow, setLandingProjectionColPow, L"接地テクスチャの色合い絞り係数");

        // "基本色"
        IMGUI_COLOR_EDIT3(item_width, "BaseColor", getBaseColor, setBaseColor, L"基本色");

        // "鏡面反射光"
        IMGUI_COLOR_EDIT3(item_width, "Specular", getSpecularColor, setSpecularColor, L"鏡面反射光");

        // "影響を受ける平行光源"
        IMGUI_INPUT_FLOAT3(item_width, "LightDirection", getLightDirection, setLightDirection, L"影響を受ける平行光源");

        // "基本色の強さ"
        IMGUI_SLIDER_FLOAT(item_width, "BaseColorStrength", 0.0f, 1.0f, getBaseColorStrength, setBaseColorStrength, L"基本色の強さ");

        // "暗さコントラスト"
        IMGUI_SLIDER_FLOAT(item_width, "DarknessContrast", 0.0f, 10.0f, getDarknessContrast, setDarknessContrast, L"暗さコントラスト");

        // "スペキュラ係数"
        IMGUI_SLIDER_FLOAT(item_width, "SpecularPower", 0.0f, 1000.0f, getSpecularPower, setSpecularPower, L"スペキュラ係数");

        // "光沢係数"
        IMGUI_SLIDER_FLOAT(item_width, "Shininess", 0.0f, 1.0f, getShininess, setShininess, L"光沢係数");

        // "環境反射係数"
        IMGUI_SLIDER_FLOAT(item_width, "FresnelFactor", 0.0f, 1.0f, getFresnelFactor, setFresnelFactor, L"環境反射係数");

        // "環境反射による透過率下限値"
        IMGUI_SLIDER_FLOAT(item_width, "FresnelBaseAlpha", 0.0f, 1.0f, getFresnelBaseAlpha, setFresnelBaseAlpha, L"環境反射による透過率下限値");

        // "ノイズ係数"
        IMGUI_SLIDER_FLOAT(item_width, "NoizeStrength", 0.0f, 5.0f, getNoizeStrength, setNoizeStrength, L"ノイズ係数");

        // "波の荒れ具合に関する係数"
        IMGUI_SLIDER_FLOAT(item_width, "BaseAmpFactor", 0.0f, 3.0f, getBaseAmpFactor, setBaseAmpFactor, L"波の荒れ具合に関する係数");

        // "波の荒れ具合に関する係数( キューブマップ用 )"
        IMGUI_SLIDER_FLOAT(item_width, "CubeAmpFactor", 0.0f, 0.5f, getCubeAmpFactor, setCubeAmpFactor, L"波の荒れ具合に関する係数( キューブマップ用 )");

        // "波の周波数に関する係数"
        IMGUI_SLIDER_FLOAT(item_width, "FrequencyFactor", 0.0f, 2.0f, getFrequencyFactor, setFrequencyFactor, L"波の周波数に関する係数");

        // "勾配による色味の変化係数"
        IMGUI_SLIDER_FLOAT(item_width, "SteepnessFactor", 0.0f, 3.0f, getSteepnessFactor, setSteepnessFactor, L"勾配による色味の変化係数");

        // "波の分散性 : WaveDir::RANDAM では無効"
        //IMGUI_SLIDER_FLOAT(item_width, "WaveDecentralization", 0.0f, 0.5f, getWaveDecentralization, setWaveDecentralization, L"波の分散性 : WaveDir::RANDAM では無効");
        {
            float value = getWaveDecentralization();
            ImGui::SetNextItemWidth(item_width);
            ImGui::SliderFloat("WaveDecentralization", &value, 0.0f, 0.5f);
            if (ImGui::IsItemHovered()) {
                description_str_ = L"波の分散性 : WaveDir::RANDAM では無効";
            }
            if (fabs(value - getWaveDecentralization()) > FLT_EPSILON) {
                setWaveDecentralization(value);
            }
        }

        // "タイムスケール"
        IMGUI_SLIDER_FLOAT(item_width, "TimeScale", 0.0f, 3.0f, getTimeScale, setTimeScale, L"タイムスケール");

        // "波の向き"
        IMGUI_COMBO(item_width, int, eWaveDirection, "WaveDirection", "WaveDir::RANDOM\0WaveDir::LEFT\0WaveDir::RIGHT\0WaveDir::FORWARD\0WaveDir::BACK\0WaveDir::LEFT_FORWARD\0WaveDir::LEFT_BACK\0WaveDir::RIGHT_FORWARD\0WaveDir::RIGHT_BACK\0\0", getWaveDirection, setWaveDirection, L"波の向き");

        // "カメラ距離によるフェード設定 ※ FOG, ALPHA は DxLib のフォグ設定が適用されます"
        IMGUI_COMBO(item_width, int, eFarFade, "FarFade", "FarFade::NONE\0FarFade::FOG\0FarFade::ALPHA\0\0", getFarFade, setFarFade, L"カメラ距離によるフェード設定 ※ FOG, ALPHA は DxLib のフォグ設定が適用されます");

        // "ゲルストナー波のサンプリング数 ( D4 は低負荷低クォリティ )"
        IMGUI_COMBO(item_width, int, eGerstnerSampling, "GerstnerSampling", "GerstnerSampling::D4\0GerstnerSampling::D8\0\0", getGerstnerSampling, setGerstnerSampling, L"ゲルストナー波のサンプリング数 ( D4 は低負荷低クォリティ )");

        //"水面の横幅サイズ ※ 大きなサイズで cut out texture を有効にすると高負荷になります"
        IMGUI_COMBO(item_width, int, eSize, "WidthSize", "Width eSize::S32\0Width eSize::S64\0Width eSize::S128\0Width eSize::S256\0Width eSize::S512\0Width eSize::S1024\0Width eSize::S2048\0Width eSize::S4096\0Width eSize::S8192\0\0", getSizeWidth, setSizeWidth, L"水面の横幅サイズ ※ 大きなサイズで cut out texture を有効にすると高負荷になります");

        //"水面の奥行サイズ ※ 大きなサイズで cut out texture を有効にすると高負荷になります"
        IMGUI_COMBO(item_width, int, eSize, "DepthSize", "Depth eSize::S32\0Depth eSize::S64\0Depth eSize::S128\0Depth eSize::S256\0Depth eSize::S512\0Depth eSize::S1024\0Depth eSize::S2048\0Depth eSize::S4096\0Width eSize::S8192\0\0", getSizeDepth, setSizeDepth, L"水面の奥行サイズ ※ 大きなサイズで cut out texture を有効にすると高負荷になります");

        // メッシュの頂点分割数
        IMGUI_INPUT_INT(item_width, "VertexDivNum", getDivNum, setDivNum, L"メッシュの頂点分割数");

        //"tess patch num", "テッセレーションによるポリゴン分割数(最大60)"
        IMGUI_INPUT_FLOAT(item_width, "TessPatchNum", getPatchNum, setPatchNum, L"テッセレーションによるポリゴン分割数(最大60)");


        ImGui::End();

        if (!description_str_.empty()) {
            int tx = 0;
            int ty = GetWindowHeight() - 30;
            int bx = GetWindowWidth();
            int by = GetWindowHeight();
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
            DrawBox(tx, ty, bx, by, 0, true);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
            DrawFormatString(tx, ty + 5, -1, L"%s", description_str_.c_str());
        }
    }

}