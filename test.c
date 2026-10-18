// Самотест ShadowHook: хукаем свою функцию, проверяем proxy, снимаем хук.
// Запуск на устройстве: ./sh_test (libshadowhook.so лежит рядом)
#include <stdio.h>
#include "shadowhook.h"

__attribute__((noinline, visibility("default")))
int target(int x) {
  volatile int a = x, b = 0;
  for (int i = 0; i < 3; i++) b += a * (i + 1);
  return b + 1;
}

static int (*orig)(int);

static int proxy(int x) {
  return orig(x) + 1000;
}

int main(void) {
  int rc = shadowhook_init(SHADOWHOOK_MODE_UNIQUE, true);
  printf("ShadowHook %s\n", shadowhook_get_version());
  if (rc != 0) {
    printf("FAIL init: %d (%s)\n", rc, shadowhook_to_errmsg(rc));
    return 1;
  }

  int before = target(2);

  void *stub = shadowhook_hook_func_addr((void *)target, (void *)proxy, (void **)&orig);
  if (stub == NULL) {
    int e = shadowhook_get_errno();
    printf("FAIL hook: %d (%s)\n", e, shadowhook_to_errmsg(e));
    return 1;
  }

  int hooked = target(2);
  int ok_hook = (hooked == before + 1000);
  printf("before=%d hooked=%d -> %s\n", before, hooked, ok_hook ? "ok" : "BAD");

  int urc = shadowhook_unhook(stub);
  int after = target(2);
  int ok_unhook = (urc == 0 && after == before);
  printf("unhook=%d after=%d -> %s\n", urc, after, ok_unhook ? "ok" : "BAD");

  puts(ok_hook && ok_unhook ? "PASS" : "FAIL");
  return (ok_hook && ok_unhook) ? 0 : 1;
}
