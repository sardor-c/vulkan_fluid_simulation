# 01-dars. Muhit, kompilyatsiya va CMake

## Maqsad

Dars oxirida:
- C++ kodi qanday qilib `.exe` faylga aylanishini tushunasiz
- Birinchi dasturni avval qo'lda (`g++` bilan), keyin **CMake** bilan build qilasiz
- Ko'p faylli loyiha tuzasiz (header + source) va birinchi `Vec2` strukturangizni yozasiz
- Compiler xatosi va linker xatosini bir-biridan ajrata olasiz

---

## Nazariya

### 1. Kompyuter C++ ni tushunmaydi

Protsessor faqat **mashina kodini** tushunadi: `48 89 5C 24 08 ...` kabi raqamlar ketma-ketligini.
C++ esa inson o'qishi uchun yozilgan matn. Ularni bog'laydigan dastur — **kompilyator**
(bizda `g++`). Uni tarjimon deb tasavvur qiling.

Tarjima 3 bosqichda bo'ladi:

```
 main.cpp ──► [1. Preprocessor] ──► [2. Compiler] ──► main.o ──┐
                                                               ├──► [3. Linker] ──► app.exe
 vec2.cpp ──► [1. Preprocessor] ──► [2. Compiler] ──► vec2.o ──┤
                                                               │
                          tayyor kutubxonalar (.a, .dll.a) ────┘
```

1. **Preprocessor** — `#` bilan boshlanadigan qatorlarni bajaradi. Masalan, `#include "vec2.hpp"`
   so'zma-so'z *"vec2.hpp faylining matnini shu yerga ko'chirib qo'y"* degani.
2. **Compiler** — har bir `.cpp` faylni **alohida** mashina kodiga tarjima qiladi va
   **object file** (`.o`) hosil qiladi. Bu fayl hali to'liq emas: unda "teshiklar" bor. Masalan,
   `main.cpp` `length()` funksiyasini chaqiradi, lekin bu funksiyaning kodi `vec2.o` ichida.
   `main.o` da faqat *"bu yerga length degan funksiyaning manzili qo'yilsin"* degan belgi turadi.
3. **Linker** — barcha `.o` fayllarni va kutubxonalarni bitta `.exe` ga yig'adi hamda
   "teshiklar"ni haqiqiy manzillar bilan to'ldiradi.

**O'xshatish:** kitobning boblarini turli tarjimonlar alohida tarjima qiladi (compiler).
Keyin muharrir ularni bitta kitobga tikadi va *"5-bobga qarang"* degan havolalarga
sahifa raqamlarini qo'yib chiqadi (linker).

**Bu nega muhim?** Chunki xatolar ham ikki xil bo'ladi:

| Xato turi | Qachon chiqadi | Odatiy sabab |
|---|---|---|
| **Compiler xatosi** | Bitta `.cpp` faylni tarjima qilayotganda | Sintaksis (`;` tushib qolgan), noto'g'ri tip, e'lon qilinmagan nom |
| **Linker xatosi** (`undefined reference to ...`) | Hammasini yig'ayotganda | Funksiya e'lon qilingan, lekin uning kodi hech qaysi `.o` yoki kutubxonada topilmadi |

### 2. Header (`.hpp`) va source (`.cpp`)

Funksiyaning ikki qismi bor:

- **E'lon (declaration)** — *"shunday funksiya mavjud, u shunday parametrlar oladi"*:
  ```cpp
  float length(Vec2 v);
  ```
- **Ta'rif (definition)** — funksiyaning tanasi, ya'ni haqiqiy kodi:
  ```cpp
  float length(Vec2 v)
  {
      return std::sqrt(v.x * v.x + v.y * v.y);
  }
  ```

Odatda e'lonlar **header** faylga (`vec2.hpp`), ta'riflar esa **source** faylga (`vec2.cpp`)
yoziladi. Boshqa fayllar header'ni `#include` qiladi, shunda compiler funksiya borligini va uni
qanday chaqirishni biladi. Haqiqiy kodni esa linker keyinroq `vec2.o` dan topib beradi.

`#pragma once` — header boshida yoziladi va bitta header bir faylga ikki marta
qo'shilib qolishining oldini oladi.

### 3. Kutubxonalar (libraries)

Kutubxona — boshqalar yozib, kompilyatsiya qilib qo'ygan tayyor kod. Biz ishlatadiganlari:

- **GLFW** — oyna ochish, klaviatura va sichqoncha bilan ishlash (02-dars)
- **Vulkan loader** — Vulkan funksiyalarini GPU drayveriga yetkazib beradi (04-dars)

Kutubxonalar ikki xil bo'ladi:

