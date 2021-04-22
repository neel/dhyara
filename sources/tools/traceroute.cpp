#include "dhyara/tools/traceroute.h"

dhyara::tools::traceroute::traceroute(dhyara::network& network, const dhyara::address& target): _network(network), _target(target), _conn_lost(0), _conn_reply(0), _sequence(1){
    using namespace std::placeholders;
    _conn_lost  = _network.echo_lost().connect(std::bind(&dhyara::tools::traceroute::lost, this, _1, _2));
    _conn_reply = _network.echo_reply().connect(std::bind(&dhyara::tools::traceroute::reply, this, _1, _2));
}

dhyara::tools::traceroute::~traceroute(){
    _network.echo_lost().disconnect(_conn_lost);
    _network.echo_reply().disconnect(_conn_reply);
    reset();
}

void dhyara::tools::traceroute::reset(){
    _conn_lost = 0;
    _conn_reply = 0;
    _sequence = 1;
}


void dhyara::tools::traceroute::operator()(){
    ESP_LOGI(
        "traceroute", 
        "to %s", 
        _target.to_string().c_str()
    );
    run(_target, _sequence);
}

void dhyara::tools::traceroute::run(const dhyara::address& target, std::uint8_t ttl){
    dhyara::packets::echo_request request(target, _network.link().address(), ttl, 0, ttl); // keeping sequence same as the ttl for easier understanding
    _network.link().send(target, dhyara::packets::type::echo_request, request);
}

void dhyara::tools::traceroute::lost(const dhyara::address&, const dhyara::packets::echo_lost& lost){
    if(lost.target() == _network.link().address()){
        dhyara::delay_type now = esp_timer_get_time();
        dhyara::delay_type rtt = now - lost.time();
        
        ESP_LOGI(
            "traceroute", 
            "* %zu %s %.2lfms", 
            _sequence,
            lost.source().to_string().c_str(),
            (double)rtt/1000.0
        );
        
        if(lost.source() != _target){
            _sequence++;
            run(_target, _sequence);
        }
    }
}


void dhyara::tools::traceroute::reply(const dhyara::address&, const dhyara::packets::echo_reply& reply){
    if(reply.target() == _network.link().address()){
        dhyara::delay_type now = esp_timer_get_time();
        dhyara::delay_type rtt = now - reply.time();
        
        ESP_LOGI(
            "traceroute", 
            "# %zu %s %.2lfms", 
            _sequence,
            reply.source().to_string().c_str(),
            (double)rtt/1000.0
        );
    }
}
