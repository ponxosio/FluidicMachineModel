#include "pluginconfiguration.h"

PluginConfiguration::PluginConfiguration()  {
    this->pluginType = "unknow";
    this->name = "unknow";
}

PluginConfiguration::PluginConfiguration(const PluginConfiguration & plugin) {
    this->pluginType = plugin.pluginType;
    this->name = plugin.name;

    for (auto it: plugin.params) {
        this->params.insert(std::make_pair(it.first, it.second));
    }
}

PluginConfiguration::PluginConfiguration(const std::string & name, std::string pluginType, const std::unordered_map<std::string,std::string> & params) {
    this->pluginType = pluginType;
    this->name = name;

    for (auto it: params) {
        this->params.insert(std::make_pair(it.first, it.second));
    }
}

PluginConfiguration::~PluginConfiguration() {

}

//getters & setter
void PluginConfiguration::setPluginType(const std::string & pluginType) {
    this->pluginType = std::string(pluginType);
}

void PluginConfiguration::setParams(const std::unordered_map<std::string,std::string> & params) {
    this->params.clear();
    for (auto it: params) {
        this->params.insert(std::make_pair(it.first, it.second));
    }
}

std::string PluginConfiguration::getPluginType() {
    return pluginType;
}

const std::unordered_map<std::string,std::string> & PluginConfiguration::getParams() {
    return params;
}

std::string PluginConfiguration::getName() {
    return this->name;
}

void PluginConfiguration::setName(const std::string & name) {
    this->name = name;
}

