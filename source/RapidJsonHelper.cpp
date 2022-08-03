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

void rapidjson::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, DirectX::XMFLOAT3& value, std::string name)
{
	writer.Key(name.c_str());
	writer.StartArray();
	writer.Double(static_cast<double>(value.x));
	writer.Double(static_cast<double>(value.y));
	writer.Double(static_cast<double>(value.z));
	writer.EndArray();
}

void rapidjson::Deserialize(DirectX::XMFLOAT3& newValue, const rapidjson::Value& value, std::string name)
{
	newValue.x = static_cast<float>(value[name.c_str()].GetArray()[0].GetDouble());
	newValue.y = static_cast<float>(value[name.c_str()].GetArray()[1].GetDouble());
	newValue.z = static_cast<float>(value[name.c_str()].GetArray()[2].GetDouble());
}
