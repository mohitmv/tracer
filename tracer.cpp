#include "tracer.hpp"

#include <iostream>

namespace quick {

bool g_enable_scope_trace = false;

void DefaultInterceptor(const TracePacket& packet) {
  std::cout << std::string(packet.depth, ' ');
  if (packet.scope_begin) {
    std::cout << "{ [" << packet.function_name << " @ " << packet.file_name
              << ":" << packet.line_number << "]";
    if (packet.arg_str.size() > 0) {
      std::cout << " (" << packet.arg_str << ")";
    }
  } else {
    std::cout << '}';
  }
  std::cout << std::endl;
}

ScopedTracer& ScopedTracer::GetInstance() {
  static ScopedTracer tracer;
  return tracer;
}



}  // namespace quick
