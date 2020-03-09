/**
 * @file <argos3/plugins/simulator/visualizations/webviz/webviz_fileserver.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include "webviz_fileserver.h"

namespace argos {
  namespace Webviz {

    bool CFileServer::handle_file_request(
      uWS::HttpResponse<false> &res, uWS::HttpRequest &req) {
      return this->handle_file_request_<false>(res, req);
    }

    bool CFileServer::handle_file_request(
      uWS::HttpResponse<true> &res, uWS::HttpRequest &req) {
      return this->handle_file_request_<true>(res, req);
    }
    /****************************************/
    /****************************************/

    template <bool SSL>
    bool CFileServer::handle_file_request_(
      uWS::HttpResponse<SSL> &res, uWS::HttpRequest &req) {
      for (const auto &kv : base_dirs_) {
        const auto &mount_point = kv.first;
        const auto &base_dir = kv.second;

        std::cout << "mount_point:" << mount_point << std::endl;
        std::cout << "base_dir:" << base_dir << std::endl;

        // // Prefix match
        // if (!req.path.find(mount_point)) {
        //   std::string sub_path = "/" + req.path.substr(mount_point.size());
        //   if (detail::is_valid_path(sub_path)) {
        //     auto path = base_dir + sub_path;
        //     if (path.back() == '/') {
        //       path += "index.html";
        //     }

        //     if (detail::is_file(path)) {
        //       detail::read_file(path, res.body);
        //       auto type = detail::find_content_type(
        //         path, file_extension_and_mimetype_map_);
        //       if (type) {
        //         res.set_header("Content-Type", type);
        //       }
        //       res.status = 200;
        //       if (!head && file_request_handler_) {
        //         file_request_handler_(req, res);
        //       }
        //       return true;
        //     }
        //   }
        // }
      }
      res.end("Hi");

      return false;
    }
  }  // namespace Webviz
}  // namespace argos