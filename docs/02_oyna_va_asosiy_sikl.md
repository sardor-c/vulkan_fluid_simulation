# 02-dars. Oyna va asosiy sikl (GLFW)

## Maqsad

Dars oxirida:
- Loyihaga tashqi kutubxonani (**GLFW**) ulay olasiz
- Ekranda oyna ochiladi va `Esc` bilan yopiladi
- Har qanday simulatsiyaning yuragi — **asosiy sikl** (main loop) va **delta time** ni tushunasiz
- Oyna sarlavhasida FPS ko'rinadi
- C++ dan: **pointer**, `nullptr`, **lambda** va **callback** bilan tanishasiz

---

## Nazariya

### 1. Oyna va hodisalar (events)

Oynalarni operatsion tizim (Windows) boshqaradi. Foydalanuvchi tugma bossa, sichqonchani
qimirlatsa yoki oyna o'lchamini o'zgartirsa, OS dasturga **hodisa** (event) xabarini yuboradi.
Xabarlar navbatga (queue) tushadi.

```
 Foydalanuvchi ──► Windows ──► [hodisalar navbati] ──► sizning dasturingiz (o'qib oladi)
   (tugma,                       ESC bosildi
   sichqoncha)                   sichqoncha (350, 200) ga siljidi
                                 oyna 1280x720 bo'ldi
```

Dastur bu navbatni **muntazam o'qib turishi** shart. Aks holda Windows oynani
*"Not responding"* (javob bermayapti) deb belgilaydi.

Har bir OS'ning oyna bilan ishlash API'si boshqacha: Windows'da Win32, Linux'da X11 yoki
Wayland, macOS'da Cocoa. **GLFW** ularning hammasini bitta oddiy API ortiga yashiradi.
Bundan tashqari, GLFW oynani Vulkan bilan bog'lashni ham biladi (06-dars).

### 2. `GLFW_NO_API`

GLFW standart holatda oyna uchun **OpenGL** kontekstini ham yaratadi (OpenGL — Vulkan'dan
oldingi, eskiroq grafik API). Biz Vulkan ishlatamiz, shuning uchun GLFW'ga oldindan aytib
qo'yamiz:

```cpp
glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // "OpenGL kerak emas"
```

### 3. Pointer (ko'rsatkich) va handle

Har bir o'zgaruvchi xotirada biror **manzilda** (address) saqlanadi. Manzil — oddiy son,
uyning raqamiga o'xshaydi. **Pointer** — manzilni saqlaydigan o'zgaruvchi.

```cpp
int a = 5;
int* p = &a; // &a — "a ning manzili". p endi a ga "ko'rsatadi"
*p = 7;      // *p — "p ko'rsatgan joydagi qiymat". Endi a == 7
```

```
 xotira:   manzil 1000: [ a = 7 ]  ◄───┐
           manzil 2000: [ p = 1000 ] ──┘  (p ichida a ning manzili turibdi)
```

`glfwCreateWindow` oyna haqidagi ichki ma'lumotlarni xotirada qayerdadir yaratadi va bizga
faqat **manzilini** qaytaradi: `GLFWwindow* window`. Bu strukturaning ichida nima borligini
bilmaymiz va bilishimiz ham shart emas. Biz uni faqat GLFW funksiyalariga qaytarib beramiz.
Bunday "ichi yopiq" tutqich **handle** deyiladi. Vulkan'da ham hamma narsa handle orqali
ishlaydi: `VkInstance`, `VkDevice`, `VkBuffer`...

**`nullptr`** — "hech narsaga ko'rsatmaydigan" pointer. Ko'p funksiyalar muvaffaqiyatsiz
bo'lganda `nullptr` qaytaradi, shuning uchun natijani **doim tekshiring**:

```cpp
if (window == nullptr) { /* xato */ }
```

### 4. Yaratdingmi — yo'q qil (teskari tartibda)

| Yaratish | Yo'q qilish |
|---|---|
| `glfwInit()` | `glfwTerminate()` |
| `glfwCreateWindow(...)` | `glfwDestroyWindow(window)` |

