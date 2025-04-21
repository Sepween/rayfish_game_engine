#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <unordered_map>

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

using namespace std;

class EngineUtils {
public:

	static inline bool configs_initialized = false;
	static inline bool has_render_config = false;
	static inline rapidjson::Document game_config;
	static inline rapidjson::Document render_config;
	static inline unordered_map<string, rapidjson::Document> template_cache;

	static bool InitializeConfigs() {
		if (configs_initialized) return true;

		if (!(filesystem::exists(getResourceFolder()) &&
			filesystem::is_directory(getResourceFolder()))) {
			cout << "error: resources/ missing";
			return false;
		}

		if (!(filesystem::exists(getGameJson()) &&
			filesystem::is_regular_file(getGameJson()))) {
			cout << "error: resources/game.config missing";
			return false;
		}

		ReadJsonFile(getGameJson(), game_config);

		if (filesystem::exists(getRenderJson()) &&
			filesystem::is_regular_file(getRenderJson())) {
			ReadJsonFile(getRenderJson(), render_config);
			has_render_config = true;
		}

		configs_initialized = true;
		return true;
	}

	static rapidjson::Document* GetCachedTemplete(const string& templateName) {
		auto it = template_cache.find(templateName);
		if (it != template_cache.end()) {
			// template already loaded
			return &(it->second);
		}

		string templatePath = getTemplate(templateName);
		if (!(filesystem::exists(templatePath) &&
			filesystem::is_regular_file(templatePath))){
				return nullptr;
		}

		rapidjson::Document& doc = template_cache[templateName];
		ReadJsonFile(templatePath, doc);

		return &template_cache[templateName];
	}

	static void ReadJsonFile(const string& path, rapidjson::Document& out_document)
	{
		FILE* file_pointer = nullptr;
#ifdef _WIN32
		fopen_s(&file_pointer, path.c_str(), "rb");
#else
		file_pointer = fopen(path.c_str(), "rb");
#endif
		char buffer[65536];
		rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
		out_document.ParseStream(stream);
		fclose(file_pointer);

		if (out_document.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
			cout << "error parsing json at [" << path << "]" << endl;
			exit(0);
		}
	}

	static string getResourceFolder() {
		return "resources";
	}

	static string getGameJson() {
		return (filesystem::path(getResourceFolder()) / "game.config").string();
	}

	static string getRenderJson() {
		return (filesystem::path(getResourceFolder()) / "rendering.config").string();
	}

	static string getSceneFolder() {
		return (filesystem::path(getResourceFolder()) / "scenes").string();
	}

	static string getScene(const string& sceneName) {
		return (filesystem::path(getSceneFolder()) / (sceneName + ".scene")).string();
	}

	static string getTemplateFolder() {
		return (filesystem::path(getResourceFolder()) / "actor_templates").string();
	}

	static string getTemplate(const string& tempName) {
		return (filesystem::path(getTemplateFolder()) / (tempName + ".template")).string();
	}

	static string getImagesFolder() {
		return (filesystem::path(getResourceFolder()) / "images").string();
	}

	static string getImage(const string& imgName) {
		return (filesystem::path(getImagesFolder()) / (imgName + ".png")).string();
	}

	static string getFontFolder() {
		return (filesystem::path(getResourceFolder()) / "fonts").string();
	}

	static string getFont(const string& fontName) {
		return (filesystem::path(getFontFolder()) / (fontName + ".ttf")).string();
	}

	static string getAudioFolder() {
		return (filesystem::path(getResourceFolder()) / "audio").string();
	}

	static string getAudioWav(const string& audioName, const string& fileType) {
		return (filesystem::path(getAudioFolder()) / (audioName + fileType)).string();
	}

	static bool loadFromDoc(const rapidjson::Document& doc, const string& memberName, int& inInt) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsInt()) {
			inInt = doc[memberName.c_str()].GetInt();
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Document& doc, const string& memberName, string& inStr) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsString()) {
			inStr = doc[memberName.c_str()].GetString();
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Document& doc, const string& memberName, bool& inBool) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsBool()) {
			inBool = doc[memberName.c_str()].GetBool();
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Document& doc, const string& memberName, char& inChar) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsString() &&
			doc[memberName.c_str()].GetStringLength() == 1) {
			inChar = doc[memberName.c_str()].GetString()[0];
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Document& doc, const string& memberName, float& inFloat) {
		if (doc.HasMember(memberName.c_str())) {
			if (doc[memberName.c_str()].IsFloat()) {
				inFloat = doc[memberName.c_str()].GetFloat();
				return true;
			}
			else if (doc[memberName.c_str()].IsInt()) {
				inFloat = static_cast<float>(doc[memberName.c_str()].GetInt());
				return true;
			}
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Document& doc, const string& memberName, vector<string>& inStrVec) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsArray()) {
			const rapidjson::Value& temp = doc[memberName.c_str()];
			for (rapidjson::SizeType i = 0; i < temp.Size(); i++) {
				if (temp[i].IsString()) {
					inStrVec.push_back(temp[i].GetString());
				}
			}
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Value& doc, const string& memberName, int& inInt) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsInt()) {
			inInt = doc[memberName.c_str()].GetInt();
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Value& doc, const string& memberName, string& inStr) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsString()) {
			inStr = doc[memberName.c_str()].GetString();
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Value& doc, const string& memberName, bool& inBool) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsBool()) {
			inBool = doc[memberName.c_str()].GetBool();
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Value& doc, const string& memberName, char& inChar) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsString() &&
			doc[memberName.c_str()].GetStringLength() == 1) {
			inChar = doc[memberName.c_str()].GetString()[0];
			return true;
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Value& doc, const string& memberName, float& inFloat) {
		if (doc.HasMember(memberName.c_str())) {
			if (doc[memberName.c_str()].IsFloat()) {
				inFloat = doc[memberName.c_str()].GetFloat();
				return true;
			}
			else if (doc[memberName.c_str()].IsInt()) {
				inFloat = static_cast<float>(doc[memberName.c_str()].GetInt());
				return true;
			}
		}
		return false;
	}

	static bool loadFromDoc(const rapidjson::Value& doc, const string& memberName, vector<string>& inStrVec) {
		if (doc.HasMember(memberName.c_str()) && doc[memberName.c_str()].IsArray()) {
			const rapidjson::Value& temp = doc[memberName.c_str()];
			for (rapidjson::SizeType i = 0; i < temp.Size(); i++) {
				if (temp[i].IsString()) {
					inStrVec.push_back(temp[i].GetString());
				}
			}
			return true;
		}
		return false;
	}

	static string obtain_word_after_phrase(const string& input, const string& phrase) {
		size_t pos = input.find(phrase);
		if (pos == string::npos) return "";

		pos += phrase.length();
		while (pos < input.size() && isspace(input[pos])) {
			++pos;
		}

		if (pos == input.size()) return "";

		size_t endPos = pos;
		while (endPos < input.size() && !isspace(input[endPos])) {
			++endPos;
		}

		return input.substr(pos, endPos - pos);
	}

	static void ReportError(const string& actor_name, const luabridge::LuaException& e) {
		string error_message = e.what();

		replace(error_message.begin(), error_message.end(), '\\', '/');

		cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << endl;
	}
};
