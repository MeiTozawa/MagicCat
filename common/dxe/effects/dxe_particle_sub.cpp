#pragma warning(disable: 4010)
#pragma warning(disable: 4819)
#pragma warning(disable: 4828)
#include <DxLib.h>
#include "../../tnl/tnl_input.h"
#include "../dxe_build_setting.h"
#include "../dxe_prototype.h"
#include "dxe_particle.h"
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


    //---------------------------------------------------------------------------------------------------------------------------------------------
    void Particle::drawImGuiController(const tnl::Vector2i& pos, bool is_window_opened, const char* window_label) {

        std::string label = "Parteicle Setting";
        if (window_label) label += "( " + std::string(window_label) + " )";

        float item_width = 155.0f;
        bool adoption = false;
        description_str_.clear();

        //  新しいウィンドウの作成
        ImGui::StyleColorsClassic();
        ImGui::SetNextWindowCollapsed(is_window_opened, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2((float)pos.x, (float)pos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(600, 690));
        ImGui::SetNextWindowBgAlpha(0.4f);
        ImGui::Begin(label.c_str(), nullptr, ImGuiWindowFlags_NoResize);

        if (ImGui::Button("save"))
        {
            std::wstring path = dxe::GetOpenSaveFilePath(L"ptcl", L"particle.ptcl", save_path_);
            if (!path.empty()) {
                saveStatus(path);
                auto tpl = tnl::DetachmentFilePath(path);
                save_path_ = std::get<0>(tpl);
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("load")) {
            std::wstring path = dxe::GetOpenReadFilePath(L"*.ptcl", load_path_);
            if (!path.empty()) {
                loadStatus(path);
                reloadTexture();
                refreshProcess();
                auto tpl = tnl::DetachmentFilePath(path);
                load_path_ = std::get<0>(tpl);
            }
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(300);
        if (ImGui::Button("texture")) {
            std::wstring path = dxe::GetOpenReadFilePath(L"*.bmp;*.png;*.jpg;*.tga", texture_path_);
            if (!path.empty()) {
                texture_ = dxe::Texture::CreateFromFile(path, true);
                reloadTexture();
                auto tpl = tnl::DetachmentFilePath(path);
                texture_path_ = std::get<0>(tpl);
            }
        }


        float ofs_x = 300;
        auto srv = texture_->getShaderResourceView().Get();
        ImGui::SetCursorPosX(ofs_x);
        ImGui::Image((ImTextureID)(intptr_t)(srv), ImVec2(100, 100));
        //ImGui::Image()


        // [ LOOP : 時間に対して均等に排出されます ] [ ONCE :１度に全ての粒子が排出され、再度再生命令があるまで排出されません ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, ePlayType, "PlayType", "LOOP\0ONCE\0\0", getPlayType, setPlayType, L"[ LOOP : 時間に対して均等に排出されます ] [ ONCE :１度に全ての粒子が排出され、再度再生命令があるまで排出されません ]");

        // [ NONE : 通常合成 ] [ ADD : 加算 ] [ SUB : 減算 ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, eEdgeEmphasis, "EdgeEmphasis", "EdgeEmphasis::NONE\0EdgeEmphasis::ADD\0EdgeEmphasis::SUB\0\0", getEdgeEmphasis, setEdgeEmphasis, L"[ NONE : 縁の強調合成なし ] [ ADD : 加算で縁の強調を行う ] [ SUB : 減算で縁の強調を行う ] ※ NONE 以外は BlendMode が無効になります");

        // [ ALPHA : 通常の半透明 ] [ ADD : 加算 ] [ SUB : 減算 ] [ MUL : 乗算 ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, eBlendState, "BlendMode", "BlendMode::ALPHA\0BlendMode::ADD\0BlendMode::SUB\0BlendMode::MUL\0\0", getBlendMode, setBlendMode, L"[ ALPHA : 通常の半透明 ] [ ADD : 加算 ] [ SUB : 減算 ] [ MUL : 乗算 ]");

        // 出現範囲 [ XYZ : 球内範囲 ] [ XY, XZ, YZ : 平面の円内範囲 ] [ AXIS : 軸上 ] [ SPHERE : 球面上 ] [ CXY, CXZ, CYZ 平面の円上 ] OrigineRange で調整
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, eOriginMode, "OriginMode", "OriginMode::XYZ\0OriginMode::XY\0OriginMode::XZ\0OriginMode::YZ\0OriginMode::AXIS\0OriginMode::SPHERE\0OriginMode::CXY\0OriginMode::CXZ\0OriginMode::CYZ\0\0", getOriginMode, setOriginMode, L"出現範囲 [ XYZ : 球内範囲 ] [ XY, XZ, YZ : 平面の円内範囲 ] [ AXIS : 軸上 ] [ SPHERE : 球面上 ] [ CXY, CXZ, CYZ 平面の円上 ] OrigineRange で調整");

        // 発生した粒子が拡散するか収束するか [ DIFF : 拡散 ] [ CONV : 収束 ] [ ICONV : 本来の収束点から逆方向へ ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, eEjectType, "EjectType", "EjectType::DIFF\0EjectType::CONV\0EjectType::ICONV\0\0", getEjectType, setEjectType, L"発生した粒子が拡散するか収束するか [ DIFF : 拡散 ] [ CONV : 収束 ] [ ICONV : 本来の収束点から逆方向へ ]");

        // 粒子の移動 [ XYZ : 全方位 ] [ XY, XZ, YZ : 平面 ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, eMoveSurface, "MoveSurface", "MoveSurface::XYZ\0MoveSurface::XY\0MoveSurface::XZ\0MoveSurface::YZ\0\0", getMoveSurface, setMoveSurface, L"粒子の移動 [ XYZ : 全方位 ] [ XY, XZ, YZ : 平面 ]");

        // 粒子の向き [ BXYZ:ビルボード ] [ BY:Y軸ビルボード ] [ OBY:上向Y軸ビルボード ] [ UpYD:移動方向に沿うYアップ ] [ X, Y, Z:向軸固定 ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, ePostureType, "PostureType", "PostureType::BXYZ\0PostureType::BY\0PostureType::OBY\0PostureType::UpYD\0PostureType::X\0PostureType::Y\0PostureType::Z\0\0", getPostureType, setPostureType, L"粒子の向き [ BXYZ:ビルボード ] [ BY:Y軸ビルボード ] [ OBY:上向Y軸ビルボード ] [ UpYD:移動方向に沿うYアップ ] [ X, Y, Z:向軸固定 ]");

        // 出現後の粒子が Position に追従するか
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, bool, "AdulationEmitter", "FALSE\0TRUE\0\0", getAdulationEmitter, setAdulationEmitter, L"出現後の粒子が Position に追従するか");

        // 出現後のサイズ変位 [ NONE : なし ] [ EXIST : 出現直後と消える直前にサイズ変化 ] [ BIGGER : 徐々に大きく ] [ SMALLER : 徐々に小さく ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, eDispSizeMode, "DispSizeMode", "DispSizeMode::NONE\0DispSizeMode::EXIST\0DispSizeMode::BIGGER\0DispSizeMode::SMALLER\0\0", getDispSizeMode, setDispSizeMode, L"出現後のサイズ変位 [ NONE : なし ] [ EXIST : 出現直後と消える直前にサイズ変化 ] [ BIGGER : 徐々に大きく ] [ SMALLER : 徐々に小さく ]");

        // 出現後の透明度変位 [ NONE : なし ] [ EXIST : 出現直後と消える直前に透明度変化 ] [ INC : 徐々に濃く ] [ DEC : 徐々に薄く ]
        ImGui::SetCursorPosX(ofs_x);
        IMGUI_COMBO(item_width, int, eDispAlphaMode, "DispAlphaMode", "DispAlphaMode::NONE\0DispAlphaMode::EXIST\0DispAlphaMode::INC\0DispAlphaMode::DEC\0\0", getDispAlphaMode, setDispAlphaMode, L"出現後の透明度変位 [ NONE : なし ] [ EXIST : 出現直後と消える直前に透明度変化 ] [ INC : 徐々に濃く ] [ DEC : 徐々に薄く ]");

        // 形状変化のモード [ CHAOS : 頂点位置が交差したり混沌とした変化をします ] [ ORDER : 頂点は交差せず秩序立った変化をします ]
        //IMGUI_COMBO(item_width, int, eDistortionMode, "DistortionMode", "DistortionMode::CHAOS\0DistortionMode::ORDER\0\0", getDistortionMode, setDistortionMode, L"形状変化のモード [ CHAOS : 頂点位置が交差したり混沌とした変化をします ] [ ORDER : 頂点は交差せず秩序立った変化をします ]");

        ImGui::SetCursorPosY(50);

        // 粒子の数
        IMGUI_SLIDER_INT(item_width, "ParticleNum", 1, 10000, getParticleNum, setParticleNum, L"粒子の数");

        // 生存時間
        IMGUI_SLIDER_FLOAT(item_width, "LifeTime", 0.1f, 60.0f, getTimeLimit, setTimeLimit, L"粒子が生まれてから消えるまでの時間( 秒 )");

        // 発生範囲
        IMGUI_SLIDER_FLOAT(item_width, "OriginRange", 0, 10000.0f, getOriginRange, setOriginRange, L"粒子が発生する範囲 OriginMode で球状や円状にできます");

        // 移動のゆらぎ係数
        IMGUI_SLIDER_FLOAT(item_width, "WaveFactor", 0, 1000.0f, getWaverFactor, setWaverFactor, L"粒子が移動する際の揺らぎの強さ係数");

        // direction 方向へ放出される粒子の速度 EjectType::CONV では無効
        IMGUI_SLIDER_FLOAT(item_width, "StartVelocity", 0.1f, 500.0f, getStartVelocity, setStartVelocity, L"direction 方向へ放出される粒子の速度 EjectType::CONV では無効");

        //重力係数 EjectType::COMV では無効
        IMGUI_SLIDER_FLOAT(item_width, "GravityFactor", 0, 20.0f, getGravityFactor, setGravityFactor, L"重力係数 EjectType::COMV では無効");

        // エミッター座標
        IMGUI_INPUT_FLOAT3(item_width, "Position(xyz)", getPosition, setPosition, L"エミッター座標");

        //粒子の収束座標 EjectTypeをCONV, ICONVで有効 この座標はPositionからの相対座標です
        IMGUI_INPUT_FLOAT3(item_width, "ConvPosition(xyz)", getConvPosition, setConvPosition, L"粒子の収束座標 EjectType を CONV, ICONV で有効 この座標は Position からの相対座標です");

        // 自己発光色
        IMGUI_COLOR_EDIT3(item_width, "EmissiveColor", getEmissive, setEmissive, L"自己発光色");

        // 拡散方向 EjectTypeをDIFFに設定したときのみ有効
        IMGUI_INPUT_FLOAT3(item_width, "DiffDir(xyz)", getDiffDirection, setDiffDirection, L"拡散方向 EjectTypeをDIFFに設定したときのみ有効");

        // 重力方向 EjectType::CONV では無効
        IMGUI_INPUT_FLOAT3(item_width, "GravityDir(xyz)", getGravityDirection, setGravityDirection, L"重力方向 EjectType::CONV では無効");

        // 粒子の生成軸
        IMGUI_INPUT_FLOAT3(item_width, "OriginAxis(xyz)", getOriginAxis, setOriginAxis, L"粒子の生成軸  ※ OriginMode を Axis に設定したときに有効");

        // 全体の時間スケール 最終的な調整に使用する事を推奨
        IMGUI_SLIDER_FLOAT(item_width, "TimeScale", 0.01f, 10.0f, getTimeScale, setTimeScale, L"全体の時間スケール 最終的な調整に使用する事を推奨");

        // 全体の透明度
        IMGUI_SLIDER_FLOAT(item_width, "Alpha", 0.0f, 1.0f, getAlpha, setAlpha, L"全体の透明度");

        // カメラに近い場合に透明化する距離
        IMGUI_SLIDER_FLOAT(item_width, "AlphaDistance", 0.0f, 100.0f, getNearAlphaDistance, setNearAlphaDistance, L"カメラに近い場合に透明化する距離");

        // 粒子のサイズ
        IMGUI_INPUT_FLOAT2(item_width, "Size(xy)", getSize, setSize, L"粒子のサイズ大きいほど処理速度に影響を与えるので注意");

        // 放出された粒子の速度の減衰係数 EjectType::CONV では無効
        IMGUI_SLIDER_FLOAT(item_width, "MovingDecayFactor", 0.0f, 20.0f, getMovingDecayFactor, setMovingDecayFactor, L"放出された粒子の速度の減衰係数 EjectType::CONV では無効");

        // diffusion dir から 一定の範囲角度を付けて拡散します EjectTypeをDIFFに設定したときのみ有効
        IMGUI_SLIDER_FLOAT(item_width, "DiffRandomAngleRange", 0.0f, 180.0f, getDiffRandomAngleRange, setDiffRandomAngleRange, L"DiffDirection から 一定の範囲角度を付けて拡散します EjectTypeをDIFFに設定したときのみ有効");

        // 輝度の揺らぎ係数
        IMGUI_SLIDER_FLOAT(item_width, "LuminanceWaveFactor", 0.0f, 30.0f, getLuminanceWaveFactor, setLuminanceWaveFactor, L"輝度の揺らぎ係数");

        // 回転係数 個々の粒子が指定値を最大としたランダムな回転をします PostureType::UpYD では適用されません
        IMGUI_SLIDER_FLOAT(item_width, "RotateFactor", 0.0f, 10.0f, getRotateFactor, setRotateFactor, L"回転係数 個々の粒子が指定値を最大としたランダムな回転をします PostureType::UpYD では適用されません");

        // 形状の歪み係数 DistortionMode の設定と連携します
        IMGUI_SLIDER_FLOAT(item_width, "DistortionRange", 0.0f, 10.0f, getDistortionRange, setDistortionRange, L"形状の歪み係数 DistortionMode の設定と連携します");

        // 形状の歪みの速さ係数 DistortionMode の設定と連携します
        IMGUI_SLIDER_FLOAT(item_width, "DistortionVelocity", 0.0f, 10.0f, getDistortionVelocity, setDistortionVelocity, L"形状の歪みの速さ係数 DistortionMode の設定と連携します");

        // 縁の強調係数
        IMGUI_SLIDER_FLOAT(item_width, "EdgeEmphasisFactor", 0.0f, 20.0f, getEdgeEmphasisFactor, setEdgeEmphasisFactor, L"縁の強調係数 ※ EdgeEmphasis::NONE では無効");


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

