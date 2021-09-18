# cxx-vhost
vhost for sylvan pod server

Inspired by  [the `vhost@3.0.2` module](https://github.com/expressjs/vhost).

# API

```c++
#include "vhost.h"

            pmc::vhost<true> vhost{.hostname = "*."+host,
                                   .handle = [](sylvanmats::http::Request *req, sylvanmats::http::Response *res){
                                       std::cout<<"your vhost handling here "<<std::endl;
                                   }};

```

as middleware.  Currently sylvanhttp is unpublished

```c++
...
     pmc::mimetypes mimetypes("./cpp_modules/mime-db/db.json");
    sylvanmats::Server server(host, 8080);
    server([&mimetypes, &host](sylvanmats::ClientLiaison& liaison){
            pmc::ldp<true> ldp;
            sylvanmats::vhost<true> vhost{.hostname = "*."+host,
                                   .handle = [&mimetypes, &ldp](sylvanmats::http::Request *req, sylvanmats::http::Response *res){
                                        std::cout<<"vhost handle "<<std::endl;
                                        ldp(req, res);
                                   }};
        liaison.use(vhost);
    });
```
