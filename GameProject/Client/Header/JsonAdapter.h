#pragma once

#pragma push_macro("new")
#undef new
#include "json.hpp"
#pragma pop_macro("new")

#include "Engine_Define.h"
#include "Define.h"
#include "Utils.h"

inline void from_json(const nlohmann::json& j, Engine::_vec3& v)
{
	j.at(0).get_to(v.x);
	j.at(1).get_to(v.y);
	j.at(2).get_to(v.z);
}

inline void to_json(nlohmann::json& j, const Engine::_vec3& v)
{
	j = nlohmann::json::array({ v.x, v.y, v.z });
}

inline void from_json(const nlohmann::json& j, TMapEntity& e)
{
	j.at("type").get_to(e.iType);
	e.wstrEntityName = Utils::Utf8ToWide(j.at("name").get<std::string>());
	j.at("pos").get_to(e.vPos);
}

inline void to_json(nlohmann::json& j, const TMapEntity& e)
{
	j = nlohmann::json{
		{"type", e.iType},
		{"name", Utils::WideToUtf8(e.wstrEntityName)},
		{"pos",  e.vPos}
	};
}