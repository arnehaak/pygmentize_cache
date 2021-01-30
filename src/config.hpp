#pragma once

#include "envvar.hpp"
#include <json.hpp>

#include <string>
#include <vector>
#include <fstream>

#include <iostream>

#include <stdexcept>
#include <filesystem>


namespace pygcache
{


class Config
{
  public:
    Config(std::string const & configfile)
      : m_configfile(configfile)
    {
      if (!std::filesystem::exists(configfile))
        throw std::runtime_error("Config file does not exist!");

      std::ifstream ifs(m_configfile);
      ifs >> m_jsonObj;
    }

    std::vector<std::string> getBaseArgs() const
    {
      std::vector<std::string> args = m_jsonObj.at("base_args");

      for (auto & arg : args)
        arg = substituteEnvVars(arg);
 
      return args;
    }
    
    std::string getPygCacheDir() const
    {      
      return substituteEnvVars(m_jsonObj.at("pyg_cache_dir"));
    }


  protected:

    static std::vector<std::string> findVars(std::string const& str)
    {
      // Tokenize
      std::vector<std::string> wordVector;

      std::size_t prev = 0, pos;
      while ((pos = str.find_first_of(" \"';/\\", prev)) != std::string::npos)
      {
        if (pos > prev)
          wordVector.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
      }
      if (prev < str.length())
        wordVector.push_back(str.substr(prev, std::string::npos));

      // Identify variables
      std::vector<std::string> vars;
      for (std::string const& word : wordVector)
        if (word[0] == '$')
          vars.push_back(word.substr(1, word.size() - 1));

      return vars;
    }


    static void replaceAll(std::string& str, const std::string& from, const std::string& to)
    {
      if (from.empty())
        return;
      size_t start_pos = 0;
      while ((start_pos = str.find(from, start_pos)) != std::string::npos)
      {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
      }
    }


    static std::string substituteEnvVars(std::string str)
    {
      for (std::string const& var : findVars(str))
      {
        std::string const envVarValue = util::getEnvVar(var);

        if (envVarValue.empty())
          throw std::runtime_error("Environment variable not set!");          

        replaceAll(str, std::string("$") + var, envVarValue);
      }

      return str;
    }

    std::string const m_configfile;
    nlohmann::json    m_jsonObj;
  
};


} // namespace pygcache
