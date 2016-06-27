/*************************************************************************
   @ File Name: hotTagging.cc
   @ Method: 
   @ Author: Jerry Shi
   @ Mail: jerryshi0110@gmail.com
   @ Created Time: 2016年2月17日 星期三 15时48分09秒
 ************************************************************************/
#include "mongoose.h"
#include "ml/rating.hpp"
#include "glog/logging.h"

// glog configuration
/*void InitGlog(const char* cProgam, const char* logDir){

    char cInfoPath[100];
    char cErrPath[100];
    char cWarnPath[100];
    char cFatalPath[100];

    snprintf(cInfoPath, sizeof(cInfoPath), "%s%s", logDir, "/INFO_");
    snprintf(cErrPath, sizeof(cErrPath), "%s%s", logDir, "/ERROR_");
    snprintf(cWarnPath, sizeof(cWarnPath), "%s%s", logDir, "/WARNING_");
    snprintf(cFatalPath, sizeof(cFatalPath), "%s%s", logDir, "/FATAL_");

    google::InitGoogleLogging(cProgam);

    FLAGS_logbufsecs = 0; // no cache
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_alsologtostderr = false; // close to stderr
    FLAGS_max_log_size = 2048; // max size 2G

    google::SetLogDestination(google::GLOG_INFO,cInfoPath);
    google::SetLogDestination(google::GLOG_ERROR,cErrPath);
    google::SetLogDestination(google::GLOG_WARNING,cWarnPath);
    google::SetLogDestination(google::GLOG_FATAL,cFatalPath);
}
*/
// ---------
// Auto pointer point to ml::Rating
boost::shared_ptr<ml::Rating> pRating;
// ---------

// http server
static int ev_handler(struct mg_connection *conn, enum mg_event ev){
    std::string data("");
    char buffer[65535];
    time_t time1, time2;
    switch(ev){
        case MG_AUTH: return MG_TRUE;
        case MG_REQUEST:
        {
            bool succ = false;
            if(mg_get_var(conn, "str", buffer, sizeof(buffer)) > 0){
                std::string str(buffer);
                data = str;
                if(data.empty())
                    succ = false;
                else
                    succ = true;
            }
            /*
            try{
                std::string content(conn->content, conn->content_len);
                postDat = content;
                if(postDat.empty())
                    succ = false;
            }catch(std::exception& ex){
                std::cerr << ex.what() << std::endl;
                succ = false;
            }*/
            double score(0.0);
            double label(0.0);
            //std::cout << "TEST " << data << std::endl;
            if(succ)
                label = pRating->PredictClassLabel(data, score);
            std::stringstream ss;
            std::string tmp("");
            double s(0.0);
            if(label == 1)
                tmp = "positive";
            else if(label == -1)
                tmp = "negative";
            else 
                tmp = "comments refused";
            if(score > 0.5)
                s = score;
            else
                s = 1 - score;
            ss << "{\"emotion\":\"" << tmp << "\",\"confidence\":" << s << "}";
            std::string res(ss.str());
            ss.str("");
            mg_send_data(conn,res.c_str(),res.size());
            return MG_TRUE;
        }
        
        default: return MG_FALSE;
    }
}

// 
static void *serve(void *server){
    for(;;) mg_poll_server((struct mg_server *)server, 1000);
    return NULL;
}

void start_http_server(int pool_size = 1){

    std::vector<struct mg_server*> servers;
    for(int p = 0; p < pool_size; ++p){
        std::string name = boost::lexical_cast<std::string>(p);
        struct mg_server *server;
        server = mg_create_server((void*)name.c_str(), ev_handler);
        if(p ==0){
            mg_set_option(server,"listening_port","38813");
        } else {
            mg_copy_listeners(servers[0], server);
        }
        servers.push_back(server);
    }

    for(uint32_t p = 0; p < servers.size(); ++p){
        struct mg_server* server = servers[p];
        mg_start_thread(serve,server);
    }
    sleep(3600*24*365*10);
}

int main(int argc, char* argv[])
{
    //InitGlog("HotDealTagging", "../resource/log");
    int thread_num = 22;
    std::string featureDir("../model");
    std::string dictDir("../../../resource/jieba");
    std::string model("../model/comment_scale.model");
    pRating.reset(new ml::Rating(featureDir, dictDir, model));

    std::cout << "Start program...\n";
    while(1){
        start_http_server(thread_num);
    }

    return EXIT_SUCCESS;
}
