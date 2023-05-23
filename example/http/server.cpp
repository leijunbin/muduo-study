#include <fcntl.h>
#include <unistd.h>

#include "../../src/base/include/Logging.h"
#include "../../src/net/include/EventLoop.h"
#include "../../src/net/include/InetAddress.h"
#include "../../src/net/include/http/HttpRequest.h"
#include "../../src/net/include/http/HttpResponse.h"
#include "../../src/net/include/http/HttpServer.h"

using namespace TinyWeb::net::http;
using namespace TinyWeb::base;
using namespace TinyWeb::net;

bool benchmark = false;

int main() {
  Logger::setLogLevel(Logger::INFO);
  std::string name = "HTTP server";
  InetAddress listenAddr = InetAddress(8086);
  EventLoop loop;
  HttpServer server(&loop, listenAddr, name);

  // server_test
  server.StaticFile("/favicon.ico", "images/favicon.ico");

  server.Get("/test", [](const HttpRequest& req, HttpResponse* resp) {
    LOG_INFO << "call get /";
    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setContentType("text/html");
    std::string now = Timestamp::now().toString();
    resp->setStringBody(R"(<html><head><title>TinyWebLib HTTP</title></head>
                        <body><h1>Hello</h1>Now is )" +
                        now + "<h1>line</h1> </body></html>");
  });

  server.DownloadFile("/log", "log.txt");
  server.DownloadFile("/filed", "test.jpg");

  server.Post("/post", [](const HttpRequest& req, HttpResponse* resp) {
    LOG_INFO << "call post";
    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setContentType("text/html");
    resp->setStringBody("post body: " + req.body() +
                        ", body size: " + std::to_string(req.body().size()));
  });

  server.Post("/login", [](const HttpRequest& req, HttpResponse* resp) {
    resp->setStatusCode(HttpResponse::k302Found);
    if (req.post().find("username")->second == "123456" &&
        req.post().find("password")->second == "123456") {
      resp->addHeader("Location", "/welcome");
    } else {
      resp->addHeader("Location", "/error");
    }
  });

  server.Post("/register", [](const HttpRequest& req, HttpResponse* resp) {
    resp->setStatusCode(HttpResponse::k302Found);
    resp->addHeader("Location", "/welcome");
  });

  server.StaticFile("/file", "test.jpg");

  // html web index
  server.StaticFile("/", "index.html");
  server.StaticFile("/images/favicon.ico", "images/favicon.ico");
  server.StaticFile("/js/custom.js", "js/custom.js");
  server.StaticFile("/css/style.css", "css/style.css");
  server.StaticFile("/css/bootstrap.min.css", "css/bootstrap.min.css");
  server.StaticFile("/css/animate.css", "css/animate.css");
  server.StaticFile("/css/magnific-popup.css", "css/magnific-popup.css");
  server.StaticFile("/css/font-awesome.min.css", "css/font-awesome.min.css");
  server.StaticFile("/css/style.css", "css/style.css");
  server.StaticFile("/images/profile-image.jpg", "images/profile-image.jpg");
  server.StaticFile("/js/jquery.js", "js/jquery.js");
  server.StaticFile("/js/bootstrap.min.js", "js/bootstrap.min.js");
  server.StaticFile("/js/smoothscroll.js", "js/smoothscroll.js");
  server.StaticFile("/js/jquery.magnific-popup.min.js",
                    "js/jquery.magnific-popup.min.js");
  server.StaticFile("/js/magnific-popup-options.js",
                    "js/magnific-popup-options.js");
  server.StaticFile("/js/wow.min.js", "js/wow.min.js");

  // html web picture
  server.StaticFile("/picture", "picture.html");
  server.StaticFile("/images/instagram-image1.jpg",
                    "images/instagram-image1.jpg");
  server.StaticFile("/images/instagram-image2.jpg",
                    "images/instagram-image2.jpg");
  server.StaticFile("/images/instagram-image3.jpg",
                    "images/instagram-image3.jpg");
  server.StaticFile("/images/instagram-image4.jpg",
                    "images/instagram-image4.jpg");
  server.StaticFile("/images/instagram-image5.jpg",
                    "images/instagram-image5.jpg");

  // html web register
  server.StaticFile("/register", "register.html");

  // html web video
  server.StaticFile("/video", "video.html");
  server.StaticFile("/video/xxx.mp4", "video/xxx.mp4");

  // html web login
  server.StaticFile("/login", "login.html");
  server.StaticFile("/welcome", "welcome.html");
  server.StaticFile("/error", "error.html");

  server.setThreadNum(4);
  server.start();
  loop.loop();
}