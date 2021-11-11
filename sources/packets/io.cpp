#include <dhyara/packets/io.h>
#include <dhyara/packets/all.h>
#include <iomanip>

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::beacon& beacon){
    stream << "BCN at " << beacon.time();
    return stream;
}

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::acknowledgement& acknowledgement){
    stream << "ACK at " << acknowledgement.time();
    return stream;
}

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::advertisement& advertisement){
    stream << "ADV for " << advertisement.dest().to_string() << " delay " << advertisement.delay() << " hops " << (int)advertisement.hops() << " name " << advertisement.name();
    return stream;
}

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::chunk& chunk){
    stream << "ADV for " << chunk.target().to_string() << " from " << chunk.source().to_string() << " id " << (int)chunk.packet() << " pending " << (int)chunk.pending();
    return stream;
}

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::delivered& delivered){
    stream << "DLV for " << delivered.target().to_string() << " from " << delivered.source().to_string() << " id " << (int)delivered.packet();
    return stream;
}

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::echo_request& echo_request){
    stream << "ICMPQ for " << echo_request.target().to_string() << " from " << echo_request.source().to_string() << " at " << echo_request.time() << " seq " << echo_request.seq() << " ttl " << (int)echo_request.ttl();
    return stream;
}

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::echo_reply& echo_reply){
    stream << "ICMPR for " << echo_reply.target().to_string() << " from " << echo_reply.source().to_string() << " hit " << echo_reply.rtime() << " origin " << echo_reply.time() << " latency "<< std::setprecision(2) << std::fixed << ((double)echo_reply.latency()/1000.0) << "ms"  << " seq " << echo_reply.seq() << " ttl " << (int)echo_reply.ttl();
    return stream;
}

std::ostream& dhyara::packets::operator<<(std::ostream& stream, const dhyara::packets::echo_lost& echo_lost){
    stream << "ICMPL for " << echo_lost.target().to_string() << " from " << echo_lost.source().to_string() << " at " << echo_lost.time() << " seq " << echo_lost.seq();
    return stream;
}