// PygmentizeCache.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <picosha2.hpp>
#include <json.hpp>
#include "fileutil.hpp"

#include <process.h>

#include <filesystem>
#include <vector>
#include <string>
#include <cstddef>
#include <iostream>

#include <cstdlib>


char const * my_env[] =
{
   "THIS=environment will be",
   "PASSED=to child.exe by the",
   "_SPAWNLE=and",
   "_SPAWNLPE=and",
   "_SPAWNVE=and",
   "_SPAWNVPE=functions",
   NULL
};


std::string computeFileHash(std::string const & filename)
{
  std::size_t numBytesUsed = 12;

  std::vector<unsigned char> hashdigest(picosha2::k_digest_size);

  {
    std::ifstream f(filename, std::ios::binary);
    picosha2::hash256(f, hashdigest.begin(), hashdigest.end());
  }

  std::string const hashSha256 = picosha2::bytes_to_hex_string(hashdigest.begin(), hashdigest.end());

  return hashSha256.substr(hashSha256.size() - (2 * numBytesUsed), std::string::npos);
}


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


std::vector<std::string> parseArgs(int argc, char * argv[])
{
  std::vector<std::string> args;

  for (int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  return args;
}


int main(int argc, char* argv[])
{
  std::vector<std::string> const argsPassThrough = parseArgs(argc, argv);

  if (argsPassThrough.empty())
  {
    std::cout << "Error: Lacking arguments to pass through!\n";
    return -1;
  }


  std::string const dirUserprofile     = getEnvVar("USERPROFILE");
  std::string const dirPygmentizeCache = getEnvVar("ARNE_PYGMENTIZE_CACHE_DIR");

  if (dirUserprofile.empty() || dirPygmentizeCache.empty())
  {
    std::cout << "Error!\n";
    return -1;
  }

  // TODO: Implement more sophisticated argument parsing to determine input and output file
  std::string filenameSrc = argsPassThrough[argsPassThrough.size() - 1];
  std::string filenameTgt = argsPassThrough[argsPassThrough.size() - 2];


  std::string const inpFileHash = computeFileHash(filenameSrc);
  
  std::string const filenameCache = dirPygmentizeCache + "\\" + inpFileHash;



  if (std::filesystem::exists(filenameCache))
  {
    // Just copy cached output
    fileutil::writeVectorToFile(filenameTgt, fileutil::readVectorFromFile(filenameCache));
  }
  else
  {
    std::vector<std::string> const argsPrefix = {
      "cmd.exe",
      "/C",
      "call",
      "\"C:\\Portsoft\\LaTeX\\Pygments\\run-in.bat\"",
      "\"" + dirUserprofile + "\\miniconda3\\envs\\env_pygments\\Scripts\\pygmentize.exe\""
    };


    std::vector<char const *> args;
    args.reserve(argsPrefix.size() + argsPassThrough.size() + 1);

    for (std::string const& arg : argsPrefix) {
      args.push_back(arg.c_str());
    }
    
    for (std::string const& arg : argsPassThrough) {
      args.push_back(arg.c_str());
    }

    args.push_back(nullptr);

    
    //  //  args[argsIdx++] = "-l";
    //  //  args[argsIdx++] = "cpp";
    //  //  args[argsIdx++] = "-f";
    //  //  args[argsIdx++] = "latex";
    //  //  args[argsIdx++] = "-P";
    //  //  args[argsIdx++] = "commandprefix=PYG";
    //  //  args[argsIdx++] = "-F";
    //  //  args[argsIdx++] = "tokenmerge";
    //  //  args[argsIdx++] = "-o";
    //  //  args[argsIdx++] = "C:\\Portsoft\\LaTeX\\Pygments\\TESTI\\OUTPUTBLUBB__Formelsammlung.out.pyg";
    //  //  args[argsIdx++] = "C:\\Portsoft\\LaTeX\\Pygments\\TESTI\\INPUT__Formelsammlung.pyg";


    _spawnvpe(_P_WAIT, args[0], &(args[0]), my_env);


    // If the command ran successful, there should be an output file now that
    // we can save to the cache
    if (std::filesystem::exists(filenameTgt))
    {
      fileutil::writeVectorToFile(filenameCache, fileutil::readVectorFromFile(filenameTgt));
    }
  }

}
