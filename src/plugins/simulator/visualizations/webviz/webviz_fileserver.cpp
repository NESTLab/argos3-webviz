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

    CFileServer::CFileServer() {
      /* Building MIME types */
      m_map_MIMETYPES.emplace("txt", "text/plain");
      m_map_MIMETYPES.emplace("htm", "text/html");
      m_map_MIMETYPES.emplace("html", "text/html");
      m_map_MIMETYPES.emplace("css", "text/css");
      m_map_MIMETYPES.emplace("jpeg", "image/jpg");
      m_map_MIMETYPES.emplace("jpg", "image/jpg");
      m_map_MIMETYPES.emplace("png", "image/png");
      m_map_MIMETYPES.emplace("gif", "image/gif");
      m_map_MIMETYPES.emplace("svg", "image/svg+xml");
      m_map_MIMETYPES.emplace("ico", "image/x-icon");
      m_map_MIMETYPES.emplace("mp4", "video/mp4");
      m_map_MIMETYPES.emplace("json", "application/json");
      m_map_MIMETYPES.emplace("pdf", "application/pdf");
      m_map_MIMETYPES.emplace("js", "application/javascript");
      m_map_MIMETYPES.emplace("xml", "application/xml");
    }

    /****************************************/
    /****************************************/

    bool CFileServer::HandleFileRequest(
      uWS::HttpResponse<false> &res, uWS::HttpRequest &req) {
      return this->HandleFileRequest_<false>(&res, &req);
    }

    /****************************************/
    /****************************************/

    bool CFileServer::HandleFileRequest(
      uWS::HttpResponse<true> &res, uWS::HttpRequest &req) {
      return this->HandleFileRequest_<true>(&res, &req);
    }

    /****************************************/
    /****************************************/

    template <bool SSL>
    bool CFileServer::HandleFileRequest_(
      uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
      /* File path to stream */
      std::string strFilePath = "";

      /* flag to stop sending data on abort */
      bool bIsSending = true;

      for (const auto &kv : base_dirs_) {
        const auto &strMountPoint = kv.first;
        const auto &strBaseDir = kv.second;

        // Prefix match
        if (!req->getUrl().find(strMountPoint)) {
          std::string strSubPath =
            "/" + std::string(req->getUrl().substr(strMountPoint.size()));

          if (IsValidPath(strSubPath)) {
            auto strPath = strBaseDir + strSubPath;
            if (strPath.back() == '/') {
              strPath += "index.html";
            }

            if (IsFile(strPath)) {
              /* Start streaming file to the client */
              strFilePath = std::string(strPath);
              break;  // File found
            }
          }
        }
      }

      /* Register handlers */
      /* lambda is mutable to update "bIsSending" counter */
      res->onAborted([bIsSending]() mutable {
        // std::cout << "! aborted" << std::endl;
        bIsSending = false;
      });

      // std::cout << "File ::" << strFilePath << std::endl;

      if (!strFilePath.empty()) {
        /* Default mime type */
        std::string strFileMIMEType = "application/octet-stream";

        /* File extension */
        std::string strFileExt = "";

        /* Get File extension */
        std::smatch m;
        static auto re = std::regex("\\.([a-zA-Z0-9]+)$");
        if (std::regex_search(strFilePath, m, re)) {
          strFileExt = m[1].str();
        }

        /* Search file extension in MIME type table */
        if (!strFileExt.empty()) {
          if (m_map_MIMETYPES.find(strFileExt) != m_map_MIMETYPES.end()) {
            strFileMIMEType = m_map_MIMETYPES.find(strFileExt)->second;
          }
        }

        // std::cout << "MIME type:" << strFileMIMEType << std::endl;

        /* Callback for when the "res" is ready to receive data  */
        res->onData([res, strFilePath, strFileMIMEType, bIsSending](
                      std::string_view ck_, bool isEnd) {
          if (isEnd) {
            /*
             * *NIX way to get file size without seeking to the end and back
             */
            struct stat cFileStatus;
            stat(strFilePath.c_str(), &cFileStatus);
            size_t unFileSize = cFileStatus.st_size;

            /* HUGE LIMITATION */
            /* TODO : Solve max size issue by properly solving corking and
             * chunked data */

            /* For now, limit max file size to 50 MB (random number)*/

            if (unFileSize > 50 * 1024 * 1024) {
              res->writeStatus("413 Payload Too Large");
              res->tryEnd("Payload Too Large");
              return;
            }

            /* Few variables */
            size_t chunk_size = 1024 * 1024;  // 1 MB chunk

            size_t total_chunks = unFileSize / chunk_size;
            size_t last_chunk_size = unFileSize % chunk_size;

            /* if the above division was uneven */
            if (last_chunk_size != 0) {
              ++total_chunks; /* add an unfilled final chunk */
            } else {          /* if division was even, last chunk is full */
              last_chunk_size = chunk_size;
            }

            /* Get the file input stream */
            std::ifstream fFileToStream(strFilePath, std::ifstream::binary);

            /* Write some headers */
            res->writeStatus("200 OK");
            res->writeHeader("Content-Type", strFileMIMEType);

            /* the loop of chunking */
            for (size_t chunk = 0; chunk < total_chunks && bIsSending;
                 ++chunk) {
              bool bIsLast = chunk == total_chunks - 1;

              size_t this_chunk_size =
                chunk == total_chunks - 1 /* if last chunk */
                  ? last_chunk_size /* then fill chunk with remaining bytes */
                  : chunk_size;     /* else fill entire chunk */

              /* if needed, we also have the position of this chunk in the
               * file size_t start_of_chunk = chunk * chunk_size; */

              /* adapt this portion as necessary, this is the fast C++ way */
              std::vector<char> chunk_data(this_chunk_size);
              fFileToStream.read(
                &chunk_data[0],   /* address of buffer start */
                this_chunk_size); /* this many bytes is to be read */

              /* do something with chunk_data before next iteration */
              // std::cout << "chunk #" << chunk << std::endl;

              std::string strToSend(chunk_data.begin(), chunk_data.end());

              res->cork([res, strToSend, bIsLast, unFileSize]() {
                if (bIsLast) {
                  res->tryEnd(strToSend, unFileSize);
                } else {
                  res->write(strToSend);
                }
              });
            }
          }
        });
        return true;
      }

      return false;
    }

    /****************************************/
    /****************************************/

    bool CFileServer::AddMountPoint(
      std::string str_mount_point, std::string str_dir) {
      if (IsDir(str_dir)) {
        std::string mnt = !str_mount_point.empty() ? str_mount_point : "/";
        if (!mnt.empty() && mnt[0] == '/') {
          base_dirs_.emplace_back(mnt, str_dir);
          return true;
        }
      }
      return false;
    }

    /****************************************/
    /****************************************/

    bool CFileServer::IsFile(const std::string &path) {
      struct stat st;
      return stat(path.c_str(), &st) >= 0 && S_ISREG(st.st_mode);
    }

    /****************************************/
    /****************************************/

    bool CFileServer::IsDir(const std::string &path) {
      struct stat st;
      return stat(path.c_str(), &st) >= 0 && S_ISDIR(st.st_mode);
    }

    /****************************************/
    /****************************************/

    bool CFileServer::IsValidPath(const std::string &path) {
      size_t level = 0;
      size_t i = 0;

      // Skip slash
      while (i < path.size() && path[i] == '/') {
        i++;
      }

      while (i < path.size()) {
        // Read component
        auto beg = i;
        while (i < path.size() && path[i] != '/') {
          i++;
        }

        auto len = i - beg;
        assert(len > 0);

        if (!path.compare(beg, len, ".")) {
          ;
        } else if (!path.compare(beg, len, "..")) {
          if (level == 0) {
            return false;
          }
          level--;
        } else {
          level++;
        }

        // Skip slash
        while (i < path.size() && path[i] == '/') {
          i++;
        }
      }
      return true;
    }
  }  // namespace Webviz
}  // namespace argos