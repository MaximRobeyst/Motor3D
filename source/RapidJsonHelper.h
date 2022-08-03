#pragma once
#include <rapidjson.h>
#include <DirectXMath.h>

#include <stringbuffer.h>
#include <prettywriter.h>

#include <string>

#undef max
#undef min
#include <document.h>

namespace rapidjson
{
	// Serialize Values
	// Basic types
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, int value, std::string name);
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, float value, std::string name);
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, bool value, std::string name);

	// Math types
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, DirectX::XMFLOAT2& value, std::string name);
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, DirectX::XMFLOAT3& value, std::string name);
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, DirectX::XMFLOAT4& value, std::string name);


	// Load Values
	// Basic types
	void Deserialize(int& newValue, const rapidjson::Value& value, std::string name);
	void Deserialize(float& newValue, const rapidjson::Value& value, std::string name);
	void Deserialize(bool& newValue, const rapidjson::Value& value, std::string name);

	// Math types
	void Deserialize(DirectX::XMFLOAT2& newValue, const rapidjson::Value& value, std::string name);
	void Deserialize(DirectX::XMFLOAT3& newValue, const rapidjson::Value& value, std::string name);
	void Deserialize(DirectX::XMFLOAT4& newValue, const rapidjson::Value& value, std::string name);
}

