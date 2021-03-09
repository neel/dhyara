#include "dhyara/tools/traceroute.h"

dhyara::tools::traceroute::traceroute(dhyara::network& network, const dhyara::peer_address& target): _network(network), _target(target), _connection(0), _sequence(1){
    using namespace std::placeholders;
    _connection = _network.echo_lost().connect(std::bind(&dhyara::tools::traceroute::lost, this, _1, _2));
}

dhyara::tools::traceroute::~traceroute(){
    _network.echo_lost().disconnect(_connection);
    _connection = 0;
}


void dhyara::tools::traceroute::operator()(){
    run(_target, _sequence);
}

void dhyara::tools::traceroute::run(const dhyara::peer_address& target, std::uint8_t ttl){
    dhyara::packets::echo_request request(target, _network.link().address(), ttl, 0, ttl); // keeping sequence same as the ttl for easier understanding
    _network.link().send(target, dhyara::packets::type::echo_request, request);
}

void dhyara::tools::traceroute::lost(const dhyara::peer_address&, const dhyara::packets::echo_lost& lost){
    if(lost.target() == _network.link().address()){
        dhyara::delay_type now = esp_timer_get_time();
        dhyara::delay_type rtt = now - lost.time();
        
        ESP_LOGI(
            "traceroute", 
            "%zu %s %.2lfms", 
            _sequence,
            lost.source().to_string().c_str(),
            (double)rtt/1000.0
        );
        
        if(lost.source() != _target){
            _sequence++;
            operator()();
        }
    }
}
