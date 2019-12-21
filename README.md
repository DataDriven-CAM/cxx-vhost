# cxx-vhost
vhost for [uWebSockets](https://github.com/uNetworking/uWebSockets)

Inspired by  [the `vhost@3.0.2` module](https://github.com/expressjs/vhost).

# API

```c++
#include "vhost.h"

            pmc::vhost<true> vhost{.hostname = "*."+host,
                                   .handle = [](uWS::HttpResponse<true> *res, uWS::HttpRequest *req){
                                       std::cout<<"your vhost handling here "<<std::endl;
                                   }};

```

as middleware.  Currently experimenting with [branching](https://github.com/rimmartin/uWebSockets) and adding a use method to the uWS::TemplatedApp struct. Then

```c++
...
                uWS::SSLApp sslApp=uWS::SSLApp(ssl_options);
                sslApp.filter([](auto *res, int huh){
                    //std::cout<<"huh "<<huh<<std::endl;
                });
                sslApp.use(cors);
                sslApp.use(vhost);

                sslApp.listen(host, port, [port, directory1](auto *token) {
                    if (token) {
                        std::cout << "Thread " << std::this_thread::get_id() << " listening on port " << port << " serving " << directory1 << "..."<< std::endl;
                    } else {
                        std::cout << "Thread " << std::this_thread::get_id() << " failed to listen on port " << port << std::endl;
                    }
                }).run();

```
