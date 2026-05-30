#pragma once
#include <wchar.h>
#include <vector>
#include <unordered_map>



//------------------------------------------------------------------------------------------------------------------
// 
// file path 
// 


constexpr const wchar_t* FILE_PATH_CSV_EXAMPLE = L"resource/csv/example.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_EXAMPLE_MAP_CHIP = L"resource/csv/example_map_chip.csv"; 

constexpr const wchar_t* FILE_PATH_PNG_MAP1_ANIM_CHIPS_32 = L"resource/dxe_map_editor_data/sample/map1/map1_anim_chips_32.png"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_ANIM_CHIP_INFO_32 = L"resource/dxe_map_editor_data/sample/map1/map1_anim_chip_info_32.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_COLLISION_LAYER_0 = L"resource/dxe_map_editor_data/sample/map1/map1_collision_layer_0.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_0 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_0.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_1 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_1.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_2 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_2.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_3 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_3.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_4 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_4.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_5 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_5.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_6 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_6.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_7 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_7.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_8 = L"resource/dxe_map_editor_data/sample/map1/map1_landscape_layer_8.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_LAYER_SETTINGS = L"resource/dxe_map_editor_data/sample/map1/map1_layer_settings.csv"; 
constexpr const wchar_t* FILE_PATH_PNG_MAP1_STIL_CHIPS_128 = L"resource/dxe_map_editor_data/sample/map1/map1_stil_chips_128.png"; 
constexpr const wchar_t* FILE_PATH_PNG_MAP1_STIL_CHIPS_32 = L"resource/dxe_map_editor_data/sample/map1/map1_stil_chips_32.png"; 
constexpr const wchar_t* FILE_PATH_PNG_MAP1_STIL_CHIPS_96 = L"resource/dxe_map_editor_data/sample/map1/map1_stil_chips_96.png"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_128 = L"resource/dxe_map_editor_data/sample/map1/map1_stil_chip_info_128.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_32 = L"resource/dxe_map_editor_data/sample/map1/map1_stil_chip_info_32.csv"; 
constexpr const wchar_t* FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_96 = L"resource/dxe_map_editor_data/sample/map1/map1_stil_chip_info_96.csv"; 

constexpr const wchar_t* FILE_PATH_BIN_DIRECTIONAL_LIGHT_PARAMETER = L"resource/dxe_parameters/directional_light/directional_light_parameter.bin"; 

constexpr const wchar_t* FILE_PATH_BIN_GRAPHICS_LAYER_STATUS = L"resource/dxe_parameters/graphics_layer/graphics_layer_status.bin"; 

constexpr const wchar_t* FILE_PATH_BIN_MATERIAL = L"resource/dxe_parameters/mesh_material/material.bin"; 

constexpr const wchar_t* FILE_PATH_BIN_CONVERGENCE = L"resource/dxe_parameters/particle/preset/convergence.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_DEFAULT = L"resource/dxe_parameters/particle/preset/default.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_EXPLOSION = L"resource/dxe_parameters/particle/preset/explosion.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_FIRE = L"resource/dxe_parameters/particle/preset/fire.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_FIREWORKS = L"resource/dxe_parameters/particle/preset/fireworks.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_RAIN = L"resource/dxe_parameters/particle/preset/rain.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_RAIN_GROUND = L"resource/dxe_parameters/particle/preset/rain_ground.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_SMOKE = L"resource/dxe_parameters/particle/preset/smoke.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_SNOW = L"resource/dxe_parameters/particle/preset/snow.bin"; 
constexpr const wchar_t* FILE_PATH_BIN_WATER_FALL = L"resource/dxe_parameters/particle/preset/water_fall.bin"; 

constexpr const wchar_t* FILE_PATH_BIN_SKYBOX_MATERIAL = L"resource/dxe_parameters/skybox_material/skybox_material.bin"; 

constexpr const wchar_t* FILE_PATH_BIN_WATER_PLANE = L"resource/dxe_parameters/water_plane/water_plane.bin"; 

constexpr const wchar_t* FILE_PATH_OTF_03TAKIBI_REGULAR = L"resource/font/03Takibi-Regular.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_06TAKIBI_BOLD = L"resource/font/06Takibi-Bold.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_AKABARA_CINDERELLA = L"resource/font/akabara-cinderella.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_APPLIMINCHO = L"resource/font/AppliMincho.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3 = L"resource/font/Corporate-Logo-Rounded-Bold-ver3.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_CORPORATE_MINCHO_VER3 = L"resource/font/Corporate-Mincho-ver3.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_DIGITALISM = L"resource/font/digitalism.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_DS_DIGIB = L"resource/font/DS-DIGIB.TTF"; 
constexpr const wchar_t* FILE_PATH_TTF_GENKAI_MINCHO = L"resource/font/genkai-mincho.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_HANGYAKU_XRRWQ = L"resource/font/Hangyaku-xRRwq.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_HARENOSORA = L"resource/font/Harenosora.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_HONOKA_SHIN_MARU_GOTHIC_R = L"resource/font/Honoka-Shin-Maru-Gothic_R.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_HONOKA_SHIN_MINCHO_L = L"resource/font/Honoka_Shin_Mincho_L.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_IPAEXM = L"resource/font/ipaexm.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_KOWAIFONT = L"resource/font/kowaiFont.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_KUROBARA_CINDERELLA = L"resource/font/kurobara-cinderella.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_LED_DIGITAL_7 = L"resource/font/led_digital_7.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_LIGHTNOVELPOPV2 = L"resource/font/LightNovelPOPv2.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_MAGICRING = L"resource/font/MagicRing.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS = L"resource/font/Mattsun-Marus_samurai_spirits.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_MOBILEFONT = L"resource/font/MobileFont.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT = L"resource/font/NagomiGokubosoGothic-ExtraLight.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_NEKO_NO_MEZAME = L"resource/font/Neko_no_Mezame.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_OTOMANOPEEONE_REGULAR = L"resource/font/OtomanopeeOne-Regular.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_PENGS = L"resource/font/PENGS.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_PLANETARIUM = L"resource/font/Planetarium.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_SAWARABIGOTHIC_REGULAR = L"resource/font/SawarabiGothic-Regular.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_SENOBI_GOTHIC_REGULAR = L"resource/font/Senobi-Gothic-Regular.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_SMARTFONTUI = L"resource/font/SmartFontUI.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_SOLID_REGULAR = L"resource/font/Solid-Regular.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_SOUKOUMINCHO = L"resource/font/SoukouMincho.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_TIMEMACHINE_WA = L"resource/font/timemachine-wa.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_TSUNAGIGOTHIC = L"resource/font/TsunagiGothic.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_UMEBOSHI_ = L"resource/font/umeboshi_.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_VOYNICH_MONOSPACE = L"resource/font/Voynich_MonoSpace.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_ZEROGOTHIC = L"resource/font/ZeroGothic.otf"; 

constexpr const wchar_t* FILE_PATH_PNG_A_LEFT_BTN = L"resource/graphics/example/a_left_btn.png"; 
constexpr const wchar_t* FILE_PATH_JPG_BACK_GROUND = L"resource/graphics/example/back_ground.jpg"; 
constexpr const wchar_t* FILE_PATH_PNG_BLACK1 = L"resource/graphics/example/black1.png"; 
constexpr const wchar_t* FILE_PATH_BMP_BLUE1 = L"resource/graphics/example/blue1.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_C1_ANIM_DOWN = L"resource/graphics/example/c1_anim_down.png"; 
constexpr const wchar_t* FILE_PATH_PNG_C1_ANIM_LEFT = L"resource/graphics/example/c1_anim_left.png"; 
constexpr const wchar_t* FILE_PATH_PNG_C1_ANIM_RIGHT = L"resource/graphics/example/c1_anim_right.png"; 
constexpr const wchar_t* FILE_PATH_PNG_C1_ANIM_UP = L"resource/graphics/example/c1_anim_up.png"; 
constexpr const wchar_t* FILE_PATH_PNG_C2_ANIM_IDLE = L"resource/graphics/example/c2_anim_idle.png"; 
constexpr const wchar_t* FILE_PATH_PNG_C2_ANIM_JUMP = L"resource/graphics/example/c2_anim_jump.png"; 
constexpr const wchar_t* FILE_PATH_PNG_C2_ANIM_RUN = L"resource/graphics/example/c2_anim_run.png"; 
constexpr const wchar_t* FILE_PATH_PNG_C2_ANIM_WALK = L"resource/graphics/example/c2_anim_walk.png"; 
constexpr const wchar_t* FILE_PATH_BMP_CUBE = L"resource/graphics/example/cube.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_CURSOR = L"resource/graphics/example/cursor.png"; 
constexpr const wchar_t* FILE_PATH_BMP_DOTTED_LINE = L"resource/graphics/example/dotted_line.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_D_RIGHT_BTN = L"resource/graphics/example/d_right_btn.png"; 
constexpr const wchar_t* FILE_PATH_PNG_EFCT1 = L"resource/graphics/example/efct1.png"; 
constexpr const wchar_t* FILE_PATH_PNG_EFCT2 = L"resource/graphics/example/efct2.png"; 
constexpr const wchar_t* FILE_PATH_PNG_FIRE = L"resource/graphics/example/fire.png"; 
constexpr const wchar_t* FILE_PATH_BMP_GREEN1 = L"resource/graphics/example/green1.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_HEIGHT_MAP = L"resource/graphics/example/height_map.png"; 
constexpr const wchar_t* FILE_PATH_PNG_LAWN = L"resource/graphics/example/lawn.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PAPER = L"resource/graphics/example/paper.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PIPO_MAP001 = L"resource/graphics/example/pipo-map001.png"; 
constexpr const wchar_t* FILE_PATH_BMP_PLAYER = L"resource/graphics/example/player.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_POINT = L"resource/graphics/example/point.png"; 
constexpr const wchar_t* FILE_PATH_BMP_RED1 = L"resource/graphics/example/red1.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_SAMP_ATTACK = L"resource/graphics/example/samp_attack.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SANDY_BEACH = L"resource/graphics/example/sandy_beach.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SCISSORS = L"resource/graphics/example/scissors.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SIFI_TOWN3_6N = L"resource/graphics/example/Sifi_town3_6n.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SKY01 = L"resource/graphics/example/sky01.png"; 
constexpr const wchar_t* FILE_PATH_PNG_STONE = L"resource/graphics/example/stone.png"; 
constexpr const wchar_t* FILE_PATH_PNG_S_DOWN_BTN = L"resource/graphics/example/s_down_btn.png"; 
constexpr const wchar_t* FILE_PATH_BMP_TEST = L"resource/graphics/example/test.bmp"; 
constexpr const wchar_t* FILE_PATH_JPG_TEST = L"resource/graphics/example/test.jpg"; 
constexpr const wchar_t* FILE_PATH_BMP_TEST2 = L"resource/graphics/example/test2.bmp"; 
constexpr const wchar_t* FILE_PATH_BMP_TEST_MAP_CHIP = L"resource/graphics/example/test_map_chip.bmp"; 
constexpr const wchar_t* FILE_PATH_BMP_WHITE = L"resource/graphics/example/white.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_W_UP_BTN = L"resource/graphics/example/w_up_btn.png"; 

