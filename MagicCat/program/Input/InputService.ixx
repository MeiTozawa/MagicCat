module;

#include <tnl_vector2i.h>

export module InputService;

namespace mc {
    export enum class InputAction
    {
        IgUp, IgDown, IgLeft, IgRight, IgConfirm, IgToggleMenu, // IgMouseClick,
        IgDrawCard, IgShowRules,

        MenuConfirm = 100,
        MenuCancel,
        MenuMouseClick,
    };

    export enum class InputContext
    {
        InGame, Menu, Cutscene
    };

    /// @brief 入力（キーボード、マウス等）の監視とコンテキスト管理を行うサービス
    export class IInputService
    {
    public:
        virtual ~IInputService() = default;

        /// @brief 指定されたアクションが現在押されているか（押しっぱなし含む）を判定する
        virtual bool IsPressed(InputAction action) const = 0;

        /// @brief 指定されたアクションが押され続けているか（ホールド中か）を判定する
        virtual bool IsHolding(InputAction action) const = 0;

        /// @brief 指定されたアクションがこのフレームで離されたかを判定する
        virtual bool IsReleased(InputAction action) const = 0;

        /// @brief マウスクリックアクションが発生した際のカーソル座標を取得する
        /// @return クリックされた画面上の座標（x, y）
        virtual tnl::Vector2i OnMouseClick(InputAction action) const = 0;

        /// @brief 新しい入力コンテキスト（InGame, Menu等）をスタックにプッシュする
        /// 同じコンテキストが連続してプッシュされる場合は無視される
        virtual void PushContext(InputContext context) = 0;

        /// @brief 現在の入力コンテキストをポップし、1つ前のコンテキストに戻す
        /// ただし、スタックが空になることは防がれる
        virtual void PopContext() = 0;

        /// @brief コンテキストスタックをクリアし、指定されたコンテキストを強制的に設定する
        virtual void ClearAndSetContext(InputContext context) = 0;
    };

    export std::unique_ptr<IInputService> CreateInputService();
} // namespace mc
