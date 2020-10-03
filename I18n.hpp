#pragma once
#include <unordered_map>
#include "FnvHash.hpp"

namespace yi18n
{
	namespace
	{
		std::unordered_map<uint32_t, const wchar_t*> hashToStrMap;

		#pragma pack(push, 1)
		struct YMOData
		{
			uint16_t len;
			struct
			{
				uint32_t hash;
				uint16_t offset;
			} table[1];
		};
		#pragma pack(pop)
	}

	void LoadTranslateData(const YMOData* ymo)
	{
		hashToStrMap.reserve(ymo->len);
		for (uint16_t i = 0; i < ymo->len; ++i)
		{
			const auto hash = ymo->table[i].hash;
			const auto offset = ymo->table[i].offset;
			const auto str = reinterpret_cast<const wchar_t*>(reinterpret_cast<const uint8_t*>(ymo) + offset);
			hashToStrMap.emplace(hash, str);
		}
	}

#ifdef _INC_WINDOWS
	void LoadTranslateDataFromResource(const HINSTANCE hInst)
	{
		const HRSRC hRes = FindResourceW(hInst, MAKEINTRESOURCEW(1), L"YMO");
		if (hRes)
		{
			const HGLOBAL hResData = LoadResource(hInst, hRes);
			if (hResData)
			{
				const auto ymo = reinterpret_cast<const YMOData*>(LockResource(hResData));
				if (ymo)
					LoadTranslateData(ymo);
			}
		}
	}
#endif

	const wchar_t* TranslateWithHash(const wchar_t* const str, const uint32_t hash)
	{
		auto it = hashToStrMap.find(hash);
		if (it != hashToStrMap.end())
			return it->second;
		return str;
	}
}

#define _(str) yi18n::TranslateWithHash(str, std::integral_constant<uint32_t, fnv1a_32(str)>::value)
#define C_(ctxt, str) yi18n::TranslateWithHash(str, std::integral_constant<uint32_t, fnv1a_32(ctxt L"\004" str)>::value)