Yo'q qilish yaratishga **teskari tartibda** bo'ladi: oyna GLFW ichida yashaydi, shuning uchun
avval oyna, keyin GLFW o'chiriladi. Uyni buzishda ham avval tomni, keyin devorlarni olasiz.

Bu qoida Vulkan'da juda muhim bo'ladi: har bir `vkCreate...` uchun `vkDestroy...` bor va
tartib buzilsa, validation layer (04-dars) darhol xato beradi.

### 5. Asosiy sikl (main loop)

Har qanday o'yin yoki simulatsiyaning yuragi — bitta `while` sikl:

```
while (oyna ochiq)
{
    1. Hodisalarni o'qish   (input:  qaysi tugma bosildi, sichqoncha qayerda)
    2. Holatni yangilash    (update: fizika shu yerda — zarrachalarni siljitish)
    3. Chizish              (render: natijani ekranga chiqarish)
}
```

Siklning har bir aylanishi — bitta **kadr** (frame). Soniyadagi kadrlar soni — **FPS**
(frames per second).

### 6. Delta time — eng muhim tushuncha

Kadrlar orasidagi vaqt doim har xil: kompyuter tezligiga, ekran chastotasiga va o'sha paytdagi
yuklamaga bog'liq. Faraz qilaylik, zarrachani **har kadrda 1 piksel** siljitamiz:

| Kompyuter | FPS | 1 soniyada siljish |
|---|---|---|
| Sekin | 30 | 30 piksel |
| Tez | 144 | 144 piksel |

Bir xil dastur turli kompyuterlarda turli tezlikda ishlayapti. Bu noto'g'ri.

To'g'ri yo'l — fizikadagi formula: **masofa = tezlik × vaqt**. Bu yerda "vaqt" — oxirgi
kadrdan beri o'tgan vaqt, ya'ni **delta time** (`dt`, soniyalarda):

```cpp
position = position + velocity * deltaTime;
```

| FPS | `dt` | Tezlik 2 m/s bo'lsa, 1 kadrdagi siljish | 1 soniyadagi siljish |
|---|---|---|---|
| 30 | 0.0333 s | 0.0667 m | **2 m** |
| 144 | 0.0069 s | 0.0139 m | **2 m** |

Endi natija FPS'ga bog'liq emas. Simulatsiyamizdagi har bir hisob-kitob `dt` bilan bo'ladi.

### 7. Input: polling va callback

Tugmani ikki usulda bilish mumkin:

