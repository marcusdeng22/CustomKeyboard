#pragma once

#include "Macro.h"
#include "ProfileColor.h"
#include <map>
#include <list>

class Profile {
private:
	LPCSTR programName;	//empty string for default profile, otherwise the application name
public:
	std::map<DWORD, std::list<Macro>> macroMapping;	//each key can trigger a list of macros
	ProfileColor colorProfile;

	Profile(LPCSTR p = "") {
		programName = p;
	}

	void addMacro(DWORD k, std::list<Macro> m) {
		macroMapping.insert({k, m});
	}

	void removeMacro() {
		macroMapping.clear();
	}

	void removeMacro(DWORD k) {
		if (macroMapping.find(k) != macroMapping.end()) {
			macroMapping.erase(k);
		}
	}

	Profile& operator = (const Profile & p) {
		programName = p.programName;
		macroMapping = p.macroMapping;
		colorProfile = p.colorProfile;
		return *this;
	}
};