# Sound Processor

Консольное приложение для чтения, обработки, генерации и записи звуковых
файлов формата WAV.

Программа поддерживает последовательное применение нескольких фильтров.
Каждый следующий фильтр получает результат работы предыдущего фильтра.

## Поддерживаемый WAV-формат

- несжатый PCM;
- один канал (mono);
- частота дискретизации 44100 Гц;
- 16 бит на отсчёт;
- знаковые целые отсчёты.

Файлы другого формата отклоняются с сообщением об ошибке.

## Сборка

Требования:

- CMake 3.16 или новее;
- компилятор с поддержкой C++17.

Обычная сборка без тестов:

```bash
cmake -S . -B build -DBUILD_TESTS=OFF
cmake --build build
```

Исполняемый файл:

```bash
./build/sound_processor
```

## Использование

Общий формат команды:

```bash
./build/sound_processor \
    [-i input.wav] \
    [-o output.wav] \
    [-f filter parameters...]...
```

Входной и выходной файлы необязательны. Без входного файла звук можно создать
с помощью генератора. Несколько фильтров применяются в порядке их записи.

Пример обработки WAV-файла:

```bash
./build/sound_processor \
    -i input.wav \
    -o output.wav \
    -f ampl 0.8 \
    -f timestretch 2
```

Пример генерации двухсекундной синусоиды частотой 440 Гц:

```bash
./build/sound_processor \
    -o output.wav \
    -f generator sin 440 2000
```

## Фильтры

### Amplification

Изменяет громкость звука:

```bash
-f ampl <factor>
```

`factor` должен быть неотрицательным. Значение `1` не меняет громкость,
`0.5` уменьшает её в два раза, `2` увеличивает в два раза.

### Normalize

Нормализует максимальную амплитуду:

```bash
-f normalize [peak]
```

`peak` принимает значения от `0` до `1`. По умолчанию используется `1`.

### Silence

Добавляет тишину в указанный интервал:

```bash
-f silence <sec|ms> <start> <end>
```

Единица измерения задаётся через `sec` или `ms`.

### Time Stretch

Изменяет длительность звука:

```bash
-f timestretch <factor>
```

`factor` должен быть больше нуля. Например, `2` увеличивает длительность
примерно в два раза.

### Low Pass

Сглаживает резкие изменения соседних отсчётов:

```bash
-f lowpass <window_size>
```

`window_size` должен быть положительным нечётным числом.

## Генераторы

### Sine

```bash
-f generator sin <frequency_hz> <duration_ms>
```

### AM

```bash
-f generator am \
    <amplitude> <carrier_hz> <modulation_hz> <depth> <duration_ms>
```

`amplitude` и `depth` принимают значения от `0` до `1`.

### FM

```bash
-f generator fm \
    <amplitude> <carrier_hz> <modulation_hz> <deviation_hz> <duration_ms>
```

`amplitude` принимает значения от `0` до `1`.

## Тесты

Тесты используют Catch2 и включаются флагом `BUILD_TESTS`:

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Если Catch2 не установлен, CMake загружает Catch2 v3.4.0 через FetchContent.

## Структура проекта

```text
app/                 точка входа программы
include/parser/      объявления парсера командной строки
include/pipeline/    объявления pipeline и конвертера
include/wav/         объявления компонентов WAV
include/filters/     объявления фильтров и генераторов
src/                 реализации компонентов
tests/               тесты Catch2
```

`Pipeline` хранит фильтры как `std::unique_ptr<IFilter>`. Благодаря виртуальному
методу `IFilter::apply()` разные фильтры применяются через общий интерфейс.
