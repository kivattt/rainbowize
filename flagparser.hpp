#ifndef FLAG_PARSER
#define FLAG_PARSER

#include <string>
#include <vector>

#define GETBITATINDEX(n,idx) (((n)>>(idx))&1)
#define WITHBITSETATINDEX(n,idx,val) (n ^ ((((n>>idx)&1) ^ val) << idx))

using std::string;
using std::vector;

namespace flagparser{
	struct ValueFlag{
		string valueName="";
		string value="";
	};

	struct FlagList{
		// --name
		vector <string> wordFlags;
		// --name=value
		vector <ValueFlag> valueFlags;
		// -a -b -c -def
		vector <char> characterFlags;
		// Everything else
		vector <string> plainFlags;

		template <class T>
		T::iterator get_flag_position(const string flagName, T &flagList){
			return std::find(flagList.begin(), flagList.end(), flagName);
		}

		vector <ValueFlag>::iterator get_value_flag_position(const string flagName){
			for (unsigned long long i=0; i<valueFlags.size(); i++){
				if (valueFlags[i].valueName == flagName)
					return valueFlags.begin() + i;
			}

			return valueFlags.end();
		}
	};

	FlagList get_flags(const int argc, char *argv[], const bool removeDuplicates=true){
		FlagList out;

		for (int i=1; i<argc; i++){
			string arg = argv[i];

			if (arg.size() == 0)
				continue;

			if ((arg.starts_with(std::string_view("--"))) && (arg.size() >= 5)){
				ValueFlag tmp;

				enum class parser_position{
					ValueName,
					Value
				};

				parser_position pos = parser_position::ValueName;
				for (unsigned long long i=2; i<arg.size(); i++){
					if (pos == parser_position::ValueName){
						if (arg[i] == '='){
							pos = parser_position::Value;
							continue;
						}

						tmp.valueName += arg[i];
					} else if (pos == parser_position::Value){
						tmp.value += arg[i];
					}
				}

				if (pos == parser_position::Value){
					out.valueFlags.push_back(tmp);
					continue;
				}
			}

			if ((arg.starts_with(std::string_view("--")) && (arg.size() >= 3))){
				out.wordFlags.push_back(arg.substr(2, arg.size()-2));
			} else if ((arg[0] == '-') && arg.size() >= 2){
				for (unsigned long long i=1; i<arg.size(); i++)
					out.characterFlags.push_back(arg[i]);
			} else{
				out.plainFlags.push_back(arg);
			}
		}

		if (!removeDuplicates)
			return out;

		if (!out.characterFlags.empty()){
			unsigned long long charactersUsed[4] = {0,0,0,0}; // 256 bits, 1 for every character
			for (unsigned long long i=0; i<out.characterFlags.size(); i++){
				const char chr = out.characterFlags[i];
				// TODO Remove division, replace with lookup table (switch statement?)
				const bool characterUsedBefore = GETBITATINDEX(charactersUsed[unsigned(chr/64.0)], chr%64);

				charactersUsed[unsigned(chr/64.0)] = WITHBITSETATINDEX(charactersUsed[unsigned(chr/64.0)], chr%64, 1);

				if (characterUsedBefore)
					out.characterFlags.erase(out.characterFlags.begin() + i--);
			}
		}

		if (!out.valueFlags.empty()){
			vector <string> flagNamesUsed;
			for (unsigned long long i=out.valueFlags.size(); i-- != 0;){
				// If valueName in flagNamesUsed
				if (std::find(flagNamesUsed.begin(), flagNamesUsed.end(), out.valueFlags[i].valueName) != flagNamesUsed.end())
					out.valueFlags.erase(out.valueFlags.begin() + i);
				else
					flagNamesUsed.push_back(out.valueFlags[i].valueName);
			}
		}

		if (!out.wordFlags.empty()){
			vector <string> wordFlagsUsed;
			for (unsigned long long i=0; i<out.wordFlags.size(); i++){
				// If wordFlag in wordFlagsUsed
				if (std::find(wordFlagsUsed.begin(), wordFlagsUsed.end(), out.wordFlags[i]) != wordFlagsUsed.end())
					out.wordFlags.erase(out.wordFlags.begin() + i--);
				else
					wordFlagsUsed.push_back(out.wordFlags[i]);
			}
		}

		return out;
	}
}

#endif // FLAG_PARSER
