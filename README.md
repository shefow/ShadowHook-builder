# ShadowHook builder

GitHub Actions, который собирает [ShadowHook](https://github.com/bytedance/android-inline-hook)
(`libshadowhook.so` + AAR) нужной версии. URL архива ищется автоматически по тегу.

## Файлы

| Файл | Назначение |
|---|---|
| `.github/workflows/build.yml` | сборка (`workflow_dispatch`) |
| `test.c` | самотест: хукает свою функцию, проверяет и снимает хук |
| `README.md` | это описание |

## Запуск

**Actions → Build ShadowHook → Run workflow**

- `version` — `2.0.1`, `v2.0.1` или `latest` (по умолчанию)
- `java` — JDK для Gradle (17 по умолчанию; для старых 1.x может понадобиться 11)
- `release` — выложить результат в GitHub Release (`shadowhook-<версия>`)

Что делает: определяет тег → скачивает `.../archive/refs/tags/v<версия>.zip` →
`./gradlew :shadowhook:assembleRelease` → достаёт `libshadowhook.so` из AAR →
собирает `test.c` через NDK для `arm64-v8a` и `armeabi-v7a`.

## Результат (artifact `shadowhook-<версия>`)

```
shadowhook-release.aar
shadowhook.h
arm64-v8a/    libshadowhook.so  sh_test
armeabi-v7a/  libshadowhook.so  sh_test
```

## Проверка на устройстве

Папку нужно положить туда, где разрешено исполнение (не `/sdcard`):

```sh
# adb
adb push arm64-v8a /data/local/tmp/sh && adb shell 'cd /data/local/tmp/sh && chmod +x sh_test && ./sh_test'
# или Termux
cp -r arm64-v8a ~/sh && cd ~/sh && chmod +x sh_test && ./sh_test
```

Ожидаемый вывод заканчивается строкой `PASS`.
