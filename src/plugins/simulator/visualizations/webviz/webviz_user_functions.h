/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/webviz_user_functions.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_USER_FUNCTIONS_H
#define ARGOS_WEBVIZ_USER_FUNCTIONS_H

namespace argos {
  class CWebvizUserFunctions;
}

#include <argos3/core/utility/configuration/base_configurable_resource.h>
#include <argos3/core/utility/plugins/factory.h>

#include <nlohmann/json.hpp>

namespace argos {
  class CWebvizUserFunctions : public CBaseConfigurableResource {
   public:
    /**
     * Class constructor.
     */
    CWebvizUserFunctions();

    /**
     * Class destructor.
     */
    virtual ~CWebvizUserFunctions();

    virtual void Init(TConfigurationNode &t_tree) {}
    virtual void Reset() {}
    virtual void Destroy() {}

    /**
     * Send data hook to add extra content to JSON message sent to clients.
     * Return a JSON object which will be attached to JSON body as "extra_data"
     */
    virtual nlohmann::json sendExtraData() { return nlohmann::json(); }
  };
}  // namespace argos

/* Definitions useful for dynamic linking of user functions */
#define REGISTER_WEBVIZ_USER_FUNCTIONS(CLASSNAME, LABEL) \
  REGISTER_SYMBOL(                                       \
    CWebvizUserFunctions,                                \
    CLASSNAME,                                           \
    LABEL,                                               \
    "undefined",                                         \
    "undefined",                                         \
    "undefined",                                         \
    "undefined",                                         \
    "undefined")

#endif