constexpr const wchar_t* FILE_PATH_BMP_BOX_BACK = L"resource/graphics/example/box/box_back.bmp"; 
constexpr const wchar_t* FILE_PATH_BMP_BOX_DOWN = L"resource/graphics/example/box/box_down.bmp"; 
constexpr const wchar_t* FILE_PATH_BMP_BOX_FORWORD = L"resource/graphics/example/box/box_forword.bmp"; 
constexpr const wchar_t* FILE_PATH_BMP_BOX_LEFT = L"resource/graphics/example/box/box_left.bmp"; 
constexpr const wchar_t* FILE_PATH_BMP_BOX_RIGHT = L"resource/graphics/example/box/box_right.bmp"; 
constexpr const wchar_t* FILE_PATH_BMP_BOX_UP = L"resource/graphics/example/box/box_up.bmp"; 

constexpr const wchar_t* FILE_PATH_PNG_PARTICLEA = L"resource/graphics/example/particle/particleA.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PARTICLEB = L"resource/graphics/example/particle/particleB.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PARTICLEC = L"resource/graphics/example/particle/particleC.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PARTICLED = L"resource/graphics/example/particle/particleD.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PARTICLEE = L"resource/graphics/example/particle/particleE.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PARTICLEF = L"resource/graphics/example/particle/particleF.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PARTICLEG = L"resource/graphics/example/particle/particleG.png"; 

constexpr const wchar_t* FILE_PATH_PNG_MAGICAL = L"resource/graphics/example/skybox/Magical.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SKYBOX_A = L"resource/graphics/example/skybox/skybox_a.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SKYBOX_B = L"resource/graphics/example/skybox/skybox_b.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SKYBOX_C = L"resource/graphics/example/skybox/skybox_c.png"; 

constexpr const wchar_t* FILE_PATH_BMP_DEC_DECORATION_A = L"resource/graphics/example/water_plane/dec_decoration_A.bmp"; 
constexpr const wchar_t* FILE_PATH_PNG_DEC_DECORATION_B = L"resource/graphics/example/water_plane/dec_decoration_B.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PAT_PATTERN_A = L"resource/graphics/example/water_plane/pat_pattern_A.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PAT_PATTERN_B = L"resource/graphics/example/water_plane/pat_pattern_B.png"; 

constexpr const wchar_t* FILE_PATH_JSON_EXAMPLE = L"resource/json/example.json"; 

constexpr const wchar_t* FILE_PATH_MV1_WOODENBRIDGE_01 = L"resource/mesh/mv/bridge/WoodenBridge_01.mv1"; 
constexpr const wchar_t* FILE_PATH_JPG_WOOD_01 = L"resource/mesh/mv/bridge/Wood_01.jpg"; 

constexpr const wchar_t* FILE_PATH_MV1_CANOE = L"resource/mesh/mv/canoe/Canoe.mv1"; 
constexpr const wchar_t* FILE_PATH_JPG_WOODFINE0025_M = L"resource/mesh/mv/canoe/WoodFine0025_M.jpg"; 

constexpr const wchar_t* FILE_PATH_MV1_BARREL = L"resource/mesh/mv/preset/barrel.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_CONE = L"resource/mesh/mv/preset/cone.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_CONE_CYLINDER = L"resource/mesh/mv/preset/cone_cylinder.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_CUBE = L"resource/mesh/mv/preset/cube.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_CYLINDER = L"resource/mesh/mv/preset/cylinder.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_DISK = L"resource/mesh/mv/preset/disk.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_DOME = L"resource/mesh/mv/preset/dome.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_HALF_BARREL = L"resource/mesh/mv/preset/half_barrel.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_HOLLOW_OUT_DISK_PLANE = L"resource/mesh/mv/preset/hollow_out_disk_plane.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_PLANE = L"resource/mesh/mv/preset/plane.mv1"; 
constexpr const wchar_t* FILE_PATH_MV1_SPHERE = L"resource/mesh/mv/preset/sphere.mv1"; 

constexpr const wchar_t* FILE_PATH_MTL_SIMPLE_TRIANGLE = L"resource/mesh/obj/simple_triangle.mtl"; 
constexpr const wchar_t* FILE_PATH_OBJ_SIMPLE_TRIANGLE = L"resource/mesh/obj/simple_triangle.obj"; 

constexpr const wchar_t* FILE_PATH_JPG_F4U1BLET = L"resource/mesh/obj/26F4U1-jet/F4U1BLET.JPG"; 
constexpr const wchar_t* FILE_PATH_JPG_F4U1FURT = L"resource/mesh/obj/26F4U1-jet/F4U1FURT.JPG"; 
constexpr const wchar_t* FILE_PATH_JPG_F4U1TLB = L"resource/mesh/obj/26F4U1-jet/F4U1TLB.JPG"; 
constexpr const wchar_t* FILE_PATH_JPG_F4U1TLLT = L"resource/mesh/obj/26F4U1-jet/F4U1TLLT.JPG"; 
constexpr const wchar_t* FILE_PATH_JPG_F4U1TLRT = L"resource/mesh/obj/26F4U1-jet/F4U1TLRT.JPG"; 
constexpr const wchar_t* FILE_PATH_JPG_F4U1WHNT = L"resource/mesh/obj/26F4U1-jet/F4U1WHNT.JPG"; 
constexpr const wchar_t* FILE_PATH_JPG_F4U1WTLT = L"resource/mesh/obj/26F4U1-jet/F4U1WTLT.JPG"; 
constexpr const wchar_t* FILE_PATH_OBJ_F4U1_L = L"resource/mesh/obj/26F4U1-jet/F4U1_L.obj"; 
constexpr const wchar_t* FILE_PATH_OBJ_MTL_F4U1_L = L"resource/mesh/obj/26F4U1-jet/F4U1_L.obj.mtl"; 

constexpr const wchar_t* FILE_PATH_X_TEST = L"resource/mesh/x_file/test.x"; 

constexpr const wchar_t* FILE_PATH_BMP_BOX = L"resource/mesh/x_file/box/box.bmp"; 
constexpr const wchar_t* FILE_PATH_X_BOX = L"resource/mesh/x_file/box/box.x"; 

constexpr const wchar_t* FILE_PATH_BMP_PLANE = L"resource/mesh/x_file/plane/plane.bmp"; 
constexpr const wchar_t* FILE_PATH_X_PLANE = L"resource/mesh/x_file/plane/plane.x"; 
constexpr const wchar_t* FILE_PATH_X_TEMP = L"resource/mesh/x_file/plane/temp.x"; 

constexpr const wchar_t* FILE_PATH_BMP_SPHERE = L"resource/mesh/x_file/sphere/sphere.bmp"; 
constexpr const wchar_t* FILE_PATH_X_SPHERE = L"resource/mesh/x_file/sphere/sphere.x"; 
constexpr const wchar_t* FILE_PATH_BMP_SPHERE1 = L"resource/mesh/x_file/sphere/sphere1.bmp"; 
constexpr const wchar_t* FILE_PATH_X_SPHERE2 = L"resource/mesh/x_file/sphere/sphere2.x"; 

constexpr const wchar_t* FILE_PATH_PSO_EREA_PROJECTION = L"resource/shader/dxe/erea_projection.pso"; 
constexpr const wchar_t* FILE_PATH_VSO_EREA_PROJECTION = L"resource/shader/dxe/erea_projection.vso"; 

constexpr const wchar_t* FILE_PATH_MP3_BGM_SAMPLE_0 = L"resource/sound/example/bgm_sample_0.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_SE_SAMPLE_0 = L"resource/sound/example/se_sample_0.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_SE_SAMPLE_1 = L"resource/sound/example/se_sample_1.mp3"; 
constexpr const wchar_t* FILE_PATH_WAV_SE_SAMPLE_2 = L"resource/sound/example/se_sample_2.wav"; 

constexpr const wchar_t* FILE_PATH_TXT_SAMPLE_TEXT = L"resource/text/sample_text.txt"; 



//------------------------------------------------------------------------------------------------------------------
// 
// file path table 
// 


