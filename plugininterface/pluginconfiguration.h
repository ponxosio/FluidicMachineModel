#ifndef PLUGINCONFIGURATION_H
#define PLUGINCONFIGURATION_H

# include <string>
# include <unordered_map>
# include <stdexcept>

#include "fluidicmachinemodel_global.h"

class PLUGINCONFIGURATION_EXPORT PluginConfiguration
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

    const std::string & getPluginType() const;
    const std::string & getName() const;
    const std::unordered_map<std::string,std::string> & getParams() const;

protected:
    std::string pluginType;
    std::string name;
    std::unordered_map<std::string,std::string> params;
};

#endif // PLUGINCONFIGURATION_H
