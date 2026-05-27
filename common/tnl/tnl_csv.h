#pragma once
#include <string>
#include <vector>
#include "tnl_util.h"

namespace tnl {

	class CsvCell {
	public:
		CsvCell(const std::wstring& s) {
			str_ = s;
			try {
				int_ = std::stoi(s);
			}catch (...) {
			}
			try {
				float_ = (float)std::stof(s);
			}
			catch (...) {
			}
			bool_ = (s == L"TRUE" || s == L"true") ? true : false;
		}

		const std::wstring& getString() const noexcept { return str_; }
		int getInt() const noexcept { return int_; }
		float getFloat() const noexcept { return float_; }
		bool getBool() const noexcept { return bool_; }

	private:
		std::wstring str_ = L"";
		int int_ = 0;
		float float_ = 0;
		bool bool_ = false;
	};

	template<typename T>
	struct CsvTraits;
	template<>
	struct CsvTraits<int> {
		static constexpr auto Name = "int";
		static constexpr int Value = 0;
	};
	template<>
	struct CsvTraits<float> {
		static constexpr auto Name = "float";
		static constexpr int Value = 1;
	};
	template<>
	struct CsvTraits<std::wstring> {
		static constexpr auto Name = "string";
		static constexpr int Value = 2;
	};
	template<>
	struct CsvTraits<CsvCell> {
		static constexpr auto Name = "CsvCell";
		static constexpr int Value = 3;
	};

	using CsvCells = std::vector<std::vector<CsvCell>>;


	template< class T = CsvCell >
	void LoadCsvFromStringBuffer(std::vector<std::vector<T>>& out, std::wstring& string_buffer) {

		std::wstring line;
		while (GetStringLine(line, string_buffer)) {
			std::vector<T> data;

			while (1) {
				size_t c = line.find(L",");
				if (c == std::wstring::npos) {
					c = line.find(L"\n");
				}
				std::wstring s = line.substr(0, c);

				if (s.empty()) {
					line = line.substr(c + 1, line.length() - (c + 1));
					if (line.empty() || line == L"/n") break;
					continue;
				}

				if constexpr (CsvTraits<int>::Value == CsvTraits<T>::Value) {
					int value = 0;
					try {
						value = std::atoi(s);
					}catch (...) {}
					data.emplace_back(value);
				}
				if constexpr (CsvTraits<float>::Value == CsvTraits<T>::Value) {
					float value = 0;
					try {
						value = std::atof(s);
					}
					catch (...) {}
					data.emplace_back(value);
				}
				if constexpr (CsvTraits<std::wstring>::Value == CsvTraits<T>::Value) {
					data.emplace_back(std::move(s));
				}
				if constexpr (CsvTraits<CsvCell>::Value == CsvTraits<T>::Value) {
					data.emplace_back(CsvCell(s));
				}

				line = line.substr(c + 1, line.length() - (c + 1));
				if (line.empty() || line == L"/n") break;
			}
			out.emplace_back(std::move(data));
		}

	}

	//----------------------------------------------------------------------------------------------
	// メモリに読み込まれた csv を読み込む
	// arg1... csv がロードされたメモリアドレス
	// arg2... メモリサイズ
	// ret.... テンプレート指定されたデータの２次元配列
	// tips... テンプレート指定できるのは以下の型
	// ....... int
	// ....... float
	// ....... std::string
	// ....... tnl::CsvCell
	// ....... テンプレート指定を省略した場合 tnl::CsvCell になります
	template< class T = CsvCell >
	std::vector<std::vector<T>> LoadCsvFromMemory(void* memory, size_t size) {
		std::vector<std::vector<T>> out;
		size_t fsize = size;

		// UTF-8からワイド文字(UTF-16)への変換
		int wchars_len = MultiByteToWideChar(CP_UTF8, 0, (char*)memory, -1, NULL, 0);
		std::wstring wstr(wchars_len, 0);
		MultiByteToWideChar(CP_UTF8, 0, (char*)memory, -1, &wstr[0], wchars_len);

		// Null文字分を調整
		if (!wstr.empty() && wstr.back() == L'\0') {
			wstr.pop_back();
		}

		LoadCsvFromStringBuffer<T>(out, wstr);
		return out;
	}


	//----------------------------------------------------------------------------------------------
	// Csv Loader
	// arg1... ファイルパス
	// ret.... テンプレート指定されたデータの２次元配列
	// tips... テンプレート指定できるのは以下の型
	// ....... int
	// ....... float
	// ....... std::string
	// ....... tnl::CsvCell
	// ....... テンプレート指定を省略した場合 tnl::CsvCell になります
	template< class T = CsvCell >
	std::vector<std::vector<T>> LoadCsv(const std::wstring& file_path) {

		size_t fsize = tnl::GetFileSize(file_path.c_str());
		if (-1LL == fsize) {
			throw std::runtime_error("LoadCsv : file path error");
		}
		char* fbuff = new char[fsize+1];
		memset(fbuff, 0, sizeof(char)*(fsize + 1));

		FILE* fp = nullptr;
		_wfopen_s(&fp, file_path.c_str(), L"rb");
		if (fp) {
			fread(fbuff, fsize, 1, fp);
			fclose(fp);
		}

		std::vector<std::vector<T>> out = LoadCsvFromMemory<T>(fbuff, fsize);

		delete[] fbuff;
		return out;
	}

}

