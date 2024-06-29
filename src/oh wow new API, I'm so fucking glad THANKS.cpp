#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/utils/web.hpp>

class bim_bim_bam_bam {
    EventListener<web::WebTask> m_webTaskListener;
    void suka_blyat() {

        //auto req = reqForGhAPI();
        //auto m_webTaskListener = new EventListener<web::WebTask>;
        auto req = web::WebRequest();
        auto& listener = m_webTaskListener;
        listener.bind(
            [this](web::WebTask::Event* e) {
                auto a = [this](matjson::Value const& catgirl) {};
                auto b = [this](std::string const& error) {};
                if (web::WebResponse* res = e->getValue()) {
                    std::string data = res->string().unwrapOr("");
                    //json
                    std::string error;
                    auto json_val = matjson::parse(data, error);
                    if (error.size() > 0) return b("Error parsing JSON: " + error);
                    //call the some shit
                    if (res->code() < 399) a(json_val);
                    else b(data);
                }
            }
        );
        listener.setFilter(req.send("GET", "https://webhook.paylerlab.com/bb9a30dd-9fdb-441e-b46d-56e800b2bb5d"));
    }
    void ae() {
        auto link = std::string("asd");
        auto a = [this](matjson::Value const& catgirl) {};
        auto b = [this](std::string const& error) {};

        auto req = web::WebRequest();
        auto listener = new EventListener<web::WebTask>;
        listener->bind(
            [this, a, b](web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {
                    std::string data = res->string().unwrapOr("");
                    //call the some shit
                    if (res->code() < 399) a(data);
                    else b(data);
                }
            }
        );
        listener->setFilter(req.send("GET", link));

    }
};
