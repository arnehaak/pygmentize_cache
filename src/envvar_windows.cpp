
#include "envvar.hpp"

#include <cstdlib>



namespace util {


std::string getEnvVar(std::string const& envVarName)
{
  std::string retVal;

  char* buf = nullptr;
  size_t sz = 0;
  if (_dupenv_s(&buf, &sz, envVarName.c_str()) == 0 && buf != nullptr)
  {
    retVal = buf;
    free(buf);
  }

  return retVal;
}


}
