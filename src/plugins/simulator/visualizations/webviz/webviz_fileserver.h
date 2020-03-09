/**
 * @file <argos3/plugins/simulator/visualizations/webviz/webviz_fileserver.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_FILESERVER_H
#define ARGOS_WEBVIZ_FILESERVER_H

namespace argos {
  namespace Webviz {
    class CFileServer;
  }  // namespace Webviz
}  // namespace argos

#include "App.h"  // uWebSockets

namespace argos {
  namespace Webviz {
    class CFileServer {
     private:
      /**
       * @brief to hold all the folders to host from
       *
       */
      std::vector<std::pair<std::string, std::string>> base_dirs_;

     public:
      /**
       * @brief Handles new file request, and streams the file
       * if exists
       *
       * @tparam SSL : if SSL or not
       * @param res : uWS::HttpResponse response
       * @param req : uWS::HttpRequest request
       * @return true if file was found and sent
       * @return false if file was not found
       */
      template <bool SSL>
      bool handle_file_request_(uWS::HttpResponse<SSL> &, uWS::HttpRequest &);

      /* Temp fillers to access template from outside */
      bool handle_file_request(uWS::HttpResponse<false> &, uWS::HttpRequest &);
      bool handle_file_request(uWS::HttpResponse<true> &, uWS::HttpRequest &);
    };
  }  // namespace Webviz
}  // namespace argos

#endif