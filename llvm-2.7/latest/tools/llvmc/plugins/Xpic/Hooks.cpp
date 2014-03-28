#include <string>
#include <vector>
#include <cstdlib>
#include "llvm/Config/config.h"

namespace hooks {
typedef std::vector<std::string> StrVec;

/// ConvertToMAttr - Convert -m* and -mno-* to -mattr=+*,-*
std::string ConvertToMAttr(const StrVec& Opts) {
  std::string out("-mattr=");

  bool firstIter = true;
  for (StrVec::const_iterator B = Opts.begin(), E = Opts.end(); B!=E; ++B) {
    const std::string& Arg = *B;

    if (firstIter)
      firstIter = false;
    else
      out += ",";

    if (Arg.find("no-") == 0 && Arg[3] != 0) {
      out += '-';
      out += Arg.c_str() + 3;
    }
    else {
      out += '+';
      out += Arg;
    }
  }

  return out;
}

std::string MakeSysLibPath(const char* InternLibPath) {
	const char *envpath = std::getenv("PATH_GNU_XPIC");

	if ( envpath == NULL ) {
		envpath = LLVM_PREFIX;
	}

	if ( InternLibPath == NULL ) {
		return std::string("-L") + std::string(envpath);
	} else {
		return std::string("-L") + std::string(envpath) + std::string(InternLibPath);
	}
}

}