- **Polling** (so'rab turish): har kadrda *"Esc hozir bosilganmi?"* deb so'raymiz.
  ```cpp
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { ... }
  ```
  *Holat* uchun qulay: "tugma bosib turilibdi" (masalan, sichqoncha tugmasini bosib turib
  suyuqlikni tortish, 16-dars).
- **Callback** (qayta chaqiruv): GLFW'ga bitta funksiya beramiz va tugma bosilganda GLFW o'sha
  funksiyani **o'zi chaqiradi**. *Bir martalik hodisa* uchun qulay: "Space **bosildi**" →
  pauza holati bir marta almashadi. Polling bilan bo'lsa, tugmani bosib turgan har kadrda
  almashib ketardi.

### 8. Lambda

Callback uchun alohida nomli funksiya yozish shart emas. Uni joyida, **lambda** sifatida
yozsa bo'ladi. Lambda — nomsiz funksiya:

```cpp
auto doubleIt = [](int x) { return x * 2; };
int y = doubleIt(21); // 42
```

- `[]` — "capture" ro'yxati: lambda tashqaridagi o'zgaruvchilardan qaysilarini "ushlab
  olishi". Bo'sh `[]` hech narsani ushlamaydi. GLFW faqat shunday lambda'ni qabul qiladi,
  chunki u oddiy C funksiyasiga aylana oladi.
- `(int x)` — parametrlar, `{ ... }` — tanasi.

Parametr nomini yozmasangiz, kompilyator "bu parametr ishlatilmayapti" deb ogohlantirmaydi:
`int /*scancode*/`. Izohdagi nom esa o'quvchiga uning nima ekanini eslatib turadi.

### 9. CMake: tashqi kutubxonani ulash

```cmake
find_package(glfw3 REQUIRED)
target_link_libraries(app PRIVATE glfw)
```

- `find_package(glfw3 REQUIRED)` — GLFW o'rnatilganda qo'yib ketgan "pasport" faylini
  (`C:/msys64/mingw64/lib/cmake/glfw3/glfw3Config.cmake`) topadi. Unda header'lar va kutubxona
  fayli qayerda ekani yozilgan. `REQUIRED` — topilmasa, configure shu yerning o'zida xato
  bilan to'xtaydi.
- `target_link_libraries(app PRIVATE glfw)` — `app` uchun GLFW header'larini ko'rinadigan
  qiladi va kutubxonani linkerga beradi. `glfw` — GLFW paketi e'lon qilgan target nomi.

GLFW dynamic kutubxona sifatida ulanadi, ya'ni ishga tushganda `glfw3.dll` kerak bo'ladi.
U `C:\msys64\mingw64\bin` da turibdi va bu papka `PATH` da bor, shuning uchun hammasi ishlaydi.
Agar `app.exe` ni boshqa kompyuterda ishga tushirsangiz va *"glfw3.dll not found"* chiqsa,
sababi shu.

---

## Amaliyot

### 1-qadam: `CMakeLists.txt` ga GLFW'ni qo'shing

`add_executable` dan **oldin** `find_package`, keyin esa `target_link_libraries`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(fluid_sim_tutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)

add_executable(app
    src/main.cpp
    src/vec2.cpp
)

target_link_libraries(app PRIVATE glfw)
target_compile_options(app PRIVATE -Wall -Wextra)
```

`vec2.cpp` loyihada qoladi: 11-darsda kerak bo'ladi.

### 2-qadam: `src/main.cpp` ni qayta yozing

```cpp
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

int main()
{
    // 1. GLFW ni ishga tushirish
    if (!glfwInit())
    {
        std::cerr << "GLFW ishga tushmadi\n";
        return 1;
    }

    // 2. Oyna yaratish (OpenGL kerak emas: biz Vulkan ishlatamiz)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sim Tutorial", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Oyna yaratilmadi\n";
        glfwTerminate();
        return 1;
    }

    // 3. Klaviatura callback'i: tugma bosilganda GLFW shu lambda'ni chaqiradi
    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int /*scancode*/, int action, int /*mods*/) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(w, GLFW_TRUE);
        }
    });

    // 4. Asosiy sikl
    double lastTime = glfwGetTime(); // GLFW ishga tushgandan beri o'tgan soniyalar
    double fpsTimer = lastTime;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // navbatdagi hodisalarni o'qish (callback'lar shu yerda chaqiriladi)

        double now = glfwGetTime();
        double deltaTime = now - lastTime; // oxirgi kadrdan beri o'tgan vaqt (soniya)
        lastTime = now;

        // update(deltaTime);  -- fizika shu yerda bo'ladi (11-dars)
        // render();           -- chizish shu yerda bo'ladi (07-dars)

        // Har soniyada bir marta FPS'ni sarlavhaga yozamiz
        frameCount++;
        if (now - fpsTimer >= 1.0)
        {
            std::string title = "Fluid Sim Tutorial | FPS: " + std::to_string(frameCount) +
                                " | kadr vaqti: " + std::to_string(deltaTime * 1000.0) + " ms";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            fpsTimer = now;
        }
    }

    // 5. Tozalash: yaratilgan tartibga teskari
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

Yangi narsalar:

