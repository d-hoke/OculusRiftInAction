#include "Lights.h"
#include "MatrixStack.h"

class Stacks {
public:
  static MatrixStack & projection() {
    static MatrixStack projection;
    return projection;
  }

  static MatrixStack & modelview() {
    static MatrixStack modelview;
    return modelview;
  }

  template <typename Function>
  static void withPush(MatrixStack & stack, Function f) {
    stack.withPush(f);
  }

  template <typename Function>
  static void withPush(MatrixStack & stack1, MatrixStack & stack2, Function f) {
    stack1.withPush([&]{
      stack2.withPush(f);
    });
  }

  template <typename Function>
  static void withPush(Function f) {
    with_push(projection(), modelview(), f);
  }

  static Lights & lights() {
    static Lights lights;
    return lights;
  }
};
