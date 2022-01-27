# A light weight tracer to integrate in complex projects

The goal of this tracer is to make debugging of complex projects easy without
much runtime overhead.

## How to use:

Put `SCOPE_TRACE()` macro at the begining of critical functions in a complex
project. **See example1.cpp for sample usage.** These tracing macro should be
placed in complex codebase at all time (i.e. not only for a temporarily period
while debugging). These tracers don't have much overhead is tracing is disabled.
See next section 'Overhead'.

Optionally, we can put variable number of args in `SCOPE_TRACE(...)`, if we
need to export additional information.


## Run time Overhead:

If tracing is not enabled at runtime (possibily by passing runtime command line
args), the runtime overhead will be just the cost of an if-check at each
SCOPE_TRACE macro. Which is pretty negligible.
However if we need to avoid that cost too, there is a compile time macro
`-D DISABLE_QUICK_SCOPE_TRACE`, to avoid that cost too. Note that if this macro
is passed at compile time, tracing cannot be enabled at runtime using command
line args in executable.


## Build and run example1.cpp :

`g++ example1.cpp tracer.cpp -o /tmp/example && /tmp/example`

## Customer interceptor :

`quick::ScopedTracer` accepts a custom trace interceptor. The default
interceptor (as implemented in `tracer.cpp`) simply prints the trace info in
stdout. However a custom defined interceptor can choose to dump the trace in a
file or selectively take specific actions example printing the trace only for a
specific file and ignoring other files.

## APIs

1.

```C++
quick::EnableScopeTrace();
```

2.

```C++
quick::DisableScopeTrace();
```

3. This will enable the scope trace implicitly.

```C++
quick::ScopedTracer::GetInstance().SetDefaultInterceptor();
```

## APIs for custom interceptor

1. Trace packet received by the trace interceptor.

```C++
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
```
2. Custom interceptor:

`quick::ScopedTracer::InterceptorType` is an alias of
`std::function<void(const TracePacket& packet)>`, which is accepted by
`SetInterceptor` API. User should define a lambda which does something when
receiving a trace packet. The default interceptor simply prints the content of
TracePacket.

3. To set the custom interceptor:

```C++
quick::ScopedTracer::GetInstance().SetInterceptor(custom_lambda);
```

## Facing any issue ?

Report in this repo or write to mohitsaini1196@gmail.com
