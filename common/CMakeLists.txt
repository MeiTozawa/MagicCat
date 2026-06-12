add_library(tnl STATIC
    tnl/json11.cpp
    tnl/tnl_input.cpp
    tnl/tnl_intersect.cpp
    tnl/tnl_math.cpp
    tnl/tnl_matrix.cpp
    tnl/tnl_quaternion.cpp
    tnl/tnl_rect.cpp
    tnl/tnl_seek_unit.cpp
    tnl/tnl_state_machine.cpp
    tnl/tnl_task_scheduler.cpp
    tnl/tnl_util.cpp
    tnl/tnl_vector2f.cpp
    tnl/tnl_vector2i.cpp
    tnl/tnl_vector3.cpp
)
target_include_directories(tnl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tnl)

add_library(dxe STATIC
    dxe/dxe.cpp
    dxe/dxe_camera.cpp
    dxe/dxe_data.cpp
    dxe/dxe_font_text.cpp
    dxe/dxe_graphics_layer.cpp
    dxe/dxe_input.cpp
    dxe/dxe_sprite.cpp
    dxe/dxe_texture.cpp
    dxe/effects/dxe_erea_projection.cpp
    dxe/effects/dxe_particle.cpp
    dxe/effects/dxe_particle_sub.cpp
    dxe/effects/dxe_shadow_map.cpp
    dxe/effects/dxe_water_plane.cpp
    dxe/effects/dxe_water_plane_sub.cpp
    dxe/imgui/imgui.cpp
    dxe/imgui/imgui_demo.cpp
    dxe/imgui/imgui_draw.cpp
    dxe/imgui/imgui_impl_dx11.cpp
    dxe/imgui/imgui_impl_win32.cpp
    dxe/imgui/imgui_tables.cpp
    dxe/imgui/imgui_widgets.cpp
    dxe/mesh/dxe_inst_mesh.cpp
    dxe/mesh/dxe_mesh.cpp
    dxe/mesh/dxe_mesh_base.cpp
    dxe/mesh/generate_mesh/dxe_mesh_barrel.cpp
    dxe/mesh/generate_mesh/dxe_mesh_box.cpp
    dxe/mesh/generate_mesh/dxe_mesh_cone.cpp
    dxe/mesh/generate_mesh/dxe_mesh_cone_cylinder.cpp
    dxe/mesh/generate_mesh/dxe_mesh_cube.cpp
    dxe/mesh/generate_mesh/dxe_mesh_cylinder.cpp
    dxe/mesh/generate_mesh/dxe_mesh_disk.cpp
    dxe/mesh/generate_mesh/dxe_mesh_disk_ring.cpp
    dxe/mesh/generate_mesh/dxe_mesh_dome.cpp
    dxe/mesh/generate_mesh/dxe_mesh_from_height_map.cpp
    dxe/mesh/generate_mesh/dxe_mesh_half_barrel.cpp
    dxe/mesh/generate_mesh/dxe_mesh_hollow_out_disk_plane.cpp
    dxe/mesh/generate_mesh/dxe_mesh_obj.cpp
    dxe/mesh/generate_mesh/dxe_mesh_plane.cpp
    dxe/mesh/generate_mesh/dxe_mesh_sphere.cpp
    dxe/mesh/generate_mesh/dxe_mesh_static_group.cpp
    dxe/mesh/generate_mesh/dxe_mesh_torus.cpp
    dxe/mesh/generate_mesh/dxe_mesh_triangle_equilateral.cpp
    dxe/mesh/generate_mesh/dxe_mesh_triangle_isosceles.cpp
    dxe/mesh/generate_mesh/dxe_mesh_triangle_right.cpp
    dxe/OBJLoader.cpp
)
target_include_directories(dxe PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dxe
    ${CMAKE_CURRENT_SOURCE_DIR}/dxe/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/DirectXTex/Inc/DirectXTex
    ${CMAKE_CURRENT_SOURCE_DIR}/DirectXTK/Inc
    ${DXLIB_DIR}
)
target_link_libraries(dxe PUBLIC tnl tweeny)

add_subdirectory(tweeny)
