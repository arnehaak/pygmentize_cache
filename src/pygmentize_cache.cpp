// PygmentizeCache.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "config.hpp"
#include "fileutil.hpp"
#include "envvar.hpp"
#include "proclauncher.hpp"

#include <filesystem>
#include <vector>
#include <string>
#include <cstddef>
#include <iostream>



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
  std::string const configFile = util::getEnvVar("PYGCACHE_CONF_FILE");
  
  if (configFile.empty())
  {
    std::cout << "Please set environment variable PYGCACHE_CONF_FILE!\n";
    return -1;
  }
  
  pygcache::Config const config(configFile);

  
  std::vector<std::string> const argsPassThrough = parseArgs(argc, argv);

  if (argsPassThrough.empty())
  {
    std::cout << "Error: Lacking arguments to pass through!\n";
    return -1;
  }  


  // TODO: Implement more sophisticated argument parsing to determine input and output file
  std::string filenameSrc = argsPassThrough[argsPassThrough.size() - 1];
  std::string filenameTgt = argsPassThrough[argsPassThrough.size() - 2];


  std::string const inpFileHash = util::computeFileHash(filenameSrc);
  
  std::string const filenameCache = config.getPygCacheDir() + "\\" + inpFileHash;


  if (std::filesystem::exists(filenameCache))
  {
    // Just copy cached output
    util::writeVectorToFile(filenameTgt, util::readVectorFromFile(filenameCache));
  }
  else
  {
    // Execute Pygmentize
    std::vector<std::string> args = config.getBaseArgs();
    args.insert(std::end(args), std::begin(argsPassThrough), std::end(argsPassThrough));
    util::launchProc(args);

    // If the command ran successful, there should be an output file now that
    // we can save to the cache
    if (std::filesystem::exists(filenameTgt))
    {
      util::writeVectorToFile(filenameCache, util::readVectorFromFile(filenameTgt));
    }
  }

}