| Kod | Ma'nosi |
|---|---|
| `std::cerr` | Xato xabarlari uchun chiqish (`std::cout` ning "xato" varianti) |
| `return 1;` | Dastur xato bilan tugadi |
| `glfwGetTime()` | GLFW ishga tushgandan beri o'tgan vaqt, soniyalarda (`double`) |
| `glfwPollEvents()` | Navbatdagi hodisalarni qayta ishlash. Callback'lar shu chaqiruv ichida ishlaydi |
| `std::string`, `std::to_string` | Matn tipi va sonni matnga aylantirish. `+` matnlarni ulaydi |
| `title.c_str()` | `std::string` ni C uslubidagi matnga (`const char*`) aylantirish. GLFW C tilida yozilgan, shuning uchun shuni talab qiladi |

### 3-qadam: build va ishga tushirish

```powershell
cmake --build build
.\build\app.exe
```

`CMakeLists.txt` o'zgargani uchun bu buyruq configure'ni ham o'zi qayta bajaradi.

---

## Tekshiruv

- 1280×720 o'lchamli qora (yoki bo'sh) oyna ochiladi.
- Sarlavhada FPS va kadr vaqti ko'rinadi.
- `Esc` yoki oynadagi ✕ tugmasi dasturni yopadi.

**FPS juda katta bo'ladi** (yuz minglab) va protsessorning bitta yadrosi 100% band bo'ladi.
Bu xato emas: siklimiz hozircha hech narsa qilmayapti va hech narsani kutmayapti. 07-darsda
Vulkan ekranga chizishni boshlaganda, kadrlar monitor chastotasiga (60/144/165 Hz) bog'lanadi
(**vsync**) va FPS o'z-o'zidan shu songa tushadi.

---

## Mashqlar

1. **Pauza.** `Space` bosilganda `isPaused` holati almashsin va pauza paytida sarlavhaga
   `| PAUZA` qo'shilsin. Hozircha `isPaused` ni `main` dan oldin global o'zgaruvchi qilib e'lon
   qiling (`bool isPaused = false;`). Keyinroq buni klass ichiga olishni o'rganamiz.
   *Savol:* nega buni polling emas, callback bilan qilish to'g'ri?
2. **O'lcham o'zgarishi.** `glfwSetFramebufferSizeCallback` bilan oyna o'lchami o'zgarganda
   konsolga yangi o'lchamni chiqaring. (Bu 06-darsda swapchain'ni qayta yaratish uchun kerak
   bo'ladi.)
3. **Sichqoncha.** Har kadrda `glfwGetCursorPos(window, &x, &y)` bilan sichqoncha pozitsiyasini
   oling va sarlavhada ko'rsating. `&x` nima uchun kerak? (Pointer bo'limini eslang: funksiya
   natijani o'sha manzilga yozib qo'yadi.)
4. **Delta time tajribasi.** `double distance = 0;` o'zgaruvchisini olib, har kadrda
   `distance += 100.0 * deltaTime;` qiling va sarlavhada ko'rsating. Taxminan 10 soniyadan keyin
   qiymat ~1000 bo'lishi kerak. Keyin uni `distance += 1.0;` ga almashtiring: endi nima
   bo'ladi va nega?
5. **(Izlanish)** `glfwPollEvents` o'rniga `glfwWaitEvents` ni sinab ko'ring. FPS va protsessor
   yuklamasi qanday o'zgardi? Nega simulatsiyada `glfwWaitEvents` ishlatib bo'lmaydi?

---

## O'zingizni tekshiring

1. Nega dastur hodisalar navbatini muntazam o'qib turishi kerak?
2. Pointer nima? Handle nima? Ikkalasiga misol keltiring.
3. Nega `glfwCreateWindow` natijasini `nullptr` ga tekshiramiz?
4. Polling va callback qanday farq qiladi? Qaysi holatda qaysi biri qulay?
5. Delta time nima va u bo'lmasa simulatsiyada nima muammo bo'ladi?
6. Nega resurslar yaratilish tartibiga teskari tartibda yo'q qilinadi?
7. `find_package` va `target_link_libraries` nima qiladi?

---

**Oldingi dars:** [01 — Muhit va kompilyatsiya](01_muhit_va_kompilyatsiya.md) ·
**Keyingi dars:** [03 — GPU va Vulkan qanday ishlaydi](03_gpu_va_vulkan.md)
