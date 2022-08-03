#include "RapidJsonHelper.h"

void rapidjson::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, int value, std::string name)
{
	writer.Key(name.c_str());
	writer.Int(value);
}

void rapidjson::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, float value, std::string name)
{
	writer.Key(name.c_str());
	writer.Double(static_cast<float>(value));
}

void rapidjson::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, bool value, std::string name)
{
	writer.Key(name.c_str());
	writer.Bool(value);
}

void rapidjson::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, DirectX::XMFLOAT2& value, std::string name)
{
	writer.Key(name.c_str());
	writer.StartArray();
	writer.Double(static_cast<double>(value.x));
	writer.Double(static_cast<double>(value.y));
	writer.EndArray();
}

void rapidjson::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, DirectX::XMFLOAT3& value, std::string name)
{
	writer.Key(name.c_str());
	writer.StartArray();
	writer.Double(static_cast<double>(value.x));
	writer.Double(static_cast<double>(value.y));
	writer.Double(static_cast<double>(value.z));
	writer.EndArray();
}

void rapidjson::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, DirectX::XMFLOAT4& value, std::string name)
{
	writer.Key(name.c_str());
	writer.StartArray();
	writer.Double(static_cast<double>(value.x));
	writer.Double(static_cast<double>(value.y));
	writer.Double(static_cast<double>(value.z));
	writer.Double(static_cast<double>(value.w));
	writer.EndArray();
}

void rapidjson::Deserialize(int& newValue, const rapidjson::Value& value, std::string name)
{
	newValue = value[name.c_str()].GetInt();
}

void rapidjson::Deserialize(float& newValue, const rapidjson::Value& value, std::string name)
{
	newValue = static_cast<float>(value[name.c_str()].GetDouble());
}

void rapidjson::Deserialize(bool& newValue, const rapidjson::Value& value, std::string name)
{
	newValue = value[name.c_str()].GetBool();
}

void rapidjson::Deserialize(DirectX::XMFLOAT2& newValue, const rapidjson::Value& value, std::string name)
{
	newValue.x = static_cast<float>(value[name.c_str()].GetArray()[0].GetDouble());
	newValue.y = static_cast<float>(value[name.c_str()].GetArray()[1].GetDouble());
}

void rapidjson::Deserialize(DirectX::XMFLOAT3& newValue, const rapidjson::Value& value, std::string name)
{
	newValue.x = static_cast<float>(value[name.c_str()].GetArray()[0].GetDouble());
	newValue.y = static_cast<float>(value[name.c_str()].GetArray()[1].GetDouble());
	newValue.z = static_cast<float>(value[name.c_str()].GetArray()[2].GetDouble());
}

void rapidjson::Deserialize(DirectX::XMFLOAT4& newValue, const rapidjson::Value& value, std::string name)
{
	newValue.x = static_cast<float>(value[name.c_str()].GetArray()[0].GetDouble());
	newValue.y = static_cast<float>(value[name.c_str()].GetArray()[1].GetDouble());
	newValue.z = static_cast<float>(value[name.c_str()].GetArray()[2].GetDouble());
	newValue.w = static_cast<float>(value[name.c_str()].GetArray()[3].GetDouble());
}
