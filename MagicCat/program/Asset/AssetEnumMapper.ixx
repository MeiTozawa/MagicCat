module;

#include <cassert>

export module AssetEnumMapper;

import AssetService;
import CardService;
import Character;

namespace mc {
    /// @brief ECardType → EImage 変換
    export EImage ToImage(ECardType type)
    {
        switch (type)
        {
        case ECardType::Rock: return EImage::Rock;
        case ECardType::Scissors: return EImage::Scissors;
        case ECardType::Paper: return EImage::Paper;
        case ECardType::Magic: return EImage::Magic;
        case ECardType::Null: return EImage::Null;
        default:
            assert(false && "未知のカードタイプを画像に変換しようとしています");
            return EImage::Null;
        }
    }

    /// @brief EAttackType → EImage 変換
    export EImage ToImage(EAttackType type)
    {
        switch (type)
        {
        case EAttackType::Rock: return EImage::Rock;
        case EAttackType::Scissors: return EImage::Scissors;
        case EAttackType::Paper: return EImage::Paper;
        default:
            assert(false && "未知の攻撃タイプを画像に変換しようとしています");
            return EImage::Null;
        }
    }

    /// @brief ESprite → ESound（攻撃音）変換。未知の値は ESound::CatAttack を返す
    export ESound SpriteToAttackSound(ESprite sprite)
    {
        switch (sprite)
        {
        case ESprite::MeowingCat:
            return ESound::CatAttack;
        case ESprite::Wolf:
        case ESprite::TimberWolf:
        case ESprite::SnowFox:
        case ESprite::Bunny:
        case ESprite::StinkySkunk:
            return ESound::WolfAttack;
        case ESprite::DaintyPig:
        case ESprite::MadBoar:
        case ESprite::SlowTurtle:
        case ESprite::SpikeyPorcupine:
        case ESprite::CoralCrab:
            return ESound::PigAttack;
        case ESprite::PasturingSheep:
        case ESprite::CluckingChicken:
        case ESprite::TinyChick:
        case ESprite::HonkingGoose:
        case ESprite::CroakingToad:
        case ESprite::LeapingFrog:
            return ESound::SheepAttack;
        default:
            return ESound::CatAttack;
        }
    }
}
