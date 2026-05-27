#pragma once
#include <DirectXMath.h>
#include <string>
#include "tnl_util.h"

namespace tnl {

	class Vector3;
	class Vector2i;
	class Vector2f final : public DirectX::XMFLOAT2 {
	public :
		Vector2f() noexcept : DirectX::XMFLOAT2(0, 0) {}
		Vector2f(const float xx, const float yy) noexcept : DirectX::XMFLOAT2(xx, yy) {}
		explicit Vector2f(int32_t xy) noexcept : DirectX::XMFLOAT2((float)xy, (float)xy) {}
		explicit Vector2f(float xy) noexcept : DirectX::XMFLOAT2(xy, xy) {}
		explicit Vector2f(DirectX::XMVECTOR& v) noexcept { DirectX::XMStoreFloat2(this, v); }
		explicit Vector2f(DirectX::XMFLOAT2& v) noexcept : DirectX::XMFLOAT2(v.x, v.y) {}
		explicit Vector2f(const tnl::Vector3& v) noexcept;
		explicit Vector2f(const tnl::Vector2i& v) noexcept;

		static const Vector2f left(){ static const Vector2f instance(-1, 0); return instance; }
		static const Vector2f right(){ static const Vector2f instance(1, 0); return instance; }
		static const Vector2f up(){ static const Vector2f instance(0, -1); return instance; }
		static const Vector2f down() { static const Vector2f instance(0, 1); return instance; }

		//-----------------------------------------------------------------------------------------------------
		//
		// operator
		//

		bool operator == (const Vector2f& other) noexcept;
		bool operator != (const Vector2f& other) noexcept;

		Vector2f& operator = (DirectX::XMVECTOR& other) noexcept;
		Vector2f& operator = (const float other) noexcept;

		Vector2f operator * (const float other) const noexcept;
		Vector2f operator * (const Vector2f& other) const noexcept;
		Vector2f& operator *= (const float other) noexcept;
		Vector2f& operator *= (const Vector2f& other) noexcept;

		Vector2f operator / (const float other) const noexcept;
		Vector2f operator / (const Vector2f& other) const noexcept;
		Vector2f& operator /= (const float other) noexcept;
		Vector2f& operator /= (const Vector2f& other) noexcept;

		Vector2f operator + (const Vector2f& other) const noexcept;
		Vector2f& operator += (const Vector2f& other) noexcept;
		Vector2f operator - (const Vector2f& other) const noexcept;
		Vector2f& operator -= (const Vector2f& other) noexcept;
		Vector2f operator - () const noexcept;

		//-----------------------------------------------------------------------------------------------------
		//
		// inline function
		//
		void abs() noexcept;
		void normalize() noexcept;
		float length() const noexcept;
		float dot(const Vector2f& v) noexcept;
		Vector2f cross(const Vector2f& v) const noexcept;
		float angle(const Vector2f& v) const noexcept;
		std::wstring toString(const std::wstring& format = L"%f") const noexcept;

		//-----------------------------------------------------------------------------------------------------
		//
		// static inline function
		//
		static Vector2f Abs( const Vector2f& v ) noexcept;
		static Vector2f	Normalize(const Vector2f& v) noexcept;
		static float	Dot(const tnl::Vector2f& v1, const tnl::Vector2f& v2) noexcept;
		static Vector2f	Cross(const tnl::Vector2f& v1, const tnl::Vector2f& v2) noexcept;
		static Vector2f	Reflection(const tnl::Vector2f& in, const tnl::Vector2f& normal) noexcept;


		// 等間隔な値の補間
		// arg1... 始値
		// arg2... 終値
		// arg3... 補間補間完了までの時間
		// arg4... 経過時間
		// ret.... 補間値
		static Vector2f UniformLerp(const Vector2f& s, const Vector2f& e, float time_limit, float ct) noexcept;

		// sin 波 -90 ~ +90度までを利用した値の補間
		// arg1... 始値
		// arg2... 終値
		// arg3... 補間完了までの時間
		// arg4... 経過時間
		// arg5... 内部で時間に対する補間を行う回数 ( デフォルトは0回 最大5回 )
		// ret.... 補間値
		// tips... 通常の sin 波による補間では間隔がやや単調だと思う場合
		// ....... arg5 の引数で補間の間隔を広げ、よりエッジの効いた補間を行うことができます
		static Vector2f SmoothLerp(const Vector2f& s, const Vector2f& e, float time_limit, float ct, int strength = 0) noexcept;

		// 等加速直線運動を利用した値の補間
		// arg1... 始値
		// arg2... 終値
		// arg3... 補間完了までの時間
		// arg4... 経過時間
		// arg5... 内部で時間に対する補間を行う回数 ( デフォルトは0回 最大5回 )
		// ret.... 補間値
		// tips... 通常の 等加速度による補間では間隔がやや単調だと思う場合
		// ....... arg5 の引数で指数関数的に速度が上がるよう調整できます
		static Vector2f AccelLerp(const Vector2f& s, const Vector2f& e, float time_limit, float ct, int strength = 0) noexcept;


