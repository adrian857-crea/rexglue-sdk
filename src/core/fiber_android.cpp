
#include <rex/platform.h>

#if REX_PLATFORM_ANDROID && REX_ARCH_ARM64

#include <rex/thread/fiber.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <new>

extern "C" void rex_fiber_swap_context(void* from, void* to);

namespace rex::thread {

thread_local Fiber* Fiber::tls_current_ = nullptr;

Fiber* Fiber::ConvertCurrentThread() {
  auto* fiber = new (std::nothrow) Fiber();
  if (!fiber) {
    return nullptr;
  }

  fiber->is_thread_fiber_ = true;
  tls_current_ = fiber;
  return fiber;
}

Fiber* Fiber::Create(
    size_t stack_size,
    void (*entry)(void*),
    void* arg) {
  if (!entry || stack_size < 16384) {
    return nullptr;
  }

  auto* fiber = new (std::nothrow) Fiber();
  if (!fiber) {
    return nullptr;
  }

  try {
    fiber->stack_.resize(stack_size);
  } catch (...) {
    delete fiber;
    return nullptr;
  }

  fiber->entry_ = entry;
  fiber->arg_ = arg;

  const uintptr_t stack_top =
      reinterpret_cast<uintptr_t>(fiber->stack_.data() +
                                  fiber->stack_.size());

  fiber->context_.sp = stack_top & ~uintptr_t{0xF};
  fiber->context_.x19_x30[11] =
      reinterpret_cast<uintptr_t>(&Fiber::Trampoline);

  return fiber;
}

void Fiber::Trampoline() {
  Fiber* fiber = tls_current_;
  assert(fiber != nullptr);
  assert(fiber->entry_ != nullptr);

  fiber->entry_(fiber->arg_);

  // A fiber must not return into an arbitrary saved address.
  std::abort();
}

void Fiber::SwitchTo(Fiber* target) {
  Fiber* from = tls_current_;

  assert(from != nullptr);
  assert(target != nullptr);

  if (!from || !target || from == target) {
    return;
  }

  tls_current_ = target;
  rex_fiber_swap_context(&from->context_, &target->context_);
}

void Fiber::Destroy() {
  if (is_thread_fiber_) {
    assert(this == tls_current_);
    if (this == tls_current_) {
      tls_current_ = nullptr;
    }
  } else {
    assert(this != tls_current_);
  }

  delete this;
}

}  // namespace rex::thread

#endif
