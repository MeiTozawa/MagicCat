module;

#include <RenderUtils.h>

export module InputService;

namespace mc {
    export enum class InputAction
    {
        Up, Down, Left, Right, Confirm, ToggleMenu, MouseClick,
        DrawCard
    };

    export enum class InputContext
    {
        InGame, Menu, Cutscene, VolumeControl
    };

    /// @brief 最後に入力を受け付けたデバイスの種別
    export enum class InputDevice
    {
        Keyboard,
        Mouse,
        Gamepad
    };

    /// @brief 入力（キーボード、マウス等）の監視とコンテキスト管理を行うサービス
    export class IInputService
    {
    public:
        virtual ~IInputService() = default;

        /// @brief 毎フレーム呼び出す。アクティブデバイスの検出など内部状態を更新する
        virtual void Update() = 0;

        /// @brief 指定されたアクションが現在押されているか（押しっぱなし含む）を判定する
        /// @param action アクション
        /// @return 押されている場合はtrue、そうでない場合はfalse
        virtual bool IsPressed(InputAction action) const = 0;

        /// @brief 指定されたアクションが押され続けているか（ホールド中か）を判定する
        /// @param action アクション
        /// @return ホールドされている場合はtrue、そうでない場合はfalse
        virtual bool IsHolding(InputAction action) const = 0;

        /// @brief 指定されたアクションがこのフレームで離されたかを判定する
        /// @param action アクション
        /// @return 離された場合はtrue、そうでない場合はfalse
        virtual bool IsReleased(InputAction action) const = 0;

        /// @brief マウスクリックアクションが発生した際のカーソル座標を取得する
        /// @param action アクション
        /// @return クリックされた画面上の座標（x, y）
        virtual Point<int> OnMouseClick(InputAction action) const = 0;

        /// @brief 最後に入力を検出したデバイス種別を返す
        /// @return Keyboard, Mouse, または Gamepad
        virtual InputDevice GetActiveDevice() const = 0;

        /// @brief 新しい入力コンテキスト（InGame, Menu等）をスタックにプッシュする
        /// 同じコンテキストが連続してプッシュされる場合は無視される
        /// @param context プッシュする入力コンテキスト
        virtual void PushContext(InputContext context) = 0;

        /// @brief 現在の入力コンテキストをポップし、1つ前のコンテキストに戻す
        /// ただし、スタックが空になることは防がれる
        virtual void PopContext() = 0;

        /// @brief コンテキストスタックをクリアし、指定されたコンテキストを強制的に設定する
        /// @param context 設定する入力コンテキスト
        virtual void ClearAndSetContext(InputContext context) = 0;

        /// @brief マウスカーソルの現在座標を取得する
        /// @return カーソルの画面上の座標（x, y）
        virtual Point<int> GetMousePosition() const = 0;
        
        /// @brief マウスカーソルが指定した矩形領域内にあるかを判定する
        /// @param rect 判定する矩形（両端を含む閉区間: x1<=x<=x2, y1<=y<=y2）
        /// @return 領域内にある場合はtrue、そうでない場合はfalse
        bool IsMouseOver(const Rect<int>& rect) const
        {
            const auto pos = GetMousePosition();
            return pos.In(rect);
        }
    };

    export std::unique_ptr<IInputService> CreateInputService();
} // namespace mc
