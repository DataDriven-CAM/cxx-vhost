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
#include <ranges>

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> 

namespace sylvanmats{

    template <bool ssl>
    struct vhost{
        std::string hostname{"*.localhost"};
        std::function<void(sylvanmats::http::Request*, sylvanmats::http::Response*)> handle{[](sylvanmats::http::Request *req, sylvanmats::http::Response *res){}};
        
        
        std::string ASTERISK_REGEXP = R"(\*)";
        std::string ASTERISK_REPLACE = R"(([^.]+))";
        std::string END_ANCHORED_REGEXP = R"((?:^|[^\\])(?:\\\\)*\$$)";
        std::string ESCAPE_REGEXP = R"(([.+?^=!:${}()|[\]/\\]))";
        std::string ESCAPE_REPLACE = R"(\\$1)";
        
        std::regex regexp;
        bool firstHandling=true;
        
        void operator()(sylvanmats::http::Request *req, sylvanmats::http::Response *res) {
            if(firstHandling){
                regexp=hostregexp(std::string_view(hostname));
                firstHandling=false;
            }
            if(vhostof(req, regexp)){
                handle(req, res);
                req->yield(false);
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
        
        bool vhostof(sylvanmats::http::Request *req, std::regex& regexp){
            bool ret=false;
            std::string&& hostname = hostnameof(req);
            std::smatch sm;
            if(std::regex_match(hostname, sm, regexp))ret=true;
            return ret;
        }
        
        std::string hostnameof(sylvanmats::http::Request *req){
            for(unsigned int headerIndex=0;headerIndex<req->httpMessage->header_field().size();headerIndex++){
//                std::cout<<req->httpMessage->header_field(headerIndex)->field_name()->getText()<<"| "<<req->httpMessage->header_field(headerIndex)->field_value(0)->getText()<<std::endl;
                if(req->httpMessage->header_field(headerIndex)->field_name()->getText().compare("Host")==0){
                    std::string value=req->httpMessage->header_field(headerIndex)->field_value(0)->getText();
                    std::string hostname(value.begin(), value.end());
                    size_t index=hostname.find(':');
                    if(index!=std::string::npos){
                        hostname=hostname.substr(0, index);
                    }
                    req->clientHost.assign(hostname);
                    return hostname;
                }
            }
            return "";
        }
    };

}