| Tur | Fayl | Qanday ishlaydi |
|---|---|---|
| **Static** | `.a` (Windows/MSVC'da `.lib`) | Kodi to'g'ridan-to'g'ri `.exe` ichiga ko'chiriladi |
| **Dynamic** | `.dll` (Linux'da `.so`) | `.exe` ishga tushganda alohida fayldan yuklanadi. Linkerga esa kichik "import library" (`.dll.a`) beriladi |

Masalan, `vulkan-1.dll` GPU drayveri bilan birga Windows'ga o'rnatilgan bo'ladi.

### 4. MSYS2 va MinGW

**MSYS2** — Windows uchun Linux'dagidek paket menejeri (`pacman`) va asboblar to'plami.
**MinGW-w64** — uning ichidagi Windows uchun GCC kompilyatori. Sizda hammasi `C:\msys64\mingw64\bin`
papkasida turibdi: `g++`, `cmake`, `ninja`, `glslc`. Bu papka `PATH` ga qo'shilgan, shuning uchun
ularni oddiy PowerShell'dan ham chaqira olasiz.

### 5. Build tizimi: CMake va Ninja

Fayllar ko'payganda `g++` buyruqlarini qo'lda yozish qiyinlashadi. Bundan tashqari, faqat
**o'zgargan** fayllarni qayta kompilyatsiya qilish kerak, aks holda katta loyiha har safar
daqiqalab build bo'ladi. Buni build tizimi hal qiladi:

```
CMakeLists.txt ──(1. configure: cmake)──► build/build.ninja ──(2. build: ninja)──► g++ ──► app.exe
```

- **CMake** — loyiha tavsifini (`CMakeLists.txt`) o'qiydi va build fayllarini *yaratadi*.
  O'zi kompilyatsiya qilmaydi. U "me'mor": reja chizadi.
- **Ninja** — CMake yaratgan reja bo'yicha `g++` ni kerakli tartibda chaqiradi va faqat
  o'zgargan fayllarni qayta build qiladi. U "quruvchi".

Ikki bosqich bor:
1. **Configure** (`cmake -S . -B build ...`) — bir marta (yoki sozlamalarni o'zgartirganda).
2. **Build** (`cmake --build build`) — har safar kodni o'zgartirganda. Agar `CMakeLists.txt`
   o'zgargan bo'lsa, bu buyruq configure'ni ham o'zi qayta bajaradi.

Barcha build natijalari alohida `build/` papkasiga tushadi ("out-of-source build").
Manba kodingiz toza qoladi va `build/` ni istalgan payt o'chirib, qayta yaratsa bo'ladi.

### 6. C++ standarti

C++ vaqt o'tishi bilan yangilanib boradi: C++11, 14, 17, 20, 23... Biz **C++17** dan
foydalanamiz: zamonaviy, hamma kompilyatorlarda to'liq ishlaydi.

---

## Amaliyot

### 1-qadam: asboblarni tekshirish

PowerShell'da (yoki VS Code terminalida):

```powershell
g++ --version
cmake --version
ninja --version
glslc --version
```

Har biri versiya raqamini chiqarishi kerak. Sizning kompyuteringizda bularning hammasi
o'rnatilgan (`fluid_sim` loyihasida tekshirganmiz). Boshqa kompyuterda biror narsa
yetishmasa, **MSYS2 MINGW64** terminalini ochib, quyidagini bajarish kifoya:

```bash
pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja \
  mingw-w64-x86_64-glfw mingw-w64-x86_64-vulkan-headers mingw-w64-x86_64-vulkan-loader \
  mingw-w64-x86_64-shaderc mingw-w64-x86_64-vulkan-validation-layers
```

**Muharrir:** istalganidan foydalanishingiz mumkin. VS Code bo'lsa, *C/C++* (yoki *clangd*) va
*CMake Tools* kengaytmalari qulay: xatolarni kod ichida ko'rsatadi.

### 2-qadam: papka tuzilishi

`fluid_sim_tutorial` ichida `src` papkasini yarating:

```powershell
cd C:\dev\VULKAN_SIMULATIONS\fluid_sim_tutorial
New-Item -ItemType Directory src
```

```
fluid_sim_tutorial/
├── docs/       (darslar, allaqachon bor)
└── src/        (C++ kodingiz)
```

### 3-qadam: birinchi dastur

`src/main.cpp` faylini yarating:

```cpp
#include <iostream>

int main()
{
    std::cout << "Salom, suyuqlik simulatsiyasi!\n";
    return 0;
}
```

Qatorma-qator:

| Kod | Ma'nosi |
|---|---|
| `#include <iostream>` | Standart kutubxonaning konsolga yozish/o'qish qismini qo'shish. `< >` — tizim yoki kutubxona fayli, `" "` — o'zimizning fayl |
| `int main()` | Dastur shu funksiyadan boshlanadi. `int` — dastur tugaganda OS'ga qaytariladigan son |
| `std::` | **Namespace** (nomlar maydoni). Standart kutubxonadagi hamma narsa `std` ichida, o'zimizning nomlar bilan to'qnashmasligi uchun |
| `std::cout << "..."` | Konsolga matn chiqarish. `\n` — yangi qatorga o'tish |
| `return 0;` | 0 = "hammasi yaxshi tugadi". Boshqa son xatoni bildiradi |

### 4-qadam: qo'lda kompilyatsiya (jarayonni ko'z bilan ko'rish uchun)

`fluid_sim_tutorial` papkasida turib:

```powershell
g++ -c src/main.cpp -o main.o    # 1+2: preprocessor + compiler → object file
g++ main.o -o hello.exe          # 3: linker → exe
.\hello.exe
```

- `-c` — "faqat kompilyatsiya qil, link qilma". Natijada `main.o` paydo bo'ladi.
- Ikkinchi buyruq faqat linkerni ishga tushiradi.

`main.o` va `hello.exe` paydo bo'lganini ko'ring. Bularni bir buyruq bilan ham qilish mumkin
(`g++ src/main.cpp -o hello.exe`), ammo bosqichlarni bir marta alohida ko'rib qo'yish foydali.

Endi ikkala faylni o'chiring, ular boshqa kerak emas:

```powershell
Remove-Item main.o, hello.exe
```

### 5-qadam: CMake bilan

`fluid_sim_tutorial` ichida `CMakeLists.txt` faylini yarating:

```cmake
cmake_minimum_required(VERSION 3.20)
project(fluid_sim_tutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    src/main.cpp
)

target_compile_options(app PRIVATE -Wall -Wextra)
```

| Qator | Ma'nosi |
|---|---|
| `cmake_minimum_required` | Kamida qaysi CMake versiyasi kerak |
| `project(... LANGUAGES CXX)` | Loyiha nomi; tili C++ (`CXX`) |
| `set(CMAKE_CXX_STANDARD 17)` | C++17 ishlatilsin |
| `add_executable(app ...)` | `app` nomli dastur (**target**) shu fayllardan yasalsin |
| `target_compile_options(... -Wall -Wextra)` | Ko'proq ogohlantirish (warning) chiqarilsin. Ogohlantirishlar ko'pincha haqiqiy xatolarni ushlaydi, ularni e'tiborsiz qoldirmang |

Configure, build va ishga tushirish:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
.\build\app.exe
```

- `-S .` — manba kod shu papkada (`CMakeLists.txt` shu yerda)
- `-B build` — build natijalari `build/` papkasiga
- `-G Ninja` — Ninja ishlatilsin
- `-DCMAKE_BUILD_TYPE=Debug` — optimizatsiyasiz, debug ma'lumotlari bilan (xato topish oson).
  Keyinroq, simulatsiya tez ishlashi kerak bo'lganda, `Release` ga o'tamiz.

Endi `main.cpp` ni o'zgartirsangiz, faqat `cmake --build build` yetarli.

### 6-qadam: ko'p faylli loyiha — `Vec2`

Simulatsiyada har bir zarrachaning **pozitsiyasi** (qayerda turibdi) va **tezligi** (qayerga,
qanchalik tez ketyapti) bo'ladi. Ikkalasi ham 2D'da ikkita son: `x` va `y`. Ularni bitta nom
ostida guruhlash uchun **struct** ishlatamiz.

`src/vec2.hpp`:

```cpp
#pragma once

// 2D vektor: tekislikdagi nuqta yoki yo'nalish
// (masalan, zarrachaning pozitsiyasi yoki tezligi)
struct Vec2
{
    float x = 0;
    float y = 0;
};

Vec2 add(Vec2 a, Vec2 b);    // a + b
Vec2 scale(Vec2 v, float s); // v * s (vektorni songa ko'paytirish)
float length(Vec2 v);        // vektor uzunligi
```

`src/vec2.cpp`:

```cpp
#include "vec2.hpp"

#include <cmath>

Vec2 add(Vec2 a, Vec2 b)
{
    return {a.x + b.x, a.y + b.y};
}

Vec2 scale(Vec2 v, float s)
{
    return {v.x * s, v.y * s};
}

float length(Vec2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}
```

`src/main.cpp` ni almashtiring:

```cpp
#include "vec2.hpp"

#include <iostream>

int main()
{
    Vec2 position{1.0f, 2.0f};
    Vec2 velocity{3.0f, 4.0f};

    std::cout << "Tezlik uzunligi: " << length(velocity) << "\n";

    // Zarracha 0.5 soniyadan keyin qayerda bo'ladi?
    // yangi pozitsiya = pozitsiya + tezlik * vaqt
    Vec2 newPosition = add(position, scale(velocity, 0.5f));
    std::cout << "Yangi pozitsiya: (" << newPosition.x << ", " << newPosition.y << ")\n";
    return 0;
}
```

`CMakeLists.txt` da yangi faylni qo'shing:

```cmake
add_executable(app
    src/main.cpp
    src/vec2.cpp
)
```

Yangi tushunchalar:

- **`struct`** — bir nechta qiymatni bitta tipga birlashtirish. `Vec2 v;` deb yozsangiz,
  `v.x` va `v.y` ga ega bo'lasiz. `= 0` — boshlang'ich qiymat.
- **`float`** — kasr son (32 bit, taxminan 7 xona aniqlik). GPU'lar asosan `float` bilan
  ishlaydi, shuning uchun biz ham shuni ishlatamiz.
- **`1.0f`** — oxiridagi `f` "bu float" degani. `f` siz `1.0` **double** (64 bit) bo'ladi.
- **`{a.x + b.x, a.y + b.y}`** — `Vec2` ni jingalak qavslar bilan yaratish.
- **`length`** — Pifagor teoremasi: `√(x² + y²)`. `(3, 4)` vektorning uzunligi `5`
  (mashhur 3-4-5 uchburchak).
- `pozitsiya + tezlik × vaqt` — bu oddiy misol emas: bu **fizika simulatsiyasining yuragi**.
  11-darsda zarrachalarimiz aynan shu formula bilan harakatlanadi.

Build qilib, ishga tushiring:

```powershell
cmake --build build
.\build\app.exe
```

---

## Tekshiruv

Konsolda quyidagi chiqishi kerak:

```
Tezlik uzunligi: 5
Yangi pozitsiya: (2.5, 4)
```

---

## Mashqlar

1. **Linker xatosi.** `CMakeLists.txt` dan `src/vec2.cpp` qatorini vaqtincha o'chirib, build
   qiling. `undefined reference to 'length(Vec2)'` kabi xato chiqadi. Bu compiler xatosimi yoki
   linker xatosimi? Nega `main.cpp` muvaffaqiyatli kompilyatsiya bo'ldi? Keyin qatorni qaytaring.
2. **Compiler xatosi.** `main.cpp` da bitta `;` ni o'chiring va build qiling. Xato xabaridagi fayl
   nomi va qator raqamini toping. Bu qaysi bosqichning xatosi?
3. **Ogohlantirish.** `main` ichida `int unused = 5;` deb yozing va build qiling.
   `-Wall` qanday ogohlantirish berganini o'qing, keyin qatorni o'chiring.
4. **Yangi funksiyalar.** `vec2.hpp/.cpp` ga quyidagilarni qo'shing va `main` da sinab ko'ring:
   - `Vec2 subtract(Vec2 a, Vec2 b)` — `a - b`
   - `float dot(Vec2 a, Vec2 b)` — `a.x*b.x + a.y*b.y` (**skalyar ko'paytma**, *dot product*)

   `dot({1, 0}, {0, 1})` nechaga teng? `dot({1, 0}, {1, 0})` chi? Bu sonlar ikki vektor
   orasidagi burchak haqida nima deydi? (Javobni 11 va 13-darslarda ishlatamiz.)
5. **Build papkasi.** `build/` papkasini butunlay o'chiring va configure + build ni qaytadan
   bajaring. Hech narsa yo'qolmadi. Nega?

### Git (tavsiya etiladi)

Har bir darsdan keyin ishingizni saqlab borish uchun Git qulay: xohlagan paytda oldingi holatga
qaytish va o'zgarishlarni ko'rish mumkin.

```powershell
git init
"build/" | Out-File -Encoding utf8 .gitignore
git add .
git commit -m "01-dars: birinchi dastur va Vec2"
```

---

## O'zingizni tekshiring

1. Preprocessor, compiler va linker — har biri nima ish qiladi?
2. `.o` fayl nima va nega uni to'g'ridan-to'g'ri ishga tushirib bo'lmaydi?
3. E'lon (declaration) va ta'rif (definition) qanday farq qiladi? Qaysi biri header'da turadi?
4. `#include <...>` va `#include "..."` farqi nimada?
5. CMake va Ninja vazifalari qanday farqlanadi?
6. Qachon `cmake -S . -B build ...` kerak, qachon `cmake --build build` yetarli?

---

**Keyingi dars:** [02 — Oyna va asosiy sikl](02_oyna_va_asosiy_sikl.md)
