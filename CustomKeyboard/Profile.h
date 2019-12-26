#pragma once

#include "Macro.h"
#include "ProfileColor.h"
#include <map>
#include <list>
#include <memory>

class Profile {
private:
	LPCSTR programName;	//empty string for default profile, otherwise the application name
public:
	//std::map<DWORD, std::list<Macro>> macroMapping;	//each key can trigger a list of macros
	std::map<DWORD, std::shared_ptr<std::list<std::shared_ptr<Macro>>>> macroMapping;

	ProfileColor colorProfile;

	Profile(LPCSTR p = "") {
		programName = p;
	}

	template<typename... Args>
	void addMacro(DWORD k, Args&&... args) {
		if (macroMapping.find(k) == macroMapping.end()) {
			macroMapping.insert({ k, std::make_shared<std::list<std::shared_ptr<Macro>>>() });
		}
		macroMapping[k]->push_back(std::make_shared<Macro>(args...));
	}

	/*void addMacro(DWORD k, std::list<Macro> m) {
		macroMapping.insert({k, m});
	}

	void removeMacro() {
		macroMapping.clear();
	}

	void removeMacro(DWORD k) {
		if (macroMapping.find(k) != macroMapping.end()) {
			macroMapping.erase(k);
		}
	}*/
};