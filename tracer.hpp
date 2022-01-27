#ifndef QUICK_TRACER_HPP_
#define QUICK_TRACER_HPP_

#include <sstream>
#include <functional>

namespace quick {

// Trace packet received by the trace interceptor.
struct TracePacket {
  // Will be true for trace packet associated with start of a scope, mostly
  // function start.
  bool scope_begin = true;
  const char* function_name = nullptr;
  const char* file_name = nullptr;
  int line_number = 0;
  // Optionally supplied arguments to SCOPE_TRACE are coverted to string.
  const std::string arg_str;
  // Depth of scope. Mostly the size of function-call-stack.
  int depth = 0;
};

void DefaultInterceptor(const TracePacket& packet);

// If g_enable_scope_trace is false, runtime overhead will be just the cost of
// if-check at each SCOPE_TRACE macro.
// Default: disabled.
extern bool g_enable_scope_trace;

void EnableScopeTrace() { g_enable_scope_trace = true; }
void DisableScopeTrace() { g_enable_scope_trace = false; }

class ScopedTraceObject;

// Usage:
// Option1:
//    quick::ScopedTracer::GetInstance()::SetDefaultInterceptor();
// Option2:
//    quick::ScopedTracer::GetInstance()::SetInterceptor(custom_interceptor);
class ScopedTracer {
 public:
  static ScopedTracer& GetInstance();
  using InterceptorType = std::function<void(const TracePacket& packet)>;
  inline void SetInterceptor(InterceptorType interceptor) {
    interceptor_ = interceptor;
    g_enable_scope_trace = true;
  }
  inline void SetDefaultInterceptor() { SetInterceptor(DefaultInterceptor); }
  friend ScopedTraceObject;

 private:
  ScopedTracer() = default;  // private ctr.
  inline void BeginScope(const char* function_name, const char* file_name,
                  int line_number, std::string arg_str) {
    TracePacket packet {true, function_name, file_name,
                        line_number, arg_str, depth_};
    interceptor_(packet);
    ++depth_;
  }
  inline void EndScope() {
    TracePacket packet {false};
    --depth_;
    packet.depth = depth_;
    interceptor_(packet);
  }

 private:
  int depth_ = 0;
  InterceptorType interceptor_ = DefaultInterceptor;
};


class ArgsToStr {
 public:
  template<typename... Ts>
  static std::string Get(const Ts&... args) {
    std::ostringstream oss;
    ArgsToStr instance(oss);
    instance.AddToStream(args...);
    return oss.str();
  }
 private:
  ArgsToStr(std::ostream& os): os(os) { }  // private ctr.
  void AddToStream() { }
  template<typename T>
  void AddToStream(const T& arg) { os << arg; }
  template<typename T, typename... Ts>
  void AddToStream(const T& arg, const Ts&... args) { os << arg << " "; AddToStream(args...); }
  std::ostream& os;
};


class ScopedTraceObject {
 public:
  template<typename... Ts>
  ScopedTraceObject(const char* function_name,
                    const char* file_name, int line_number, const Ts&... ts) {
    if (g_enable_scope_trace) {
      ScopedTracer::GetInstance().BeginScope(
          function_name, file_name, line_number, ArgsToStr::Get(ts...));
    }
  }
  ~ScopedTraceObject() {
    if (g_enable_scope_trace) {
      ScopedTracer::GetInstance().EndScope();
    }
  }
};

}  // namespace quick

#define QUICK_TOKEN_PASTE(x, y) x ## y
#define QUICK_TOKEN_PASTE2(x, y) QUICK_TOKEN_PASTE(x, y)
#define QUICK_SCOPE_UNIQUE_NAME() QUICK_TOKEN_PASTE2(xoeb_7r9_, __LINE__)

// If compile time macro DISABLE_QUICK_SCOPE_TRACE is ON, there will be zero
// runtime overhead of SCOPE_TRACE.
#ifdef DISABLE_QUICK_SCOPE_TRACE
#define QUICK_SCOPE_TRACE(...) 
#else
#define QUICK_SCOPE_TRACE(...) quick::ScopedTraceObject QUICK_SCOPE_UNIQUE_NAME() \
  (__func__, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif  // DISABLE_QUICK_SCOPE_TRACE

#define SCOPE_TRACE(...) QUICK_SCOPE_TRACE(__VA_ARGS__)

#endif //  QUICK_TRACER_HPP_
