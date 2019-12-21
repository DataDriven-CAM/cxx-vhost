#include <stdio.h>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <string>
#include <string_view>
#include <regex>
#include <functional>

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> 

namespace pmc{

    template <bool SSL>
    struct vhost{
        std::string hostname{"*.localhost"};
        std::function<void(uWS::HttpResponse<SSL>*, uWS::HttpRequest*)> handle{[](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req){}};
        
        
        std::string ASTERISK_REGEXP = R"(\*)";
        std::string ASTERISK_REPLACE = R"(([^.]+))";
        std::string END_ANCHORED_REGEXP = R"((?:^|[^\\])(?:\\\\)*\$$)";
        std::string ESCAPE_REGEXP = R"(([.+?^=!:${}()|[\]/\\]))";
        std::string ESCAPE_REPLACE = R"(\\$1)";
        
        std::regex regexp;
        bool firstHandling=true;
        
        void operator()(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            if(firstHandling){regexp=hostregexp(std::string_view(hostname));
                firstHandling=false;}
            if(vhostof(req, regexp)){
                handle(res, req);
                req->setYield(false);
            }
        };
        
        std::regex hostregexp(std::string_view val){
            //isregexp(val);
            std::string source(val.begin(), val.end());
            std::regex r(ESCAPE_REGEXP, std::regex_constants::ECMAScript | std::regex_constants::icase);
            std::smatch sm;
            std::string source2;
            source2=std::regex_replace(source, r, "\\$&");
            source="";
            std::regex r2(ASTERISK_REGEXP, std::regex_constants::ECMAScript | std::regex_constants::icase);
            while(std::regex_search(source2, sm, r2)){
                source+=sm.prefix();
                source+=ASTERISK_REPLACE;
                source2=sm.suffix();
            }
            source+=source2;
            if (source[0] != '^') {
                source = '^' + source;
            }

            // force trailing anchor matching
            std::regex r3(END_ANCHORED_REGEXP, std::regex_constants::ECMAScript | std::regex_constants::icase);
            if (!std::regex_match(source, sm, regexp)) {
                source += '$';
            }            
            return std::regex(source, std::regex_constants::ECMAScript | std::regex_constants::icase);
        };
        
        bool vhostof(uWS::HttpRequest *req, std::regex& regexp){
            bool ret=false;
            std::string hostname = hostnameof(req);
            std::smatch sm;
            if(std::regex_match(hostname, sm, regexp))ret=true;
            return ret;
        }
        
        std::string hostnameof(uWS::HttpRequest *req){
            for(uWS::HttpRequest::HeaderIterator it=req->begin();it!=req->end();++it){
                if((*it).first.compare("host")==0){
                    size_t index=(*it).second.find(':');
                    std::string hostname((*it).second.begin(), (*it).second.end());
                    if(index!=std::string::npos){
                        hostname=hostname.substr(0, index);
                    }
                    return hostname;
                }
            }
            return "";
        }
    };

}


