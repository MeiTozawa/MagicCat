module;

export module CharacterService;

import Enemy;
import Player;
import EventBus;
import Character;
import ConfigService;

namespace mc
{
    /// @brief キャラクター（プレイヤーおよび敵）の管理を行うサービス
    export class ICharacterService
    {
    public:
        virtual ~ICharacterService() = default;

        /// @brief サービスを初期化し、キャラクターの初期状態をセットアップする
        virtual void Start() = 0;

        /// @brief 敵キャラクターのインスタンスを取得する
        /// @return 敵キャラクターの参照
        virtual Enemy& GetEnemy() = 0;

        /// @brief プレイヤーキャラクターのインスタンスを取得する
        /// @return プレイヤーキャラクターの参照
        virtual Player& GetPlayer() = 0;
    };

    export Shared<ICharacterService> CreateCharacterService(IConfigService* configService);
} // namespace mc
