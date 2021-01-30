
#include "proclauncher.hpp"

#include <cstdio>
#include <process.h>


namespace util {


void launchProc(std::vector<std::string> args)
{
  char const* my_env[] =
  {
     "THIS=environment will be",
     "PASSED=to child.exe by the",
     "_SPAWNLE=and",
     "_SPAWNLPE=and",
     "_SPAWNVE=and",
     "_SPAWNVPE=functions",
     NULL
  };

  std::vector<char const*> args_cstrvec;
  args_cstrvec.reserve(args.size() + 1);

  for (std::string const& arg : args) {
    args_cstrvec.push_back(arg.c_str());
  }

  args_cstrvec.push_back(nullptr);

  ::_spawnvpe(_P_WAIT, args_cstrvec[0], &(args_cstrvec[0]), my_env);
}


} // namespace util