		// 等減速直線運動を利用した値の補間
		// arg1... 始値
		// arg2... 終値
		// arg3... 補間完了までの時間
		// arg4... 経過時間
		// ret.... 補間値
		static Vector2f DecelLerp(const Vector2f& s, const Vector2f& e, float time_limit, float ct) noexcept;

	};

	//-----------------------------------------------------------------------------------------------------
	inline bool Vector2f::operator == (const Vector2f& other) noexcept {
		return ( this->x == other.x && this->y == other.y  );
	}

	//-----------------------------------------------------------------------------------------------------
	inline bool Vector2f::operator != (const Vector2f& other) noexcept {
		return (this->x != other.x || this->y != other.y);
	}

	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator = (DirectX::XMVECTOR &other) noexcept {
		DirectX::XMStoreFloat2(this, other);
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator = (const float other) noexcept {
		this->x = other;
		this->y = other;
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::operator * (const Vector2f& other) const noexcept {
		return { x * other.x, y * other.y };
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::operator * (const float other) const noexcept {
		return { x * other, y * other };
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator *= (const float other) noexcept {
		*this = *this * other;
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator *= (const Vector2f& other) noexcept {
		Vector2f v = other;
		*this = *this * v;
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::operator / (const float other) const noexcept {
		return { x / other, y / other };
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::operator / (const Vector2f& other) const noexcept {
		return { x / other.x, y / other.y };
	}

	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator /= (const float other) noexcept {
		*this = *this / other;
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator /= (const Vector2f& other) noexcept {
		*this = *this / other;
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::operator + (const Vector2f& other) const noexcept {
		DirectX::XMFLOAT2 v2(this->x + other.x, this->y + other.y);
		return static_cast<Vector2f>(v2);
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator += (const Vector2f& other) noexcept {
		*this = *this + other;
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::operator - (const Vector2f& other) const noexcept {
		DirectX::XMFLOAT2 v2(this->x - other.x, this->y - other.y);
		return static_cast<Vector2f>(v2);
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f& Vector2f::operator -= (const Vector2f& other) noexcept {
		*this = *this - other;
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::operator-() const noexcept {
		Vector2f v = { -x, -y };
		return v;
	}

	//-----------------------------------------------------------------------------------------------------
	inline void Vector2f::abs() noexcept { x = (0 > x) ? -x : x; y = (0 > y) ? -y : y; }

	//-----------------------------------------------------------------------------------------------------
	inline void Vector2f::normalize() noexcept {
		DirectX::XMStoreFloat2(this, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(this)));
	}
	//-----------------------------------------------------------------------------------------------------
	inline float Vector2f::length() const noexcept {
		return DirectX::XMVector2Length(DirectX::XMLoadFloat2(this)).m128_f32[0];
	}

	//-----------------------------------------------------------------------------------------------------
	inline float Vector2f::dot(const Vector2f& v) noexcept {
		return DirectX::XMVector2Dot(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&v)).m128_f32[0];
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::cross(const Vector2f& v) const noexcept {
		XMFLOAT2 f2;
		DirectX::XMStoreFloat2(&f2, DirectX::XMVector2Cross(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&v)));
		return static_cast<Vector2f>(f2);
	}

	//-----------------------------------------------------------------------------------------------------
	inline float Vector2f::angle(const Vector2f& v) const noexcept {
		Vector2f t1 = *this;
		Vector2f t2 = v;
		t1.normalize();
		t2.normalize();
		return acosf(t1.dot(t2));
	}

	//-----------------------------------------------------------------------------------------------------
	inline std::wstring Vector2f::toString(const std::wstring& format) const noexcept {
		std::wstring str = L"[ x : ";
		str += FloatToString(x, format);
		str += L" ] [ y : ";
		str += FloatToString(y, format);
		str += L" ] ";
		return str;
	}

	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::Abs(const Vector2f& v) noexcept {
		Vector2f r = v;
		r.abs();
		return v;
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::Normalize(const Vector2f& v) noexcept {
		Vector2f f2;
		DirectX::XMStoreFloat2(&f2, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&v)));
		return static_cast<Vector2f>(f2);
	}
	//-----------------------------------------------------------------------------------------------------
	inline float Vector2f::Dot(const tnl::Vector2f& v1, const tnl::Vector2f& v2) noexcept {
		return DirectX::XMVector2Dot(DirectX::XMLoadFloat2(&v1), DirectX::XMLoadFloat2(&v2)).m128_f32[0];
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::Cross(const tnl::Vector2f& v1, const tnl::Vector2f& v2) noexcept {
		XMFLOAT2 f2;
		DirectX::XMStoreFloat2(&f2, DirectX::XMVector3Cross(DirectX::XMLoadFloat2(&v1), DirectX::XMLoadFloat2(&v2)));
		return static_cast<Vector2f>(f2);
	}
	//-----------------------------------------------------------------------------------------------------
	inline Vector2f Vector2f::Reflection(const tnl::Vector2f& in, const tnl::Vector2f& normal) noexcept {
		return tnl::Vector2f::Normalize(in - ((normal * tnl::Vector2f::Dot(in, normal) * 2.0f)));
	}


}
