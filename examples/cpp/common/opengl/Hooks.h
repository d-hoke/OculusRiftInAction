#pragma once

namespace oria {
  typedef std::vector<std::function<void()>> VecLambda;
  
  inline VecLambda & getShutdownHooks() {
    static VecLambda hooks;
    return hooks;
  }

  template <typename F>
  void addShudownHook(F f) {
    getShutdownHooks().push_back(f);
  }

  inline void runShutdownHooks() {
    VecLambda & hooks = getShutdownHooks();
    std::for_each(hooks.begin(), hooks.end(), [&](std::function<void()> f){
      f();
    });
  }
}
