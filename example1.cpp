#include "tracer.hpp"

#include <iostream>
#include <string>

std::string Q(const std::string& s) {
  SCOPE_TRACE("s = ", s);
  return "ABC_";
}

std::string P(const std::string& s) {
  auto ss = Q(s);
  ss += s;
  return ss;
}

int K(const std::string& s, int k) {
  SCOPE_TRACE();
  if (k == 3) {
    return P(s + "__" + std::to_string(k)).size();
  } else {
    return P(s).size();
  }
}

int G(int i, int p) {
  SCOPE_TRACE("i = ", i, ", p = ", p);
  if (i == 3) {
    SCOPE_TRACE("inside if ; i = ", i);
    int zero = i*i - (i+1)*(i-1) - 1;  // (x+1)*(x-1) == x*x - 1
    return 1/zero;
  } else {
    K(std::to_string(i), i + 20);
    SCOPE_TRACE("outside if ; i = ", i);
  }
  return i*2;
}


int F(int n) {
  SCOPE_TRACE();
  int p = 0;
  for (int i = 0; i < n; i++) {
    p += G(i, i+10);
  }
  return n + p;
}


int main(int argc, char** argv) {
  quick::ScopedTracer::GetInstance().SetDefaultInterceptor();
  F(argc + 5);
}