const std::vector<const wchar_t*> FILE_PATH_TBL_CSV = {
    FILE_PATH_CSV_EXAMPLE,
    FILE_PATH_CSV_EXAMPLE_MAP_CHIP,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_DXE_MAP_EDITOR_DATA_SAMPLE_MAP1 = {
    FILE_PATH_PNG_MAP1_ANIM_CHIPS_32,
    FILE_PATH_CSV_MAP1_ANIM_CHIP_INFO_32,
    FILE_PATH_CSV_MAP1_COLLISION_LAYER_0,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_0,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_1,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_2,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_3,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_4,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_5,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_6,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_7,
    FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_8,
    FILE_PATH_CSV_MAP1_LAYER_SETTINGS,
    FILE_PATH_PNG_MAP1_STIL_CHIPS_128,
    FILE_PATH_PNG_MAP1_STIL_CHIPS_32,
    FILE_PATH_PNG_MAP1_STIL_CHIPS_96,
    FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_128,
    FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_32,
    FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_96,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_DXE_PARAMETERS_DIRECTIONAL_LIGHT = {
    FILE_PATH_BIN_DIRECTIONAL_LIGHT_PARAMETER,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_DXE_PARAMETERS_GRAPHICS_LAYER = {
    FILE_PATH_BIN_GRAPHICS_LAYER_STATUS,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_DXE_PARAMETERS_MESH_MATERIAL = {
    FILE_PATH_BIN_MATERIAL,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_DXE_PARAMETERS_PARTICLE_PRESET = {
    FILE_PATH_BIN_CONVERGENCE,
    FILE_PATH_BIN_DEFAULT,
    FILE_PATH_BIN_EXPLOSION,
    FILE_PATH_BIN_FIRE,
    FILE_PATH_BIN_FIREWORKS,
    FILE_PATH_BIN_RAIN,
    FILE_PATH_BIN_RAIN_GROUND,
    FILE_PATH_BIN_SMOKE,
    FILE_PATH_BIN_SNOW,
    FILE_PATH_BIN_WATER_FALL,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_DXE_PARAMETERS_SKYBOX_MATERIAL = {
    FILE_PATH_BIN_SKYBOX_MATERIAL,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_DXE_PARAMETERS_WATER_PLANE = {
    FILE_PATH_BIN_WATER_PLANE,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_FONT = {
    FILE_PATH_OTF_03TAKIBI_REGULAR,
    FILE_PATH_OTF_06TAKIBI_BOLD,
    FILE_PATH_TTF_AKABARA_CINDERELLA,
    FILE_PATH_OTF_APPLIMINCHO,
    FILE_PATH_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3,
    FILE_PATH_OTF_CORPORATE_MINCHO_VER3,
    FILE_PATH_TTF_DIGITALISM,
    FILE_PATH_TTF_DS_DIGIB,
    FILE_PATH_TTF_GENKAI_MINCHO,
    FILE_PATH_TTF_HANGYAKU_XRRWQ,
    FILE_PATH_OTF_HARENOSORA,
    FILE_PATH_OTF_HONOKA_SHIN_MARU_GOTHIC_R,
    FILE_PATH_OTF_HONOKA_SHIN_MINCHO_L,
    FILE_PATH_TTF_IPAEXM,
    FILE_PATH_OTF_KOWAIFONT,
    FILE_PATH_TTF_KUROBARA_CINDERELLA,
    FILE_PATH_TTF_LED_DIGITAL_7,
    FILE_PATH_OTF_LIGHTNOVELPOPV2,
    FILE_PATH_TTF_MAGICRING,
    FILE_PATH_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS,
    FILE_PATH_TTF_MOBILEFONT,
    FILE_PATH_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT,
    FILE_PATH_TTF_NEKO_NO_MEZAME,
    FILE_PATH_TTF_OTOMANOPEEONE_REGULAR,
    FILE_PATH_TTF_PENGS,
    FILE_PATH_OTF_PLANETARIUM,
    FILE_PATH_TTF_SAWARABIGOTHIC_REGULAR,
    FILE_PATH_TTF_SENOBI_GOTHIC_REGULAR,
    FILE_PATH_OTF_SMARTFONTUI,
    FILE_PATH_OTF_SOLID_REGULAR,
    FILE_PATH_TTF_SOUKOUMINCHO,
    FILE_PATH_TTF_TIMEMACHINE_WA,
    FILE_PATH_TTF_TSUNAGIGOTHIC,
    FILE_PATH_TTF_UMEBOSHI_,
    FILE_PATH_TTF_VOYNICH_MONOSPACE,
    FILE_PATH_OTF_ZEROGOTHIC,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_GRAPHICS_EXAMPLE = {
    FILE_PATH_PNG_A_LEFT_BTN,
    FILE_PATH_JPG_BACK_GROUND,
    FILE_PATH_PNG_BLACK1,
    FILE_PATH_BMP_BLUE1,
    FILE_PATH_PNG_C1_ANIM_DOWN,
    FILE_PATH_PNG_C1_ANIM_LEFT,
    FILE_PATH_PNG_C1_ANIM_RIGHT,
    FILE_PATH_PNG_C1_ANIM_UP,
    FILE_PATH_PNG_C2_ANIM_IDLE,
    FILE_PATH_PNG_C2_ANIM_JUMP,
    FILE_PATH_PNG_C2_ANIM_RUN,
    FILE_PATH_PNG_C2_ANIM_WALK,
    FILE_PATH_BMP_CUBE,
    FILE_PATH_PNG_CURSOR,
    FILE_PATH_BMP_DOTTED_LINE,
    FILE_PATH_PNG_D_RIGHT_BTN,
    FILE_PATH_PNG_EFCT1,
    FILE_PATH_PNG_EFCT2,
    FILE_PATH_PNG_FIRE,
    FILE_PATH_BMP_GREEN1,
    FILE_PATH_PNG_HEIGHT_MAP,
    FILE_PATH_PNG_LAWN,
    FILE_PATH_PNG_PAPER,
    FILE_PATH_PNG_PIPO_MAP001,
    FILE_PATH_BMP_PLAYER,
    FILE_PATH_PNG_POINT,
    FILE_PATH_BMP_RED1,
    FILE_PATH_PNG_SAMP_ATTACK,
    FILE_PATH_PNG_SANDY_BEACH,
    FILE_PATH_PNG_SCISSORS,
    FILE_PATH_PNG_SIFI_TOWN3_6N,
    FILE_PATH_PNG_SKY01,
    FILE_PATH_PNG_STONE,
    FILE_PATH_PNG_S_DOWN_BTN,
    FILE_PATH_BMP_TEST,
    FILE_PATH_JPG_TEST,
    FILE_PATH_BMP_TEST2,
    FILE_PATH_BMP_TEST_MAP_CHIP,
    FILE_PATH_BMP_WHITE,
    FILE_PATH_PNG_W_UP_BTN,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_GRAPHICS_EXAMPLE_BOX = {
    FILE_PATH_BMP_BOX_BACK,
    FILE_PATH_BMP_BOX_DOWN,
    FILE_PATH_BMP_BOX_FORWORD,
    FILE_PATH_BMP_BOX_LEFT,
    FILE_PATH_BMP_BOX_RIGHT,
    FILE_PATH_BMP_BOX_UP,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_GRAPHICS_EXAMPLE_PARTICLE = {
    FILE_PATH_PNG_PARTICLEA,
    FILE_PATH_PNG_PARTICLEB,
    FILE_PATH_PNG_PARTICLEC,
    FILE_PATH_PNG_PARTICLED,
    FILE_PATH_PNG_PARTICLEE,
    FILE_PATH_PNG_PARTICLEF,
    FILE_PATH_PNG_PARTICLEG,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_GRAPHICS_EXAMPLE_SKYBOX = {
    FILE_PATH_PNG_MAGICAL,
    FILE_PATH_PNG_SKYBOX_A,
    FILE_PATH_PNG_SKYBOX_B,
    FILE_PATH_PNG_SKYBOX_C,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_GRAPHICS_EXAMPLE_WATER_PLANE = {
    FILE_PATH_BMP_DEC_DECORATION_A,
    FILE_PATH_PNG_DEC_DECORATION_B,
    FILE_PATH_PNG_PAT_PATTERN_A,
    FILE_PATH_PNG_PAT_PATTERN_B,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_JSON = {
    FILE_PATH_JSON_EXAMPLE,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_MV_BRIDGE = {
    FILE_PATH_MV1_WOODENBRIDGE_01,
    FILE_PATH_JPG_WOOD_01,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_MV_CANOE = {
    FILE_PATH_MV1_CANOE,
    FILE_PATH_JPG_WOODFINE0025_M,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_MV_PRESET = {
    FILE_PATH_MV1_BARREL,
    FILE_PATH_MV1_CONE,
    FILE_PATH_MV1_CONE_CYLINDER,
    FILE_PATH_MV1_CUBE,
    FILE_PATH_MV1_CYLINDER,
    FILE_PATH_MV1_DISK,
    FILE_PATH_MV1_DOME,
    FILE_PATH_MV1_HALF_BARREL,
    FILE_PATH_MV1_HOLLOW_OUT_DISK_PLANE,
    FILE_PATH_MV1_PLANE,
    FILE_PATH_MV1_SPHERE,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_OBJ = {
    FILE_PATH_MTL_SIMPLE_TRIANGLE,
    FILE_PATH_OBJ_SIMPLE_TRIANGLE,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_OBJ_26F4U1_JET = {
    FILE_PATH_JPG_F4U1BLET,
    FILE_PATH_JPG_F4U1FURT,
    FILE_PATH_JPG_F4U1TLB,
    FILE_PATH_JPG_F4U1TLLT,
    FILE_PATH_JPG_F4U1TLRT,
    FILE_PATH_JPG_F4U1WHNT,
    FILE_PATH_JPG_F4U1WTLT,
    FILE_PATH_OBJ_F4U1_L,
    FILE_PATH_OBJ_MTL_F4U1_L,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_X_FILE = {
    FILE_PATH_X_TEST,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_X_FILE_BOX = {
    FILE_PATH_BMP_BOX,
    FILE_PATH_X_BOX,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_X_FILE_PLANE = {
    FILE_PATH_BMP_PLANE,
    FILE_PATH_X_PLANE,
    FILE_PATH_X_TEMP,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_MESH_X_FILE_SPHERE = {
    FILE_PATH_BMP_SPHERE,
    FILE_PATH_X_SPHERE,
    FILE_PATH_BMP_SPHERE1,
    FILE_PATH_X_SPHERE2,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_SHADER_DXE = {
    FILE_PATH_PSO_EREA_PROJECTION,
    FILE_PATH_VSO_EREA_PROJECTION,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_SOUND_EXAMPLE = {
    FILE_PATH_MP3_BGM_SAMPLE_0,
    FILE_PATH_MP3_SE_SAMPLE_0,
    FILE_PATH_MP3_SE_SAMPLE_1,
    FILE_PATH_WAV_SE_SAMPLE_2,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_TEXT = {
    FILE_PATH_TXT_SAMPLE_TEXT,
};


//------------------------------------------------------------------------------------------------------------------
// 
// file path map 
// 


const std::unordered_map<std::wstring, const wchar_t*> FILE_PATH_DICTIONARY = { 
    { std::wstring( L"example.csv" ), FILE_PATH_CSV_EXAMPLE },
    { std::wstring( L"example_map_chip.csv" ), FILE_PATH_CSV_EXAMPLE_MAP_CHIP },
    { std::wstring( L"map1_anim_chips_32.png" ), FILE_PATH_PNG_MAP1_ANIM_CHIPS_32 },
    { std::wstring( L"map1_anim_chip_info_32.csv" ), FILE_PATH_CSV_MAP1_ANIM_CHIP_INFO_32 },
    { std::wstring( L"map1_collision_layer_0.csv" ), FILE_PATH_CSV_MAP1_COLLISION_LAYER_0 },
    { std::wstring( L"map1_landscape_layer_0.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_0 },
    { std::wstring( L"map1_landscape_layer_1.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_1 },
    { std::wstring( L"map1_landscape_layer_2.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_2 },
    { std::wstring( L"map1_landscape_layer_3.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_3 },
    { std::wstring( L"map1_landscape_layer_4.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_4 },
    { std::wstring( L"map1_landscape_layer_5.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_5 },
    { std::wstring( L"map1_landscape_layer_6.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_6 },
    { std::wstring( L"map1_landscape_layer_7.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_7 },
    { std::wstring( L"map1_landscape_layer_8.csv" ), FILE_PATH_CSV_MAP1_LANDSCAPE_LAYER_8 },
    { std::wstring( L"map1_layer_settings.csv" ), FILE_PATH_CSV_MAP1_LAYER_SETTINGS },
    { std::wstring( L"map1_stil_chips_128.png" ), FILE_PATH_PNG_MAP1_STIL_CHIPS_128 },
    { std::wstring( L"map1_stil_chips_32.png" ), FILE_PATH_PNG_MAP1_STIL_CHIPS_32 },
    { std::wstring( L"map1_stil_chips_96.png" ), FILE_PATH_PNG_MAP1_STIL_CHIPS_96 },
    { std::wstring( L"map1_stil_chip_info_128.csv" ), FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_128 },
    { std::wstring( L"map1_stil_chip_info_32.csv" ), FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_32 },
    { std::wstring( L"map1_stil_chip_info_96.csv" ), FILE_PATH_CSV_MAP1_STIL_CHIP_INFO_96 },
    { std::wstring( L"directional_light_parameter.bin" ), FILE_PATH_BIN_DIRECTIONAL_LIGHT_PARAMETER },
    { std::wstring( L"graphics_layer_status.bin" ), FILE_PATH_BIN_GRAPHICS_LAYER_STATUS },
    { std::wstring( L"material.bin" ), FILE_PATH_BIN_MATERIAL },
    { std::wstring( L"convergence.bin" ), FILE_PATH_BIN_CONVERGENCE },
    { std::wstring( L"default.bin" ), FILE_PATH_BIN_DEFAULT },
    { std::wstring( L"explosion.bin" ), FILE_PATH_BIN_EXPLOSION },
    { std::wstring( L"fire.bin" ), FILE_PATH_BIN_FIRE },
    { std::wstring( L"fireworks.bin" ), FILE_PATH_BIN_FIREWORKS },
    { std::wstring( L"rain.bin" ), FILE_PATH_BIN_RAIN },
    { std::wstring( L"rain_ground.bin" ), FILE_PATH_BIN_RAIN_GROUND },
    { std::wstring( L"smoke.bin" ), FILE_PATH_BIN_SMOKE },
    { std::wstring( L"snow.bin" ), FILE_PATH_BIN_SNOW },
    { std::wstring( L"water_fall.bin" ), FILE_PATH_BIN_WATER_FALL },
    { std::wstring( L"skybox_material.bin" ), FILE_PATH_BIN_SKYBOX_MATERIAL },
    { std::wstring( L"water_plane.bin" ), FILE_PATH_BIN_WATER_PLANE },
    { std::wstring( L"03Takibi-Regular.otf" ), FILE_PATH_OTF_03TAKIBI_REGULAR },
    { std::wstring( L"06Takibi-Bold.otf" ), FILE_PATH_OTF_06TAKIBI_BOLD },
    { std::wstring( L"akabara-cinderella.ttf" ), FILE_PATH_TTF_AKABARA_CINDERELLA },
    { std::wstring( L"AppliMincho.otf" ), FILE_PATH_OTF_APPLIMINCHO },
    { std::wstring( L"Corporate-Logo-Rounded-Bold-ver3.otf" ), FILE_PATH_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3 },
    { std::wstring( L"Corporate-Mincho-ver3.otf" ), FILE_PATH_OTF_CORPORATE_MINCHO_VER3 },
    { std::wstring( L"digitalism.ttf" ), FILE_PATH_TTF_DIGITALISM },
    { std::wstring( L"DS-DIGIB.TTF" ), FILE_PATH_TTF_DS_DIGIB },
    { std::wstring( L"genkai-mincho.ttf" ), FILE_PATH_TTF_GENKAI_MINCHO },
    { std::wstring( L"Hangyaku-xRRwq.ttf" ), FILE_PATH_TTF_HANGYAKU_XRRWQ },
    { std::wstring( L"Harenosora.otf" ), FILE_PATH_OTF_HARENOSORA },
    { std::wstring( L"Honoka-Shin-Maru-Gothic_R.otf" ), FILE_PATH_OTF_HONOKA_SHIN_MARU_GOTHIC_R },
    { std::wstring( L"Honoka_Shin_Mincho_L.otf" ), FILE_PATH_OTF_HONOKA_SHIN_MINCHO_L },
    { std::wstring( L"ipaexm.ttf" ), FILE_PATH_TTF_IPAEXM },
    { std::wstring( L"kowaiFont.otf" ), FILE_PATH_OTF_KOWAIFONT },
    { std::wstring( L"kurobara-cinderella.ttf" ), FILE_PATH_TTF_KUROBARA_CINDERELLA },
    { std::wstring( L"led_digital_7.ttf" ), FILE_PATH_TTF_LED_DIGITAL_7 },
    { std::wstring( L"LightNovelPOPv2.otf" ), FILE_PATH_OTF_LIGHTNOVELPOPV2 },
    { std::wstring( L"MagicRing.ttf" ), FILE_PATH_TTF_MAGICRING },
    { std::wstring( L"Mattsun-Marus_samurai_spirits.ttf" ), FILE_PATH_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS },
    { std::wstring( L"MobileFont.ttf" ), FILE_PATH_TTF_MOBILEFONT },
    { std::wstring( L"NagomiGokubosoGothic-ExtraLight.otf" ), FILE_PATH_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT },
    { std::wstring( L"Neko_no_Mezame.ttf" ), FILE_PATH_TTF_NEKO_NO_MEZAME },
    { std::wstring( L"OtomanopeeOne-Regular.ttf" ), FILE_PATH_TTF_OTOMANOPEEONE_REGULAR },
    { std::wstring( L"PENGS.ttf" ), FILE_PATH_TTF_PENGS },
    { std::wstring( L"Planetarium.otf" ), FILE_PATH_OTF_PLANETARIUM },
    { std::wstring( L"SawarabiGothic-Regular.ttf" ), FILE_PATH_TTF_SAWARABIGOTHIC_REGULAR },
    { std::wstring( L"Senobi-Gothic-Regular.ttf" ), FILE_PATH_TTF_SENOBI_GOTHIC_REGULAR },
    { std::wstring( L"SmartFontUI.otf" ), FILE_PATH_OTF_SMARTFONTUI },
    { std::wstring( L"Solid-Regular.otf" ), FILE_PATH_OTF_SOLID_REGULAR },
    { std::wstring( L"SoukouMincho.ttf" ), FILE_PATH_TTF_SOUKOUMINCHO },
    { std::wstring( L"timemachine-wa.ttf" ), FILE_PATH_TTF_TIMEMACHINE_WA },
    { std::wstring( L"TsunagiGothic.ttf" ), FILE_PATH_TTF_TSUNAGIGOTHIC },
    { std::wstring( L"umeboshi_.ttf" ), FILE_PATH_TTF_UMEBOSHI_ },
    { std::wstring( L"Voynich_MonoSpace.ttf" ), FILE_PATH_TTF_VOYNICH_MONOSPACE },
    { std::wstring( L"ZeroGothic.otf" ), FILE_PATH_OTF_ZEROGOTHIC },
    { std::wstring( L"a_left_btn.png" ), FILE_PATH_PNG_A_LEFT_BTN },
    { std::wstring( L"back_ground.jpg" ), FILE_PATH_JPG_BACK_GROUND },
    { std::wstring( L"black1.png" ), FILE_PATH_PNG_BLACK1 },
    { std::wstring( L"blue1.bmp" ), FILE_PATH_BMP_BLUE1 },
    { std::wstring( L"c1_anim_down.png" ), FILE_PATH_PNG_C1_ANIM_DOWN },
    { std::wstring( L"c1_anim_left.png" ), FILE_PATH_PNG_C1_ANIM_LEFT },
    { std::wstring( L"c1_anim_right.png" ), FILE_PATH_PNG_C1_ANIM_RIGHT },
    { std::wstring( L"c1_anim_up.png" ), FILE_PATH_PNG_C1_ANIM_UP },
    { std::wstring( L"c2_anim_idle.png" ), FILE_PATH_PNG_C2_ANIM_IDLE },
    { std::wstring( L"c2_anim_jump.png" ), FILE_PATH_PNG_C2_ANIM_JUMP },
    { std::wstring( L"c2_anim_run.png" ), FILE_PATH_PNG_C2_ANIM_RUN },
    { std::wstring( L"c2_anim_walk.png" ), FILE_PATH_PNG_C2_ANIM_WALK },
    { std::wstring( L"cube.bmp" ), FILE_PATH_BMP_CUBE },
    { std::wstring( L"cursor.png" ), FILE_PATH_PNG_CURSOR },
    { std::wstring( L"dotted_line.bmp" ), FILE_PATH_BMP_DOTTED_LINE },
    { std::wstring( L"d_right_btn.png" ), FILE_PATH_PNG_D_RIGHT_BTN },
    { std::wstring( L"efct1.png" ), FILE_PATH_PNG_EFCT1 },
    { std::wstring( L"efct2.png" ), FILE_PATH_PNG_EFCT2 },
    { std::wstring( L"fire.png" ), FILE_PATH_PNG_FIRE },
    { std::wstring( L"green1.bmp" ), FILE_PATH_BMP_GREEN1 },
    { std::wstring( L"height_map.png" ), FILE_PATH_PNG_HEIGHT_MAP },
    { std::wstring( L"lawn.png" ), FILE_PATH_PNG_LAWN },
    { std::wstring( L"paper.png" ), FILE_PATH_PNG_PAPER },
    { std::wstring( L"pipo-map001.png" ), FILE_PATH_PNG_PIPO_MAP001 },
    { std::wstring( L"player.bmp" ), FILE_PATH_BMP_PLAYER },
    { std::wstring( L"point.png" ), FILE_PATH_PNG_POINT },
    { std::wstring( L"red1.bmp" ), FILE_PATH_BMP_RED1 },
    { std::wstring( L"samp_attack.png" ), FILE_PATH_PNG_SAMP_ATTACK },
    { std::wstring( L"sandy_beach.png" ), FILE_PATH_PNG_SANDY_BEACH },
    { std::wstring( L"scissors.png" ), FILE_PATH_PNG_SCISSORS },
    { std::wstring( L"Sifi_town3_6n.png" ), FILE_PATH_PNG_SIFI_TOWN3_6N },
    { std::wstring( L"sky01.png" ), FILE_PATH_PNG_SKY01 },
    { std::wstring( L"stone.png" ), FILE_PATH_PNG_STONE },
    { std::wstring( L"s_down_btn.png" ), FILE_PATH_PNG_S_DOWN_BTN },
    { std::wstring( L"test.bmp" ), FILE_PATH_BMP_TEST },
    { std::wstring( L"test.jpg" ), FILE_PATH_JPG_TEST },
    { std::wstring( L"test2.bmp" ), FILE_PATH_BMP_TEST2 },
    { std::wstring( L"test_map_chip.bmp" ), FILE_PATH_BMP_TEST_MAP_CHIP },
    { std::wstring( L"white.bmp" ), FILE_PATH_BMP_WHITE },
    { std::wstring( L"w_up_btn.png" ), FILE_PATH_PNG_W_UP_BTN },
    { std::wstring( L"box_back.bmp" ), FILE_PATH_BMP_BOX_BACK },
    { std::wstring( L"box_down.bmp" ), FILE_PATH_BMP_BOX_DOWN },
    { std::wstring( L"box_forword.bmp" ), FILE_PATH_BMP_BOX_FORWORD },
    { std::wstring( L"box_left.bmp" ), FILE_PATH_BMP_BOX_LEFT },
    { std::wstring( L"box_right.bmp" ), FILE_PATH_BMP_BOX_RIGHT },
    { std::wstring( L"box_up.bmp" ), FILE_PATH_BMP_BOX_UP },
    { std::wstring( L"particleA.png" ), FILE_PATH_PNG_PARTICLEA },
    { std::wstring( L"particleB.png" ), FILE_PATH_PNG_PARTICLEB },
    { std::wstring( L"particleC.png" ), FILE_PATH_PNG_PARTICLEC },
    { std::wstring( L"particleD.png" ), FILE_PATH_PNG_PARTICLED },
    { std::wstring( L"particleE.png" ), FILE_PATH_PNG_PARTICLEE },
    { std::wstring( L"particleF.png" ), FILE_PATH_PNG_PARTICLEF },
    { std::wstring( L"particleG.png" ), FILE_PATH_PNG_PARTICLEG },
    { std::wstring( L"Magical.png" ), FILE_PATH_PNG_MAGICAL },
    { std::wstring( L"skybox_a.png" ), FILE_PATH_PNG_SKYBOX_A },
    { std::wstring( L"skybox_b.png" ), FILE_PATH_PNG_SKYBOX_B },
    { std::wstring( L"skybox_c.png" ), FILE_PATH_PNG_SKYBOX_C },
    { std::wstring( L"dec_decoration_A.bmp" ), FILE_PATH_BMP_DEC_DECORATION_A },
    { std::wstring( L"dec_decoration_B.png" ), FILE_PATH_PNG_DEC_DECORATION_B },
    { std::wstring( L"pat_pattern_A.png" ), FILE_PATH_PNG_PAT_PATTERN_A },
    { std::wstring( L"pat_pattern_B.png" ), FILE_PATH_PNG_PAT_PATTERN_B },
    { std::wstring( L"example.json" ), FILE_PATH_JSON_EXAMPLE },
    { std::wstring( L"WoodenBridge_01.mv1" ), FILE_PATH_MV1_WOODENBRIDGE_01 },
    { std::wstring( L"Wood_01.jpg" ), FILE_PATH_JPG_WOOD_01 },
    { std::wstring( L"Canoe.mv1" ), FILE_PATH_MV1_CANOE },
    { std::wstring( L"WoodFine0025_M.jpg" ), FILE_PATH_JPG_WOODFINE0025_M },
    { std::wstring( L"barrel.mv1" ), FILE_PATH_MV1_BARREL },
    { std::wstring( L"cone.mv1" ), FILE_PATH_MV1_CONE },
    { std::wstring( L"cone_cylinder.mv1" ), FILE_PATH_MV1_CONE_CYLINDER },
    { std::wstring( L"cube.mv1" ), FILE_PATH_MV1_CUBE },
    { std::wstring( L"cylinder.mv1" ), FILE_PATH_MV1_CYLINDER },
    { std::wstring( L"disk.mv1" ), FILE_PATH_MV1_DISK },
    { std::wstring( L"dome.mv1" ), FILE_PATH_MV1_DOME },
    { std::wstring( L"half_barrel.mv1" ), FILE_PATH_MV1_HALF_BARREL },
    { std::wstring( L"hollow_out_disk_plane.mv1" ), FILE_PATH_MV1_HOLLOW_OUT_DISK_PLANE },
    { std::wstring( L"plane.mv1" ), FILE_PATH_MV1_PLANE },
    { std::wstring( L"sphere.mv1" ), FILE_PATH_MV1_SPHERE },
    { std::wstring( L"simple_triangle.mtl" ), FILE_PATH_MTL_SIMPLE_TRIANGLE },
    { std::wstring( L"simple_triangle.obj" ), FILE_PATH_OBJ_SIMPLE_TRIANGLE },
    { std::wstring( L"F4U1BLET.JPG" ), FILE_PATH_JPG_F4U1BLET },
    { std::wstring( L"F4U1FURT.JPG" ), FILE_PATH_JPG_F4U1FURT },
    { std::wstring( L"F4U1TLB.JPG" ), FILE_PATH_JPG_F4U1TLB },
    { std::wstring( L"F4U1TLLT.JPG" ), FILE_PATH_JPG_F4U1TLLT },
    { std::wstring( L"F4U1TLRT.JPG" ), FILE_PATH_JPG_F4U1TLRT },
    { std::wstring( L"F4U1WHNT.JPG" ), FILE_PATH_JPG_F4U1WHNT },
    { std::wstring( L"F4U1WTLT.JPG" ), FILE_PATH_JPG_F4U1WTLT },
    { std::wstring( L"F4U1_L.obj" ), FILE_PATH_OBJ_F4U1_L },
    { std::wstring( L"F4U1_L.obj.mtl" ), FILE_PATH_OBJ_MTL_F4U1_L },
    { std::wstring( L"test.x" ), FILE_PATH_X_TEST },
    { std::wstring( L"box.bmp" ), FILE_PATH_BMP_BOX },
    { std::wstring( L"box.x" ), FILE_PATH_X_BOX },
    { std::wstring( L"plane.bmp" ), FILE_PATH_BMP_PLANE },
    { std::wstring( L"plane.x" ), FILE_PATH_X_PLANE },
    { std::wstring( L"temp.x" ), FILE_PATH_X_TEMP },
    { std::wstring( L"sphere.bmp" ), FILE_PATH_BMP_SPHERE },
    { std::wstring( L"sphere.x" ), FILE_PATH_X_SPHERE },
    { std::wstring( L"sphere1.bmp" ), FILE_PATH_BMP_SPHERE1 },
    { std::wstring( L"sphere2.x" ), FILE_PATH_X_SPHERE2 },
    { std::wstring( L"erea_projection.pso" ), FILE_PATH_PSO_EREA_PROJECTION },
    { std::wstring( L"erea_projection.vso" ), FILE_PATH_VSO_EREA_PROJECTION },
    { std::wstring( L"bgm_sample_0.mp3" ), FILE_PATH_MP3_BGM_SAMPLE_0 },
    { std::wstring( L"se_sample_0.mp3" ), FILE_PATH_MP3_SE_SAMPLE_0 },
    { std::wstring( L"se_sample_1.mp3" ), FILE_PATH_MP3_SE_SAMPLE_1 },
    { std::wstring( L"se_sample_2.wav" ), FILE_PATH_WAV_SE_SAMPLE_2 },
    { std::wstring( L"sample_text.txt" ), FILE_PATH_TXT_SAMPLE_TEXT },
};


//------------------------------------------------------------------------------------------------------------------
// 
// file name 
// 


constexpr const wchar_t* FILE_NAME_CSV_EXAMPLE = L"example.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_EXAMPLE_MAP_CHIP = L"example_map_chip.csv"; 

constexpr const wchar_t* FILE_NAME_PNG_MAP1_ANIM_CHIPS_32 = L"map1_anim_chips_32.png"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_ANIM_CHIP_INFO_32 = L"map1_anim_chip_info_32.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_COLLISION_LAYER_0 = L"map1_collision_layer_0.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_0 = L"map1_landscape_layer_0.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_1 = L"map1_landscape_layer_1.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_2 = L"map1_landscape_layer_2.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_3 = L"map1_landscape_layer_3.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_4 = L"map1_landscape_layer_4.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_5 = L"map1_landscape_layer_5.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_6 = L"map1_landscape_layer_6.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_7 = L"map1_landscape_layer_7.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_8 = L"map1_landscape_layer_8.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_LAYER_SETTINGS = L"map1_layer_settings.csv"; 
constexpr const wchar_t* FILE_NAME_PNG_MAP1_STIL_CHIPS_128 = L"map1_stil_chips_128.png"; 
constexpr const wchar_t* FILE_NAME_PNG_MAP1_STIL_CHIPS_32 = L"map1_stil_chips_32.png"; 
constexpr const wchar_t* FILE_NAME_PNG_MAP1_STIL_CHIPS_96 = L"map1_stil_chips_96.png"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_STIL_CHIP_INFO_128 = L"map1_stil_chip_info_128.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_STIL_CHIP_INFO_32 = L"map1_stil_chip_info_32.csv"; 
constexpr const wchar_t* FILE_NAME_CSV_MAP1_STIL_CHIP_INFO_96 = L"map1_stil_chip_info_96.csv"; 

constexpr const wchar_t* FILE_NAME_BIN_DIRECTIONAL_LIGHT_PARAMETER = L"directional_light_parameter.bin"; 

constexpr const wchar_t* FILE_NAME_BIN_GRAPHICS_LAYER_STATUS = L"graphics_layer_status.bin"; 

constexpr const wchar_t* FILE_NAME_BIN_MATERIAL = L"material.bin"; 

constexpr const wchar_t* FILE_NAME_BIN_CONVERGENCE = L"convergence.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_DEFAULT = L"default.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_EXPLOSION = L"explosion.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_FIRE = L"fire.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_FIREWORKS = L"fireworks.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_RAIN = L"rain.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_RAIN_GROUND = L"rain_ground.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_SMOKE = L"smoke.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_SNOW = L"snow.bin"; 
constexpr const wchar_t* FILE_NAME_BIN_WATER_FALL = L"water_fall.bin"; 

constexpr const wchar_t* FILE_NAME_BIN_SKYBOX_MATERIAL = L"skybox_material.bin"; 

constexpr const wchar_t* FILE_NAME_BIN_WATER_PLANE = L"water_plane.bin"; 

constexpr const wchar_t* FILE_NAME_OTF_03TAKIBI_REGULAR = L"03Takibi-Regular.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_06TAKIBI_BOLD = L"06Takibi-Bold.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_AKABARA_CINDERELLA = L"akabara-cinderella.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_APPLIMINCHO = L"AppliMincho.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3 = L"Corporate-Logo-Rounded-Bold-ver3.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_CORPORATE_MINCHO_VER3 = L"Corporate-Mincho-ver3.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_DIGITALISM = L"digitalism.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_DS_DIGIB = L"DS-DIGIB.TTF"; 
constexpr const wchar_t* FILE_NAME_TTF_GENKAI_MINCHO = L"genkai-mincho.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_HANGYAKU_XRRWQ = L"Hangyaku-xRRwq.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_HARENOSORA = L"Harenosora.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_HONOKA_SHIN_MARU_GOTHIC_R = L"Honoka-Shin-Maru-Gothic_R.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_HONOKA_SHIN_MINCHO_L = L"Honoka_Shin_Mincho_L.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_IPAEXM = L"ipaexm.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_KOWAIFONT = L"kowaiFont.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_KUROBARA_CINDERELLA = L"kurobara-cinderella.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_LED_DIGITAL_7 = L"led_digital_7.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_LIGHTNOVELPOPV2 = L"LightNovelPOPv2.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_MAGICRING = L"MagicRing.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS = L"Mattsun-Marus_samurai_spirits.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_MOBILEFONT = L"MobileFont.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT = L"NagomiGokubosoGothic-ExtraLight.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_NEKO_NO_MEZAME = L"Neko_no_Mezame.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_OTOMANOPEEONE_REGULAR = L"OtomanopeeOne-Regular.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_PENGS = L"PENGS.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_PLANETARIUM = L"Planetarium.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_SAWARABIGOTHIC_REGULAR = L"SawarabiGothic-Regular.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_SENOBI_GOTHIC_REGULAR = L"Senobi-Gothic-Regular.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_SMARTFONTUI = L"SmartFontUI.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_SOLID_REGULAR = L"Solid-Regular.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_SOUKOUMINCHO = L"SoukouMincho.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_TIMEMACHINE_WA = L"timemachine-wa.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_TSUNAGIGOTHIC = L"TsunagiGothic.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_UMEBOSHI_ = L"umeboshi_.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_VOYNICH_MONOSPACE = L"Voynich_MonoSpace.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_ZEROGOTHIC = L"ZeroGothic.otf"; 

constexpr const wchar_t* FILE_NAME_PNG_A_LEFT_BTN = L"a_left_btn.png"; 
constexpr const wchar_t* FILE_NAME_JPG_BACK_GROUND = L"back_ground.jpg"; 
constexpr const wchar_t* FILE_NAME_PNG_BLACK1 = L"black1.png"; 
constexpr const wchar_t* FILE_NAME_BMP_BLUE1 = L"blue1.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_C1_ANIM_DOWN = L"c1_anim_down.png"; 
constexpr const wchar_t* FILE_NAME_PNG_C1_ANIM_LEFT = L"c1_anim_left.png"; 
constexpr const wchar_t* FILE_NAME_PNG_C1_ANIM_RIGHT = L"c1_anim_right.png"; 
constexpr const wchar_t* FILE_NAME_PNG_C1_ANIM_UP = L"c1_anim_up.png"; 
constexpr const wchar_t* FILE_NAME_PNG_C2_ANIM_IDLE = L"c2_anim_idle.png"; 
constexpr const wchar_t* FILE_NAME_PNG_C2_ANIM_JUMP = L"c2_anim_jump.png"; 
constexpr const wchar_t* FILE_NAME_PNG_C2_ANIM_RUN = L"c2_anim_run.png"; 
constexpr const wchar_t* FILE_NAME_PNG_C2_ANIM_WALK = L"c2_anim_walk.png"; 
constexpr const wchar_t* FILE_NAME_BMP_CUBE = L"cube.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_CURSOR = L"cursor.png"; 
constexpr const wchar_t* FILE_NAME_BMP_DOTTED_LINE = L"dotted_line.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_D_RIGHT_BTN = L"d_right_btn.png"; 
constexpr const wchar_t* FILE_NAME_PNG_EFCT1 = L"efct1.png"; 
constexpr const wchar_t* FILE_NAME_PNG_EFCT2 = L"efct2.png"; 
constexpr const wchar_t* FILE_NAME_PNG_FIRE = L"fire.png"; 
constexpr const wchar_t* FILE_NAME_BMP_GREEN1 = L"green1.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_HEIGHT_MAP = L"height_map.png"; 
constexpr const wchar_t* FILE_NAME_PNG_LAWN = L"lawn.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PAPER = L"paper.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PIPO_MAP001 = L"pipo-map001.png"; 
constexpr const wchar_t* FILE_NAME_BMP_PLAYER = L"player.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_POINT = L"point.png"; 
constexpr const wchar_t* FILE_NAME_BMP_RED1 = L"red1.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_SAMP_ATTACK = L"samp_attack.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SANDY_BEACH = L"sandy_beach.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SCISSORS = L"scissors.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SIFI_TOWN3_6N = L"Sifi_town3_6n.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SKY01 = L"sky01.png"; 
constexpr const wchar_t* FILE_NAME_PNG_STONE = L"stone.png"; 
constexpr const wchar_t* FILE_NAME_PNG_S_DOWN_BTN = L"s_down_btn.png"; 
constexpr const wchar_t* FILE_NAME_BMP_TEST = L"test.bmp"; 
constexpr const wchar_t* FILE_NAME_JPG_TEST = L"test.jpg"; 
constexpr const wchar_t* FILE_NAME_BMP_TEST2 = L"test2.bmp"; 
constexpr const wchar_t* FILE_NAME_BMP_TEST_MAP_CHIP = L"test_map_chip.bmp"; 
constexpr const wchar_t* FILE_NAME_BMP_WHITE = L"white.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_W_UP_BTN = L"w_up_btn.png"; 

constexpr const wchar_t* FILE_NAME_BMP_BOX_BACK = L"box_back.bmp"; 
constexpr const wchar_t* FILE_NAME_BMP_BOX_DOWN = L"box_down.bmp"; 
constexpr const wchar_t* FILE_NAME_BMP_BOX_FORWORD = L"box_forword.bmp"; 
constexpr const wchar_t* FILE_NAME_BMP_BOX_LEFT = L"box_left.bmp"; 
constexpr const wchar_t* FILE_NAME_BMP_BOX_RIGHT = L"box_right.bmp"; 
constexpr const wchar_t* FILE_NAME_BMP_BOX_UP = L"box_up.bmp"; 

constexpr const wchar_t* FILE_NAME_PNG_PARTICLEA = L"particleA.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PARTICLEB = L"particleB.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PARTICLEC = L"particleC.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PARTICLED = L"particleD.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PARTICLEE = L"particleE.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PARTICLEF = L"particleF.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PARTICLEG = L"particleG.png"; 

constexpr const wchar_t* FILE_NAME_PNG_MAGICAL = L"Magical.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SKYBOX_A = L"skybox_a.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SKYBOX_B = L"skybox_b.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SKYBOX_C = L"skybox_c.png"; 

constexpr const wchar_t* FILE_NAME_BMP_DEC_DECORATION_A = L"dec_decoration_A.bmp"; 
constexpr const wchar_t* FILE_NAME_PNG_DEC_DECORATION_B = L"dec_decoration_B.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PAT_PATTERN_A = L"pat_pattern_A.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PAT_PATTERN_B = L"pat_pattern_B.png"; 

constexpr const wchar_t* FILE_NAME_JSON_EXAMPLE = L"example.json"; 

constexpr const wchar_t* FILE_NAME_MV1_WOODENBRIDGE_01 = L"WoodenBridge_01.mv1"; 
constexpr const wchar_t* FILE_NAME_JPG_WOOD_01 = L"Wood_01.jpg"; 

constexpr const wchar_t* FILE_NAME_MV1_CANOE = L"Canoe.mv1"; 
constexpr const wchar_t* FILE_NAME_JPG_WOODFINE0025_M = L"WoodFine0025_M.jpg"; 

constexpr const wchar_t* FILE_NAME_MV1_BARREL = L"barrel.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_CONE = L"cone.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_CONE_CYLINDER = L"cone_cylinder.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_CUBE = L"cube.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_CYLINDER = L"cylinder.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_DISK = L"disk.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_DOME = L"dome.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_HALF_BARREL = L"half_barrel.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_HOLLOW_OUT_DISK_PLANE = L"hollow_out_disk_plane.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_PLANE = L"plane.mv1"; 
constexpr const wchar_t* FILE_NAME_MV1_SPHERE = L"sphere.mv1"; 

constexpr const wchar_t* FILE_NAME_MTL_SIMPLE_TRIANGLE = L"simple_triangle.mtl"; 
constexpr const wchar_t* FILE_NAME_OBJ_SIMPLE_TRIANGLE = L"simple_triangle.obj"; 

constexpr const wchar_t* FILE_NAME_JPG_F4U1BLET = L"F4U1BLET.JPG"; 
constexpr const wchar_t* FILE_NAME_JPG_F4U1FURT = L"F4U1FURT.JPG"; 
constexpr const wchar_t* FILE_NAME_JPG_F4U1TLB = L"F4U1TLB.JPG"; 
constexpr const wchar_t* FILE_NAME_JPG_F4U1TLLT = L"F4U1TLLT.JPG"; 
constexpr const wchar_t* FILE_NAME_JPG_F4U1TLRT = L"F4U1TLRT.JPG"; 
constexpr const wchar_t* FILE_NAME_JPG_F4U1WHNT = L"F4U1WHNT.JPG"; 
constexpr const wchar_t* FILE_NAME_JPG_F4U1WTLT = L"F4U1WTLT.JPG"; 
constexpr const wchar_t* FILE_NAME_OBJ_F4U1_L = L"F4U1_L.obj"; 
constexpr const wchar_t* FILE_NAME_OBJ_MTL_F4U1_L = L"F4U1_L.obj.mtl"; 

constexpr const wchar_t* FILE_NAME_X_TEST = L"test.x"; 

constexpr const wchar_t* FILE_NAME_BMP_BOX = L"box.bmp"; 
constexpr const wchar_t* FILE_NAME_X_BOX = L"box.x"; 

constexpr const wchar_t* FILE_NAME_BMP_PLANE = L"plane.bmp"; 
constexpr const wchar_t* FILE_NAME_X_PLANE = L"plane.x"; 
constexpr const wchar_t* FILE_NAME_X_TEMP = L"temp.x"; 

constexpr const wchar_t* FILE_NAME_BMP_SPHERE = L"sphere.bmp"; 
constexpr const wchar_t* FILE_NAME_X_SPHERE = L"sphere.x"; 
constexpr const wchar_t* FILE_NAME_BMP_SPHERE1 = L"sphere1.bmp"; 
constexpr const wchar_t* FILE_NAME_X_SPHERE2 = L"sphere2.x"; 

constexpr const wchar_t* FILE_NAME_PSO_EREA_PROJECTION = L"erea_projection.pso"; 
constexpr const wchar_t* FILE_NAME_VSO_EREA_PROJECTION = L"erea_projection.vso"; 

constexpr const wchar_t* FILE_NAME_MP3_BGM_SAMPLE_0 = L"bgm_sample_0.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_SE_SAMPLE_0 = L"se_sample_0.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_SE_SAMPLE_1 = L"se_sample_1.mp3"; 
constexpr const wchar_t* FILE_NAME_WAV_SE_SAMPLE_2 = L"se_sample_2.wav"; 

constexpr const wchar_t* FILE_NAME_TXT_SAMPLE_TEXT = L"sample_text.txt"; 



//------------------------------------------------------------------------------------------------------------------
// 
// file name table 
// 


const std::vector<const wchar_t*> FILE_NAME_TBL_CSV = {
    FILE_NAME_CSV_EXAMPLE,
    FILE_NAME_CSV_EXAMPLE_MAP_CHIP,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_DXE_MAP_EDITOR_DATA_SAMPLE_MAP1 = {
    FILE_NAME_PNG_MAP1_ANIM_CHIPS_32,
    FILE_NAME_CSV_MAP1_ANIM_CHIP_INFO_32,
    FILE_NAME_CSV_MAP1_COLLISION_LAYER_0,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_0,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_1,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_2,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_3,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_4,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_5,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_6,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_7,
    FILE_NAME_CSV_MAP1_LANDSCAPE_LAYER_8,
    FILE_NAME_CSV_MAP1_LAYER_SETTINGS,
    FILE_NAME_PNG_MAP1_STIL_CHIPS_128,
    FILE_NAME_PNG_MAP1_STIL_CHIPS_32,
    FILE_NAME_PNG_MAP1_STIL_CHIPS_96,
    FILE_NAME_CSV_MAP1_STIL_CHIP_INFO_128,
    FILE_NAME_CSV_MAP1_STIL_CHIP_INFO_32,
    FILE_NAME_CSV_MAP1_STIL_CHIP_INFO_96,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_DXE_PARAMETERS_DIRECTIONAL_LIGHT = {
    FILE_NAME_BIN_DIRECTIONAL_LIGHT_PARAMETER,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_DXE_PARAMETERS_GRAPHICS_LAYER = {
    FILE_NAME_BIN_GRAPHICS_LAYER_STATUS,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_DXE_PARAMETERS_MESH_MATERIAL = {
    FILE_NAME_BIN_MATERIAL,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_DXE_PARAMETERS_PARTICLE_PRESET = {
    FILE_NAME_BIN_CONVERGENCE,
    FILE_NAME_BIN_DEFAULT,
    FILE_NAME_BIN_EXPLOSION,
    FILE_NAME_BIN_FIRE,
    FILE_NAME_BIN_FIREWORKS,
    FILE_NAME_BIN_RAIN,
    FILE_NAME_BIN_RAIN_GROUND,
    FILE_NAME_BIN_SMOKE,
    FILE_NAME_BIN_SNOW,
    FILE_NAME_BIN_WATER_FALL,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_DXE_PARAMETERS_SKYBOX_MATERIAL = {
    FILE_NAME_BIN_SKYBOX_MATERIAL,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_DXE_PARAMETERS_WATER_PLANE = {
    FILE_NAME_BIN_WATER_PLANE,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_FONT = {
    FILE_NAME_OTF_03TAKIBI_REGULAR,
    FILE_NAME_OTF_06TAKIBI_BOLD,
    FILE_NAME_TTF_AKABARA_CINDERELLA,
    FILE_NAME_OTF_APPLIMINCHO,
    FILE_NAME_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3,
    FILE_NAME_OTF_CORPORATE_MINCHO_VER3,
    FILE_NAME_TTF_DIGITALISM,
    FILE_NAME_TTF_DS_DIGIB,
    FILE_NAME_TTF_GENKAI_MINCHO,
    FILE_NAME_TTF_HANGYAKU_XRRWQ,
    FILE_NAME_OTF_HARENOSORA,
    FILE_NAME_OTF_HONOKA_SHIN_MARU_GOTHIC_R,
    FILE_NAME_OTF_HONOKA_SHIN_MINCHO_L,
    FILE_NAME_TTF_IPAEXM,
    FILE_NAME_OTF_KOWAIFONT,
    FILE_NAME_TTF_KUROBARA_CINDERELLA,
    FILE_NAME_TTF_LED_DIGITAL_7,
    FILE_NAME_OTF_LIGHTNOVELPOPV2,
    FILE_NAME_TTF_MAGICRING,
    FILE_NAME_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS,
    FILE_NAME_TTF_MOBILEFONT,
    FILE_NAME_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT,
    FILE_NAME_TTF_NEKO_NO_MEZAME,
    FILE_NAME_TTF_OTOMANOPEEONE_REGULAR,
    FILE_NAME_TTF_PENGS,
    FILE_NAME_OTF_PLANETARIUM,
    FILE_NAME_TTF_SAWARABIGOTHIC_REGULAR,
    FILE_NAME_TTF_SENOBI_GOTHIC_REGULAR,
    FILE_NAME_OTF_SMARTFONTUI,
    FILE_NAME_OTF_SOLID_REGULAR,
    FILE_NAME_TTF_SOUKOUMINCHO,
    FILE_NAME_TTF_TIMEMACHINE_WA,
    FILE_NAME_TTF_TSUNAGIGOTHIC,
    FILE_NAME_TTF_UMEBOSHI_,
    FILE_NAME_TTF_VOYNICH_MONOSPACE,
    FILE_NAME_OTF_ZEROGOTHIC,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_GRAPHICS_EXAMPLE = {
    FILE_NAME_PNG_A_LEFT_BTN,
    FILE_NAME_JPG_BACK_GROUND,
    FILE_NAME_PNG_BLACK1,
    FILE_NAME_BMP_BLUE1,
    FILE_NAME_PNG_C1_ANIM_DOWN,
    FILE_NAME_PNG_C1_ANIM_LEFT,
    FILE_NAME_PNG_C1_ANIM_RIGHT,
    FILE_NAME_PNG_C1_ANIM_UP,
    FILE_NAME_PNG_C2_ANIM_IDLE,
    FILE_NAME_PNG_C2_ANIM_JUMP,
    FILE_NAME_PNG_C2_ANIM_RUN,
    FILE_NAME_PNG_C2_ANIM_WALK,
    FILE_NAME_BMP_CUBE,
    FILE_NAME_PNG_CURSOR,
    FILE_NAME_BMP_DOTTED_LINE,
    FILE_NAME_PNG_D_RIGHT_BTN,
    FILE_NAME_PNG_EFCT1,
    FILE_NAME_PNG_EFCT2,
    FILE_NAME_PNG_FIRE,
    FILE_NAME_BMP_GREEN1,
    FILE_NAME_PNG_HEIGHT_MAP,
    FILE_NAME_PNG_LAWN,
    FILE_NAME_PNG_PAPER,
    FILE_NAME_PNG_PIPO_MAP001,
    FILE_NAME_BMP_PLAYER,
    FILE_NAME_PNG_POINT,
    FILE_NAME_BMP_RED1,
    FILE_NAME_PNG_SAMP_ATTACK,
    FILE_NAME_PNG_SANDY_BEACH,
    FILE_NAME_PNG_SCISSORS,
    FILE_NAME_PNG_SIFI_TOWN3_6N,
    FILE_NAME_PNG_SKY01,
    FILE_NAME_PNG_STONE,
    FILE_NAME_PNG_S_DOWN_BTN,
    FILE_NAME_BMP_TEST,
    FILE_NAME_JPG_TEST,
    FILE_NAME_BMP_TEST2,
    FILE_NAME_BMP_TEST_MAP_CHIP,
    FILE_NAME_BMP_WHITE,
    FILE_NAME_PNG_W_UP_BTN,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_GRAPHICS_EXAMPLE_BOX = {
    FILE_NAME_BMP_BOX_BACK,
    FILE_NAME_BMP_BOX_DOWN,
    FILE_NAME_BMP_BOX_FORWORD,
    FILE_NAME_BMP_BOX_LEFT,
    FILE_NAME_BMP_BOX_RIGHT,
    FILE_NAME_BMP_BOX_UP,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_GRAPHICS_EXAMPLE_PARTICLE = {
    FILE_NAME_PNG_PARTICLEA,
    FILE_NAME_PNG_PARTICLEB,
    FILE_NAME_PNG_PARTICLEC,
    FILE_NAME_PNG_PARTICLED,
    FILE_NAME_PNG_PARTICLEE,
    FILE_NAME_PNG_PARTICLEF,
    FILE_NAME_PNG_PARTICLEG,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_GRAPHICS_EXAMPLE_SKYBOX = {
    FILE_NAME_PNG_MAGICAL,
    FILE_NAME_PNG_SKYBOX_A,
    FILE_NAME_PNG_SKYBOX_B,
    FILE_NAME_PNG_SKYBOX_C,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_GRAPHICS_EXAMPLE_WATER_PLANE = {
    FILE_NAME_BMP_DEC_DECORATION_A,
    FILE_NAME_PNG_DEC_DECORATION_B,
    FILE_NAME_PNG_PAT_PATTERN_A,
    FILE_NAME_PNG_PAT_PATTERN_B,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_JSON = {
    FILE_NAME_JSON_EXAMPLE,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_MV_BRIDGE = {
    FILE_NAME_MV1_WOODENBRIDGE_01,
    FILE_NAME_JPG_WOOD_01,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_MV_CANOE = {
    FILE_NAME_MV1_CANOE,
    FILE_NAME_JPG_WOODFINE0025_M,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_MV_PRESET = {
    FILE_NAME_MV1_BARREL,
    FILE_NAME_MV1_CONE,
    FILE_NAME_MV1_CONE_CYLINDER,
    FILE_NAME_MV1_CUBE,
    FILE_NAME_MV1_CYLINDER,
    FILE_NAME_MV1_DISK,
    FILE_NAME_MV1_DOME,
    FILE_NAME_MV1_HALF_BARREL,
    FILE_NAME_MV1_HOLLOW_OUT_DISK_PLANE,
    FILE_NAME_MV1_PLANE,
    FILE_NAME_MV1_SPHERE,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_OBJ = {
    FILE_NAME_MTL_SIMPLE_TRIANGLE,
    FILE_NAME_OBJ_SIMPLE_TRIANGLE,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_OBJ_26F4U1_JET = {
    FILE_NAME_JPG_F4U1BLET,
    FILE_NAME_JPG_F4U1FURT,
    FILE_NAME_JPG_F4U1TLB,
    FILE_NAME_JPG_F4U1TLLT,
    FILE_NAME_JPG_F4U1TLRT,
    FILE_NAME_JPG_F4U1WHNT,
    FILE_NAME_JPG_F4U1WTLT,
    FILE_NAME_OBJ_F4U1_L,
    FILE_NAME_OBJ_MTL_F4U1_L,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_X_FILE = {
    FILE_NAME_X_TEST,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_X_FILE_BOX = {
    FILE_NAME_BMP_BOX,
    FILE_NAME_X_BOX,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_X_FILE_PLANE = {
    FILE_NAME_BMP_PLANE,
    FILE_NAME_X_PLANE,
    FILE_NAME_X_TEMP,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_MESH_X_FILE_SPHERE = {
    FILE_NAME_BMP_SPHERE,
    FILE_NAME_X_SPHERE,
    FILE_NAME_BMP_SPHERE1,
    FILE_NAME_X_SPHERE2,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_SHADER_DXE = {
    FILE_NAME_PSO_EREA_PROJECTION,
    FILE_NAME_VSO_EREA_PROJECTION,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_SOUND_EXAMPLE = {
    FILE_NAME_MP3_BGM_SAMPLE_0,
    FILE_NAME_MP3_SE_SAMPLE_0,
    FILE_NAME_MP3_SE_SAMPLE_1,
    FILE_NAME_WAV_SE_SAMPLE_2,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_TEXT = {
    FILE_NAME_TXT_SAMPLE_TEXT,
};


//------------------------------------------------------------------------------------------------------------------
// 
// font name 
// 


constexpr const wchar_t* FONT_NAME_03TAKIBI_REGULAR = L"03Takibi-Regular"; 
constexpr const wchar_t* FONT_NAME_06TAKIBI_BOLD = L"06Takibi-Bold"; 
constexpr const wchar_t* FONT_NAME_AKABARA_CINDERELLA = L"akabara-cinderella"; 
constexpr const wchar_t* FONT_NAME_APPLIMINCHO = L"AppliMincho"; 
constexpr const wchar_t* FONT_NAME_CORPORATE_LOGO_ROUNDED_VER3 = L"Corporate Logo Rounded ver3"; 
constexpr const wchar_t* FONT_NAME_CORPORATE_MINCHO_VER3 = L"Corporate Mincho ver3"; 
constexpr const wchar_t* FONT_NAME_DIGITALISM = L"Digitalism"; 
constexpr const wchar_t* FONT_NAME_GENKAIMINCHO = L"Genkaimincho"; 
constexpr const wchar_t* FONT_NAME_HANGYAKU = L"Hangyaku"; 
constexpr const wchar_t* FONT_NAME_HARENOSORAMINCHO = L"HarenosoraMincho"; 
constexpr const wchar_t* FONT_NAME_HONOKA_SHIN_MARU_GOTHIC = L"Honoka Shin Maru-Gothic"; 
constexpr const wchar_t* FONT_NAME_HONOKA_SHIN_MINCHO = L"Honoka Shin Mincho"; 
constexpr const wchar_t* FONT_NAME_IPAEXMINCHO = L"IPAexMincho"; 
constexpr const wchar_t* FONT_NAME_07REALLYSCARYMINCHOTAI = L"07ReallyScaryMinchotai"; 
constexpr const wchar_t* FONT_NAME_KUROBARA_CINDERELLA = L"kurobara-cinderella"; 
constexpr const wchar_t* FONT_NAME_LED_DIGITAL_7 = L"LED Digital 7"; 
constexpr const wchar_t* FONT_NAME_LIGHTNOVELPOPV2 = L"LightNovelPopV2"; 
constexpr const wchar_t* FONT_NAME_MAGICRING = L"MagicRing"; 
constexpr const wchar_t* FONT_NAME_SAMURAI_SPIRITS = L"samurai_spirits"; 
constexpr const wchar_t* FONT_NAME_MOBILE_FONT = L"Mobile Font"; 
constexpr const wchar_t* FONT_NAME_NAGOMI_GOKUBOSO_GOTHIC = L"Nagomi Gokuboso Gothic"; 
constexpr const wchar_t* FONT_NAME_NEKO_NO_MEZAME = L"Neko_no_Mezame"; 
constexpr const wchar_t* FONT_NAME_OTOMANOPEE_ONE = L"Otomanopee One"; 
constexpr const wchar_t* FONT_NAME_PENGS = L"PENGS"; 
constexpr const wchar_t* FONT_NAME_PLANETARIUM = L"Planetarium"; 
constexpr const wchar_t* FONT_NAME_SAWARABI_GOTHIC = L"Sawarabi Gothic"; 
constexpr const wchar_t* FONT_NAME_SENOBI_GOTHIC = L"Senobi Gothic"; 
constexpr const wchar_t* FONT_NAME_03SMARTFONTUI = L"03SmartFontUI"; 
constexpr const wchar_t* FONT_NAME_SOLID = L"Solid"; 
constexpr const wchar_t* FONT_NAME_SOUKOUMINCHO = L"SoukouMincho"; 
constexpr const wchar_t* FONT_NAME_TIMEMACHINE_WA = L"timemachine wa"; 
constexpr const wchar_t* FONT_NAME_TSUNAGI_GOTHIC_BLACK = L"Tsunagi Gothic Black"; 
constexpr const wchar_t* FONT_NAME_UMEBOSHI = L"umeboshi"; 
constexpr const wchar_t* FONT_NAME_VOYNICH_SPACE = L"Voynich_Space"; 
constexpr const wchar_t* FONT_NAME_ZEROGOTHIC = L"ZeroGothic"; 


//------------------------------------------------------------------------------------------------------------------
// 
// font name table 
// 


const std::vector<const wchar_t*> FONT_NAME_TABLE = { 
     FONT_NAME_03TAKIBI_REGULAR,
     FONT_NAME_06TAKIBI_BOLD,
     FONT_NAME_AKABARA_CINDERELLA,
     FONT_NAME_APPLIMINCHO,
     FONT_NAME_CORPORATE_LOGO_ROUNDED_VER3,
     FONT_NAME_CORPORATE_MINCHO_VER3,
     FONT_NAME_DIGITALISM,
     FONT_NAME_GENKAIMINCHO,
     FONT_NAME_HANGYAKU,
     FONT_NAME_HARENOSORAMINCHO,
     FONT_NAME_HONOKA_SHIN_MARU_GOTHIC,
     FONT_NAME_HONOKA_SHIN_MINCHO,
     FONT_NAME_IPAEXMINCHO,
     FONT_NAME_07REALLYSCARYMINCHOTAI,
     FONT_NAME_KUROBARA_CINDERELLA,
     FONT_NAME_LED_DIGITAL_7,
     FONT_NAME_LIGHTNOVELPOPV2,
     FONT_NAME_MAGICRING,
     FONT_NAME_SAMURAI_SPIRITS,
     FONT_NAME_MOBILE_FONT,
     FONT_NAME_NAGOMI_GOKUBOSO_GOTHIC,
     FONT_NAME_NEKO_NO_MEZAME,
     FONT_NAME_OTOMANOPEE_ONE,
     FONT_NAME_PENGS,
     FONT_NAME_PLANETARIUM,
     FONT_NAME_SAWARABI_GOTHIC,
     FONT_NAME_SENOBI_GOTHIC,
     FONT_NAME_03SMARTFONTUI,
     FONT_NAME_SOLID,
     FONT_NAME_SOUKOUMINCHO,
     FONT_NAME_TIMEMACHINE_WA,
     FONT_NAME_TSUNAGI_GOTHIC_BLACK,
     FONT_NAME_UMEBOSHI,
     FONT_NAME_VOYNICH_SPACE,
     FONT_NAME_ZEROGOTHIC,
};


//------------------------------------------------------------------------------------------------------------------
// 
// font name to path map 
// 


const std::unordered_map<std::wstring, const wchar_t*> FONT_NAME_TO_PATH_DICTIONARY = { 
     { std::wstring( FONT_NAME_03TAKIBI_REGULAR ), L"resource/font/03Takibi-Regular.otf" },
     { std::wstring( FONT_NAME_06TAKIBI_BOLD ), L"resource/font/06Takibi-Bold.otf" },
     { std::wstring( FONT_NAME_AKABARA_CINDERELLA ), L"resource/font/akabara-cinderella.ttf" },
     { std::wstring( FONT_NAME_APPLIMINCHO ), L"resource/font/AppliMincho.otf" },
     { std::wstring( FONT_NAME_CORPORATE_LOGO_ROUNDED_VER3 ), L"resource/font/Corporate-Logo-Rounded-Bold-ver3.otf" },
     { std::wstring( FONT_NAME_CORPORATE_MINCHO_VER3 ), L"resource/font/Corporate-Mincho-ver3.otf" },
     { std::wstring( FONT_NAME_DIGITALISM ), L"resource/font/digitalism.ttf" },
     { std::wstring( FONT_NAME_GENKAIMINCHO ), L"resource/font/genkai-mincho.ttf" },
     { std::wstring( FONT_NAME_HANGYAKU ), L"resource/font/Hangyaku-xRRwq.ttf" },
     { std::wstring( FONT_NAME_HARENOSORAMINCHO ), L"resource/font/Harenosora.otf" },
     { std::wstring( FONT_NAME_HONOKA_SHIN_MARU_GOTHIC ), L"resource/font/Honoka-Shin-Maru-Gothic_R.otf" },
     { std::wstring( FONT_NAME_HONOKA_SHIN_MINCHO ), L"resource/font/Honoka_Shin_Mincho_L.otf" },
     { std::wstring( FONT_NAME_IPAEXMINCHO ), L"resource/font/ipaexm.ttf" },
     { std::wstring( FONT_NAME_07REALLYSCARYMINCHOTAI ), L"resource/font/kowaiFont.otf" },
     { std::wstring( FONT_NAME_KUROBARA_CINDERELLA ), L"resource/font/kurobara-cinderella.ttf" },
     { std::wstring( FONT_NAME_LED_DIGITAL_7 ), L"resource/font/led_digital_7.ttf" },
     { std::wstring( FONT_NAME_LIGHTNOVELPOPV2 ), L"resource/font/LightNovelPOPv2.otf" },
     { std::wstring( FONT_NAME_MAGICRING ), L"resource/font/MagicRing.ttf" },
     { std::wstring( FONT_NAME_SAMURAI_SPIRITS ), L"resource/font/Mattsun-Marus_samurai_spirits.ttf" },
     { std::wstring( FONT_NAME_MOBILE_FONT ), L"resource/font/MobileFont.ttf" },
     { std::wstring( FONT_NAME_NAGOMI_GOKUBOSO_GOTHIC ), L"resource/font/NagomiGokubosoGothic-ExtraLight.otf" },
     { std::wstring( FONT_NAME_NEKO_NO_MEZAME ), L"resource/font/Neko_no_Mezame.ttf" },
     { std::wstring( FONT_NAME_OTOMANOPEE_ONE ), L"resource/font/OtomanopeeOne-Regular.ttf" },
     { std::wstring( FONT_NAME_PENGS ), L"resource/font/PENGS.ttf" },
     { std::wstring( FONT_NAME_PLANETARIUM ), L"resource/font/Planetarium.otf" },
     { std::wstring( FONT_NAME_SAWARABI_GOTHIC ), L"resource/font/SawarabiGothic-Regular.ttf" },
     { std::wstring( FONT_NAME_SENOBI_GOTHIC ), L"resource/font/Senobi-Gothic-Regular.ttf" },
     { std::wstring( FONT_NAME_03SMARTFONTUI ), L"resource/font/SmartFontUI.otf" },
     { std::wstring( FONT_NAME_SOLID ), L"resource/font/Solid-Regular.otf" },
     { std::wstring( FONT_NAME_SOUKOUMINCHO ), L"resource/font/SoukouMincho.ttf" },
     { std::wstring( FONT_NAME_TIMEMACHINE_WA ), L"resource/font/timemachine-wa.ttf" },
     { std::wstring( FONT_NAME_TSUNAGI_GOTHIC_BLACK ), L"resource/font/TsunagiGothic.ttf" },
     { std::wstring( FONT_NAME_UMEBOSHI ), L"resource/font/umeboshi_.ttf" },
     { std::wstring( FONT_NAME_VOYNICH_SPACE ), L"resource/font/Voynich_MonoSpace.ttf" },
     { std::wstring( FONT_NAME_ZEROGOTHIC ), L"resource/font/ZeroGothic.otf" },
};
