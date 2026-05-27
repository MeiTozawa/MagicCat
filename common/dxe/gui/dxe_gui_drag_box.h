#pragma once
#include <functional>
#include <algorithm>
#include "../../tnl/tnl_vector2i.h"
#include "../../tnl/tnl_util.h"

namespace dxe {

    class GuiDragBox {
    public:

        GuiDragBox(const std::function<void(const std::wstring& drag_data_path)>& draged_function);

        TNL_PROPERTY(int32_t, Width, width_);
        TNL_PROPERTY(int32_t, Height, height_);
        TNL_PROPERTY(tnl::Vector2i, Position, pos_);
        TNL_PROPERTY(std::wstring, Label, label_);
        TNL_PROPERTY(std::function<void()>, Description, description_function_);

        void update();
        void draw();

    private:
        int32_t width_ = 100;
        int32_t height_ = 30;
        tnl::Vector2i pos_;
        std::wstring label_;
        bool is_forcus_ = false;
        std::function<void(const std::wstring& drag_data_path)> draged_function_;
        std::function<void()> description_function_ = nullptr;
    };



}