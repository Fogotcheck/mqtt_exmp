# **[Проект](https://github.com/Fogotcheck/)**

## *Клонирование репозитория*

Клонировать репозиторий вы можете следующим образом:

```console
git clone "https://github.com/Fogotcheck/stm32_cmake_build.git"
```

## *Структура проекта*

```
├── AppMain                                 // main.cpp
|
├── Cmake
|       ├── Toolchain                       // минимальный набор правил для сборки
|       ├── Opts                            // дополнительные функции для сборки
|       └── Utils                           // дополнительные утилиты настройки окружения
|
├── Chip                                    // платформозависимые библиотеки
|       └── STM32F439ZITx
|             ├── Core                      // ядро для чипа
|             ├── Drivers                   // HAL + CMSIS
|             └── STM32F439ZITx.ioc         // файл автогенерации CubeMX
|
├── Lib                                     // библиотеки для проектов
|       ├── ...
|       ├── FreeRTOS                        // как пример для сборки сторонних библиотек
|       └── CMakeLists.txt                  // CMakeLists для сборки библиотек
|
|
├── CMakeLists.txt                          // основной файл для сборки проекта
|
└── README.md                               // Вы находитесь тут

```

## *Сборка проекта*

### Настройка сборки проекта
Сборка проекта основана на [Cmake](https://cmake.org/). Минимальные требования для настройки сборки проекта - являются указание toolchain:

```console
CMAKE_TOOLCHAIN_FILE=./Cmake/Toolchain/Arm-none-eabi-toolchain.cmake
```
Таким образом настройка проекта для сборки arm-none-eabi будет выглядеть:

```console
cmake -DCMAKE_TOOLCHAIN_FILE=./Cmake/Toolchain/Arm-none-eabi-toolchain.cmake -B ./build
```

### Дополнительные утилиты настройки проекта

Утилита настройки [pre-commit](https://pre-commit.com)

```console
cmake --build ./build --target UTILS_BuildTest_VIRTUAL_ENV
```

Утилита настройки редактора [vscode](https://code.visualstudio.com/)
```console
cmake --build ./build --target UTILS_BuildTest_VSC_CONFIGURE
```
### Сборка проекта

Сборка осуществляется командой:

```console
cmake --build ./build
```

## *Дополнительные ссылки*

* [Трэкер задач](https://github.com/Fogotcheck/)
