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

#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <vector>

#include "App.h"  // uWebSockets

namespace argos {
  namespace Webviz {
    class CFileServer {
     private:
      /**
       * @brief MIME map for files
       *
       */
      std::map<std::string, std::string> m_map_MIMETYPES;

      /**
       * @brief to hold all the folders to host from
       *
       */
      std::vector<std::pair<std::string, std::string>> base_dirs_;

      /**
       * @brief is the path a valid string path type
       *
       * @param path
       * @return true
       * @return false
       */
      bool IsValidPath(const std::string &path);

      /**
       * @brief
       *
       * @param path Check if the path is a valid file on system
       * @return true
       * @return false
       */
      bool IsFile(const std::string &path);

      /**
       * @brief Check if the path is a valid folder on system
       *
       * @param path
       * @return true
       * @return false
       */
      bool IsDir(const std::string &path);

     public:
      /**
       * @brief Construct a new CFileServer object
       *
       */
      CFileServer();

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
      bool HandleFileRequest_(uWS::HttpResponse<SSL> *, uWS::HttpRequest *);

      /* Temp fillers to access template from outside */
      bool HandleFileRequest(uWS::HttpResponse<false> &, uWS::HttpRequest &);
      bool HandleFileRequest(uWS::HttpResponse<true> &, uWS::HttpRequest &);

      /**
       * @brief Add a new folder/file as mount point
       *
       * @param url URI path for the mount
       * @param path folder path to the mount
       *
       * @return true if the folder/file was added
       * @return false if the folder/file was invalid
       */
      bool AddMountPoint(std::string, std::string);
    };
  }  // namespace Webviz
}  // namespace argos

#endif