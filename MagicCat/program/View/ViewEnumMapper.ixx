module;

#include <cassert>

export module ViewEnumMapper;

import AssetService;
import CardService;
import Character;

namespace mc
{
    /**
     * @brief ドメイン層のカードタイプをビュー層の画像タイプに安全にマッピングします。
     */
    export EImage ToImage(ECardType type)
    {
        switch (type)
        {
        case ECardType::Rock:     return EImage::Rock;
        case ECardType::Scissors: return EImage::Scissors;
        case ECardType::Paper:    return EImage::Paper;
        case ECardType::Magic:    return EImage::Magic;
        case ECardType::Null:     return EImage::Null;
        default:
            assert(false && "未知のカードタイプを画像に変換しようとしています");
            return EImage::Null;
        }
    }

    /**
     * @brief ドメイン層の攻撃タイプをビュー層の画像タイプに安全にマッピングします。
     */
    export EImage ToImage(EAttackType type)
    {
        switch (type)
        {
        case EAttackType::Rock:     return EImage::Rock;
        case EAttackType::Scissors: return EImage::Scissors;
        case EAttackType::Paper:    return EImage::Paper;
        default:
            assert(false && "未知の攻撃タイプを画像に変換しようとしています");
            return EImage::Null;
        }
    }
}
