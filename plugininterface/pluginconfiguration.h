#ifndef PLUGINCONFIGURATION_H
#define PLUGINCONFIGURATION_H

# include <string>
# include <unordered_map>
# include <stdexcept>

class PluginConfiguration
{
public:
    PluginConfiguration();
    PluginConfiguration(const PluginConfiguration & plugin);
    PluginConfiguration(const std::string & name, std::string pluginType, const std::unordered_map<std::string,std::string> & params);
    virtual ~PluginConfiguration();

    //getters & setters
    void setPluginType(const std::string & pluginType);
    void setParams(const std::unordered_map<std::string,std::string> & params);
    void setName(const std::string & name);

    std::string getPluginType();
    std::string getName();
    const std::unordered_map<std::string,std::string> & getParams();

protected:
    std::string pluginType;
    std::string name;
    std::unordered_map<std::string,std::string> params;
};

#endif // PLUGINCONFIGURATION_H
