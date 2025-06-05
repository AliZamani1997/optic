#include <Pages.h>
#include "pages/_content.h"

static const char * TAG = "Server.Pages";
bool sendContent(WebServer * server ,String name){

    for (size_t i = 0; i < sizeof(__web_contents)/sizeof(_content_t *); i++)
    {
        _content_t * p = __web_contents[i];
        if(name==p->name){
            server->sendHeader("Content-Encoding", "gzip");
            server->send_P(200, p->type, (const char *) p->address, p->size);
            return true;
        }
    }
    LOG_ERROR("Not found page source : %s",name.c_str());
    return false;
}
// bool sendStyle(WebServer * server){

//     for (size_t i = 0; i < sizeof(__web_contents)/sizeof(_content_t *); i++)
//     {
//         _content_t * p = __web_contents[i];
//         if(name==p->name){
//             server->sendHeader("Content-Encoding", "gzip");
//             server->send_P(200, "text/html", (const char *) p->ad  , p->size);
//             return true;
//         }
//     }
//     LOG_ERROR("Not found page source : %s",name.c_str());
//     return false;
// }