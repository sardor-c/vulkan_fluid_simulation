# 03-dars. GPU va Vulkan qanday ishlaydi

## Maqsad

Dars oxirida:
- CPU va GPU nega har xil ishlashini va qanday masala GPU'ga mos kelishini tushunasiz
- Koddagi `vk...()` chaqiruvi GPU'gacha qanday yo'l bosib o'tishini bilasiz
- Vulkan obyektlari **xaritasi**ni ko'rasiz: keyingi darslarda nimalarni va qaysi tartibda
  yaratishimizni oldindan bilib olasiz
- Nega Vulkan shunchalik ko'p kod talab qilishini tushunasiz
- Loyihaga Vulkan'ni ulab, birinchi Vulkan funksiyasini chaqirasiz: versiyani chiqarasiz
- C++ dan: `#define`, `uint32_t` va bitlarga "joylangan" sonlar bilan tanishasiz

Bu dars asosan nazariya. Shoshilmang: keyingi darslardagi har bir qadam shu yerdagi xaritaga
tayanadi.

---

## Nazariya

### 1. CPU va GPU: ikki xil ishchi jamoa

Tasavvur qiling, 100 000 ta test varaqasini tekshirish kerak. Har bir varaqa oson: javoblarni
kalit bilan solishtirish kifoya.

- **CPU** — 8 ta professor. Har biri juda aqlli va tez, istalgan murakkab masalani yechadi.
  Lekin ular atigi 8 ta: har biriga 12 500 tadan varaqa to'g'ri keladi.
- **GPU** — 3 500 ta talaba. Har biri professordan sekinroq va faqat oddiy ishni biladi.
  Lekin ular **bir vaqtda** ishlaydi: har biriga ~28 tadan varaqa to'g'ri keladi.

Bunday ishni talabalar ancha tez tugatadi.

| | CPU | GPU (sizning RTX 3060) |
|---|---|---|
| Yadrolar soni | 6–16 (Task Manager'da ko'rsa bo'ladi) | 3584 |
| Bitta yadro | Juda kuchli: murakkab mantiq, ko'p `if`, tez javob | Oddiy va sekinroq |
| Nimaga zo'r | Ketma-ket, turli-tuman ishlar (o'yin mantiqi, fayllar, OS) | **Bir xil ishni juda ko'p ma'lumotga** qo'llash |

Ekrandagi har bir piksel rangini hisoblash aynan shunday masala: 1280×720 = 921 600 ta piksel,
har biri uchun bir xil formula. GPU shuning uchun ixtiro qilingan.

**Suyuqlik simulatsiyamiz ham shunday masala.** SPH'da har bir zarracha uchun bir xil ish
qilinadi: *"qo'shnilarimni top, zichligimni hisobla, menga ta'sir qilayotgan bosim kuchini
hisobla"*. Bitta zarrachaning hisobi boshqasining hisobini kutmaydi. 2-modulda buni CPU'da
yozamiz va bir necha ming zarrachada to'xtab qolamiz. 3-modulda GPU'ga ko'chiramiz va yuz
minglab zarrachaga chiqamiz.

Yana bir muhim detal: GPU yadrolari **guruh bo'lib** ishlaydi (NVIDIA'da 32 tadan). Guruhdagi
hamma yadro bir paytda **bitta buyruqni** bajaradi, faqat har biri o'z ma'lumoti bilan. Xuddi
sinf bir ovozdan diktant yozgandek. Shuning uchun GPU dasturida (shader'da) `if` qimmatga
tushishi mumkin: guruhning bir qismi `if` ichiga kirsa, qolganlari kutib turadi. Bunga
17-darsda qaytamiz.

### 2. Ikki alohida dunyo: RAM va VRAM

CPU va GPU'ning **xotirasi alohida**:

```
 ┌───────────────────────┐                     ┌───────────────────────┐
 │ CPU                   │    PCIe shinasi     │ GPU                   │
 │                       │◄═══════════════════►│                       │
 │ RAM (operativ xotira) │   tor "ko'prik"     │ VRAM (videoxotira)    │
 │                       │   ~16–32 GB/s       │ o'qish ~360 GB/s      │
 └───────────────────────┘                     └───────────────────────┘
```

GPU o'z VRAM'idan juda tez o'qiydi, lekin CPU bilan GPU orasidagi "ko'prik" ancha tor.
Asosiy qoida: **ma'lumotni GPU'ga bir marta yuboring va o'sha yerda saqlang.** Agar har kadrda
100 000 zarrachani CPU'da hisoblab, keyin GPU'ga ko'chirsak, vaqtning katta qismi "ko'prikda"
o'tadi. 3-modulda fizikani **to'liq** GPU'ga ko'chirishimizning sababi shu.

### 3. CPU va GPU bir-birini kutmaydi

CPU GPU'ga buyruqlarni bittalab yubormaydi. Avval **buyruqlar ro'yxatini** yozadi
(*"ekranni tozala, uchburchak chiz, ..."*), keyin ro'yxatni GPU'ga butunlay uzatadi va
**kutmasdan** o'z ishini davom ettiradi. GPU ro'yxatni o'z vaqtida bajaradi:

```
 vaqt ───────────────────────────────────────────────────────────────────►
 CPU:  [1-kadrni yozish]→uzatish [2-kadrni yozish]→uzatish [3-kadrni yozish] ...
 GPU:                    [1-kadrni bajarish      ][2-kadrni bajarish      ] ...
```

Ikkalasi parallel ishlaydi va bu juda samarali. Lekin muammo ham bor: CPU GPU hali ishlatib
turgan ma'lumotni o'zgartirib qo'yishi mumkin. Shuning uchun Vulkan'da *"GPU shu ishni
tugatdimi?"* deb bilish vositalari bor (**fence**, **semaphore**). Bu **sinxronizatsiya**
deyiladi va 07-darsda o'rganiladi.

**O'xshatish:** restoran menejeri (CPU) buyurtmani qog'ozga yozadi, oshxona derazasidan
uzatadi va darhol keyingi mijozga o'tadi. Oshxona (GPU) taomni o'z vaqtida tayyorlaydi va
*"tayyor!"* qo'ng'irog'ini chaladi.

### 4. Vulkan nima va chaqiruv GPU'gacha qanday yetadi

**Vulkan — kutubxona emas, spetsifikatsiya** (standart). Khronos Group (GPU ishlab
chiqaruvchilari, o'yin kompaniyalari va boshqalarning birlashmasi) qog'ozda *"`vkCreateBuffer`
mana bunday ishlashi kerak"* deb yozib qo'yadi. Haqiqiy kodni esa har bir GPU ishlab
chiqaruvchisi (NVIDIA, AMD, Intel) o'z **drayveri** ichida yozadi.

Chaqiruvingiz GPU'gacha shunday yo'l bosadi:

```
 app.exe          sizning kodingiz: vkCreateInstance(...)
    │
    ▼
 vulkan-1.dll     LOADER (yuklovchi). Khronos yozgan, drayver bilan birga System32 ga
    │             o'rnatilgan. Kompyuterdagi drayverlarni topadi va chaqiruvni kerakli
    │             drayverga yo'naltiradi
    ▼
 [ layer'lar ]    ixtiyoriy "oraliq qatlam"lar. Masalan, validation layer har bir
    │             chaqiruvni tekshiradi (04-dars)
    ▼
 nvoglv64.dll     DRAYVER (ICD). NVIDIA yozgan haqiqiy Vulkan kodi. Buyruqlaringizni
    │             GPU tushunadigan tilga o'giradi
    ▼
 Windows → GPU    buyruqlar va ma'lumotlar videokartaga yetib boradi
```

**Loader nega kerak?** Bitta kompyuterda bir nechta GPU va drayver bo'lishi mumkin (masalan,
noutbukda protsessor ichidagi Intel GPU va alohida NVIDIA). Dastur esa doim bitta
`vulkan-1.dll` bilan gaplashadi, qolganini loader hal qiladi. Shuning uchun 05-darsda avval
GPU'lar ro'yxatini olamiz, keyin ulardan birini **tanlaymiz**.

Linker esa (01-dars) faqat `libvulkan-1.dll.a` import kutubxonasini ko'radi. Unda *"bu
funksiyalar dastur ishga tushganda `vulkan-1.dll` dan olinadi"* degan yozuv bor, xolos.

Shuning uchun uchta har xil **Vulkan versiyasi** bor va ular farq qilishi normal:

| Versiya | Qayerdan keladi | Qachon aniqlanadi | Sizda |
|---|---|---|---|
| **Header** | `vulkan/vulkan.h` (MSYS2 o'rnatgan) | Kompilyatsiya paytida: kompilyator qaysi funksiya va struct'larni "biladi" | 1.4.357 |
| **Loader** (instance) | `vulkan-1.dll` | Dastur ishga tushganda | 1.4.341 |
| **Device** | NVIDIA drayveri, aniq shu GPU uchun | Dastur ishga tushganda (05-darsda o'qiymiz) | 1.4.341 |

Bugun ulardan ikkinchisini o'z kodingiz bilan chiqarasiz.

> MSYS2 ham o'z `vulkan-1.dll` ini olib kelgan (`C:\msys64\mingw64\bin`). Lekin Windows
> DLL'ni avval dastur papkasidan, keyin `System32` dan qidiradi va `PATH` dagi papkalarga
> eng oxirida qaraydi. Shuning uchun dasturingiz drayver o'rnatgan loader'ni yuklaydi.

### 5. Vulkan obyektlari xaritasi

Vulkan'da ishlash — bir nechta obyektni yaratib, ularni bir-biriga ulash. Hammasi **handle**
(02-dars: `GLFWwindow*` ga o'xshash, ichi yopiq tutqich). Qavs ichida — o'sha obyekt
yaratiladigan dars:

```
 VkInstance ────────────── Vulkan bilan aloqa: "dasturim Vulkan ishlatadi"            (04)
  │
  ├── VkSurfaceKHR ─────── GLFW oynasining Vulkan'dagi "yuzasi"                       (06)
  │
  └── VkPhysicalDevice ─── haqiqiy GPU (RTX 3060). Yaratilmaydi, ro'yxatdan tanlanadi (05)
        │
        └── VkDevice ───── logical device: shu GPU bilan ochilgan "ish sessiyasi"     (05)
              │
              ├── VkQueue ──────────── buyruqlar navbati: ro'yxat shu yerga uzatiladi (05)
              ├── VkSwapchainKHR ───── ekranga chiqadigan 2–3 ta rasm navbati         (06)
              │     └── VkImage, VkImageView
              ├── VkCommandPool
              │     └── VkCommandBuffer ── buyruqlar ro'yxati: "tozala", "chiz" ...   (07)
              ├── VkFence, VkSemaphore ── "tayyor!" signallari (sinxronizatsiya)      (07)
              ├── VkShaderModule ───── GPU uchun kompilyatsiya qilingan dastur        (08)
              ├── VkPipelineLayout
              │     └── VkPipeline ─── "qanday chizish/hisoblash"ning to'liq sozlamasi (08)
              ├── VkBuffer + VkDeviceMemory ── GPU xotirasidagi ma'lumot              (09)
              └── VkDescriptorSet ──── shader'ga "ma'lumot qaysi buffer'da" deydi      (10)
```

Xuddi shu xaritani restoran o'xshatishi bilan:

| Obyekt | Restorandagi o'xshashi |
|---|---|
| **Instance** | Restoranlar tarmog'iga a'zo bo'lish shartnomasi |
| **Physical device** | Haqiqiy oshxona binosi va jihozlari |
| **Logical device** | Shu oshxonada ish boshlash: kalitlar endi sizda |
| **Queue** | Buyurtma uzatiladigan deraza |
| **Command buffer** | Buyurtma qog'ozi: avval to'liq yozasiz, keyin bir martada uzatasiz |
| **Pipeline** | Retsept va oshxona liniyasining sozlanishi |
| **Shader** | Retseptdagi har bir oshpaz bajaradigan qadam |
| **Buffer** | Ombordagi masalliq |
| **Descriptor** | "Masalliq qaysi javonda" yorlig'i |
| **Surface** | Mijozlar zali (oyna) |
| **Swapchain** | Zalga navbat bilan chiqarib turiladigan 2–3 ta likopcha |
| **Fence / Semaphore** | "Tayyor!" qo'ng'irog'i |

> Eski darsliklarda (masalan, vulkan-tutorial.com) `VkRenderPass` va `VkFramebuffer` ham bor.
> Vulkan 1.3 dagi **dynamic rendering** ularni ixtiyoriy qilib qo'ydi, biz ularsiz ishlaymiz.
> Kodimiz **Vulkan 1.3** ga mo'ljallanadi: u barcha zamonaviy drayverlarda bor.

#### Nomlash qoidalari

Bu jadvalni bilsangiz, notanish Vulkan kodini ham o'qiy olasiz:

| Ko'rinish | Ma'nosi | Misol |
|---|---|---|
| `Vk...` | Tip: struct yoki handle | `VkInstance`, `VkBufferCreateInfo` |
| `vk...` | Funksiya | `vkCreateInstance` |
| `VK_...` | Konstanta yoki makros | `VK_SUCCESS`, `VK_API_VERSION_1_3` |
| `vkCreate...` / `vkDestroy...` | Yaratish / yo'q qilish. **Doim juft** (02-dars, 4-bo'lim) | `vkCreateBuffer` / `vkDestroyBuffer` |
| `vkGet...`, `vkEnumerate...` | Ma'lumot olish / ro'yxat olish | `vkEnumeratePhysicalDevices` |
| `vkCmd...` | Buyruqni command buffer'ga **yozib qo'yadi**, hozir bajarmaydi! | `vkCmdDraw` |
| `...KHR`, `...EXT` | Kengaytma (extension): standartning asosiy qismidan tashqari imkoniyat | `VkSurfaceKHR` |

Oxirgi qator haqida: ekranga chiqarish (surface, swapchain) Vulkan'ning asosiy qismiga
kirmaydi, chunki Vulkan ekransiz ham ishlay olishi kerak (masalan, serverda faqat hisob-kitob
uchun). Shuning uchun ular kengaytma sifatida yoqiladi. `KHR` — Khronos tasdiqlagan kengaytma,
`EXT` — bir nechta kompaniya kelishib qo'shgan kengaytma.

#### Obyekt qanday yaratiladi: "ariza" usuli

Vulkan funksiyalari ko'p parametr olmaydi. Buning o'rniga siz **struct** to'ldirasiz (xuddi
ariza blankasi kabi) va uni funksiyaga berasiz. Buni hozir yozish shart emas, faqat qarab
chiqing (04-darsda o'zingiz yozasiz):

```cpp
VkInstanceCreateInfo createInfo{};                          // {} — barcha maydonlar 0 bo'ladi
createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;  // "bu qaysi blanka" belgisi
// ... boshqa maydonlar ...

VkInstance instance = VK_NULL_HANDLE;                       // natija shu yerga yoziladi
VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
if (result != VK_SUCCESS) {
    // xato
}
// ... ishlatish ...
vkDestroyInstance(instance, nullptr);
```

Bu yerdagi uchta qoida Vulkan'ning **hamma** joyida takrorlanadi:

1. Har bir `...CreateInfo` struct'ining birinchi maydoni `sType`: struct o'z turini o'zi
   aytadi. Nega bunday qilinganini 04-darsda ko'ramiz.
2. Natija (yangi handle) **pointer orqali** qaytadi: `&instance`. 02-darsdagi
   `glfwGetCursorPos(window, &mouseX, &mouseY)` ham xuddi shunday ishlaydi.
3. Funksiya **`VkResult`** qaytaradi: `VK_SUCCESS` (0) — muvaffaqiyat, manfiy son — xato
   (masalan, `VK_ERROR_OUT_OF_DEVICE_MEMORY`). Natijani doim tekshiring.

O'rtadagi `nullptr` — xotira ajratuvchi (allocator). Biz uni o'zgartirmaymiz, doim `nullptr`.

### 6. Nega Vulkan shunchalik batafsil?

Vulkan'dan oldin **OpenGL** bor edi (u hali ham ishlatiladi). Ekranga uchburchak chizish
uchun OpenGL'da ~100 qator kod yetadi, Vulkan'da esa ~1000 qator kerak. Nega?

**O'xshatish: avtomat va mexanik karobka.** OpenGL — avtomat: gazni bosasiz, mashina qaysi
uzatmaga o'tishni o'zi hal qiladi. Qulay, lekin har doim ham o'z vaqtida emas. Vulkan —
mexanik: har bir uzatmani o'zingiz almashtirasiz. Qiyinroq, lekin mashina to'liq sizning
qo'lingizda.

| | OpenGL | Vulkan |
|---|---|---|
| Kim qaror qiladi | Drayver taxmin qiladi (*"bu buffer endi kerak emasdir, ehtimol"*) | Siz aniq aytasiz (**explicit**) |
| Xatolarni tekshirish | Drayver har chaqiruvda tekshiradi (bu sekin) | Drayver **tekshirmaydi**: xato bo'lsa, crash yoki g'alati natija. Tekshirish uchun validation layer yoqiladi |
| Og'ir ishlar qachon | Chizish paytida, kutilmaganda (o'yinlardagi "qotish") | Oldindan, yaratish paytida (masalan, pipeline). Chizish tez va bir tekis |
| Bir nechta CPU yadrosi | Asosan bitta yadro | Bir nechta yadroda parallel buyruq yozsa bo'ladi |
| Hisob-kitob (compute) | Keyinroq qo'shilgan | Boshidanoq to'liq huquqli |

Biz uchun bu nimani anglatadi:
- **Boshida ko'p kod bo'ladi.** 04–07-darslarda oynani faqat bo'yaymiz, birinchi uchburchak
  08-darsda chiqadi. Bu normal: "tog'"ning eng tik joyi shu.
- Lekin bu kod **bir marta** yoziladi. Keyin yangi narsa (zarrachalar, compute shader)
  qo'shish ancha oson.
- **Validation layer — eng yaqin do'stingiz.** Vulkan'ning o'zi xato haqida deyarli hech
  narsa demaydi. Layer esa *"VkBuffer 0x1a2b yo'q qilinmay qoldi"* darajasida aniq aytadi.
  Shuning uchun keyingi darsni aynan undan boshlaymiz.

### 7. Versiya bitta songa qanday joylanadi

Vulkan versiyani (masalan, 1.4.341) **bitta** 32 bitli songa joylab qaytaradi.

**O'xshatish:** 2026-10-17 sanasini bitta son qilib yozish mumkin: `20261017`. Undan yilni
olish uchun 10 000 ga bo'lamiz (qoldiqni tashlaymiz): `2026`. Kunni olish uchun 100 ga
bo'lgandagi qoldiqni olamiz: `17`. Har bir qismning o'z "xonalari" bor.

Vulkan ham xuddi shunday qiladi, faqat o'nlik xonalar o'rniga **bit**larni ishlatadi:

```
 bitlar:  31 ... 29 | 28 ......... 22 | 21 ............ 12 | 11 ............. 0
          variant   |      major      |       minor        |       patch
          (3 bit)   |     (7 bit)     |      (10 bit)      |      (12 bit)
             0      |        1        |         4          |        341
```

`variant` Vulkan uchun doim 0. Qismlarni ajratib olish uchun tayyor makroslar bor:
`VK_API_VERSION_MAJOR(v)`, `VK_API_VERSION_MINOR(v)`, `VK_API_VERSION_PATCH(v)`. Ularning ichida
nima borligini 1-mashqda o'zingiz ko'rasiz.

**`uint32_t`** — *unsigned integer, 32 bit*: manfiy bo'lmaydigan, aniq 32 bitli butun son.
U `<cstdint>` da e'lon qilingan, Vulkan header'i uni o'zi ulaydi. Oddiy `int` ning o'lchami
platformaga qarab har xil bo'lishi mumkin. GPU bilan ishlashda esa bitlar soni aniq bo'lishi
shart, shuning uchun Vulkan hamma joyda `uint32_t` ishlatadi.

---

## Amaliyot

### 1-qadam: 02-darsdagi tajribalarni tartibga keltiring

`src/main.cpp` da mashqlardan qolgan uchta narsani to'g'rilang:

1. `glfwWaitEvents()` o'rniga yana `glfwPollEvents()` ni qo'ying. Sichqoncha qimirlamasa ham
   simulatsiya davom etishi kerak.
2. `distance` o'zgaruvchisini va uning sarlavhadagi qismini o'chiring: tajriba tugadi.
3. FPS hisobini tuzating. Sarlavhani har **0.5** soniyada yangilayapsiz, lekin `frameCount` ni
   to'g'ridan-to'g'ri FPS deb chiqaryapsiz. 0.5 soniyada 80 ta kadr bo'lsa, sarlavhada "80"
   chiqadi, aslida esa FPS 160. FPS — *kadrlar soni ÷ o'tgan vaqt*:

   ```cpp
   double fps = frameCount / (now - fpsTimer);
   ```

   Shundan keyin sarlavhani yangilash oralig'ini istalgancha o'zgartirsangiz ham FPS to'g'ri
   chiqadi.

### 2-qadam: `CMakeLists.txt` ga Vulkan'ni qo'shing

`find_package(glfw3 REQUIRED)` dan keyin:

```cmake
find_package(Vulkan REQUIRED)
```

va `target_link_libraries` ga yangi target'ni qo'shing:

```cmake
target_link_libraries(app PRIVATE glfw Vulkan::Vulkan)
```

- `find_package(Vulkan REQUIRED)` — bu safar "pasport"ni CMake'ning o'zi biladi: unda tayyor
  `FindVulkan.cmake` moduli bor. U Vulkan header'larini (`C:/msys64/mingw64/include/vulkan/`)
  va import kutubxonasini (`libvulkan-1.dll.a`) qidiradi. Vulkan SDK o'rnatilgan kompyuterda
  esa `VULKAN_SDK` muhit o'zgaruvchisiga qaraydi.
- `Vulkan::Vulkan` — topilgan paket e'lon qilgan target: header'lar va kutubxona birga.
  `::` — *"Vulkan paketidagi Vulkan target'i"* degani.

GLFW target'i nega `glfw::glfw` emas, shunchaki `glfw`? Nomni har bir paket o'zi tanlaydi.
`::` li nomlarning afzalligi shundaki, nomni xato yozsangiz, CMake configure paytidayoq
*"bunday target yo'q"* deydi. `::` siz nom xato yozilsa, CMake uni oddiy kutubxona fayli deb
o'ylaydi va xato faqat linker bosqichida chiqadi.

### 3-qadam: Vulkan header'ini ulang

`main.cpp` ning boshidagi GLFW qatorini shunday o'zgartiring:

```cpp
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
```

`#define` — preprocessor buyrug'i (01-dars). U `GLFW_INCLUDE_VULKAN` degan "bayroq"ni ko'taradi.
`glfw3.h` ichida esa *"bu bayroq ko'tarilgan bo'lsa, `<vulkan/vulkan.h>` ni ham ula va GLFW'ning
Vulkan bilan ishlaydigan funksiyalarini och"* degan shart yozilgan. Preprocessor faylni yuqoridan
pastga o'qiydi, shuning uchun `#define` `#include` dan **oldin** turishi shart. GLFW'ning bu
funksiyalari (masalan, oynadan surface yaratish) 06-darsda kerak bo'ladi.

### 4-qadam: birinchi Vulkan funksiyasi

`main` dan **oldin** yangi funksiya yozing. (Nega oldin? 01-dars: kompilyator funksiyani
ishlatilishidan oldin "ko'rgan" bo'lishi kerak.)

```cpp
// Vulkan loader qo'llab-quvvatlaydigan versiyani konsolga chiqaradi.
// Muvaffaqiyatli bo'lsa true qaytaradi.
bool printVulkanVersion() {
    uint32_t version = 0;
    VkResult result = vkEnumerateInstanceVersion(&version);
    if (result != VK_SUCCESS) {
        std::cerr << "vkEnumerateInstanceVersion xato qaytardi: " << result << std::endl;
        return false;
    }

    std::cout << "Vulkan (loader) versiyasi: "
              << VK_API_VERSION_MAJOR(version) << "."
              << VK_API_VERSION_MINOR(version) << "."
              << VK_API_VERSION_PATCH(version) << std::endl;
    return true;
}
```

Uni `main` ning eng boshida, `glfwInit()` dan oldin chaqiring:

```cpp
int main() {
    if (!printVulkanVersion()) {
        return 1;
    }

    if (!glfwInit()) {
        // ... avvalgidek
```

Qatorma-qator:

| Kod | Ma'nosi |
|---|---|
| `uint32_t version = 0;` | Javob yoziladigan "quti". Boshlang'ich qiymat beramiz: funksiya xato qilsa ham ichida tasodifiy "axlat" qolmaydi |
| `vkEnumerateInstanceVersion(&version)` | Loader'dan *"qaysi Vulkan versiyasini bilasan?"* deb so'raydi va javobni `version` ning **manziliga** yozib qo'yadi |
| `VkResult result = ...` | Natija kodi: har bir Vulkan funksiyasi shuni qaytaradi |
| `if (result != VK_SUCCESS)` | Xatoni tekshirish. `result` ni `<<` bilan chiqarsak, son ko'rinadi (masalan, `-1`). Uning nomini `vulkan_core.h` dagi `VkResult` ro'yxatidan topsa bo'ladi |
| `VK_API_VERSION_MAJOR(version)` va qolganlari | Bitta sondan major, minor va patch qismlarini ajratib oladi (Nazariya, 7-bo'lim) |
| `return false;` / `return true;` | Natijaga qarab `main` dasturni to'xtatadi yoki davom ettiradi |

`vkEnumerateInstanceVersion` ga hali `VkInstance` kerak emas: u Vulkan "ishga tushishidan"
oldin ham chaqirsa bo'ladigan kam sonli funksiyalardan biri.

### 5-qadam: build va ishga tushirish

```powershell
cmake --build build
.\build\app.exe
```

---

## Tekshiruv

Konsolda, oyna ochilishidan oldin shunday qator chiqadi:

```
Vulkan (loader) versiyasi: 1.4.341
```

Raqam drayveringizga bog'liq: drayver yangilansa, u ham o'zgaradi. Asosiysi, `1.3` yoki undan
yuqori bo'lsin. Keyin oyna 02-darsdagidek ochiladi, sarlavhadagi FPS esa endi to'g'ri
hisoblanadi.

Konsolda `EOSOverlayVkLayer-Win64.json` topilmadi degan xabar chiqsa, e'tibor bermang. Bu Epic
Games o'rnatgan overlay qatlami: ro'yxatga yozib qo'yilgan, lekin fayli o'chib ketgan. Zararsiz.

Xato chiqsa, avval **qaysi bosqichda** chiqqanini aniqlang (01-darsdagi "compiler yoki linker?"
jadvali):

| Xato matni | Bosqich | Sabab |
|---|---|---|
| `Could NOT find Vulkan` | configure (CMake) | Vulkan header'lari yoki import kutubxonasi o'rnatilmagan (`mingw-w64-x86_64-vulkan-headers`, `mingw-w64-x86_64-vulkan-loader`) |
| `'VkResult' was not declared in this scope` | compiler | `#define GLFW_INCLUDE_VULKAN` yozilmagan yoki `#include` dan **keyin** yozilgan |
| `undefined reference to 'vkEnumerateInstanceVersion'` | linker | `target_link_libraries` ga `Vulkan::Vulkan` qo'shilmagan |

---

## Mashqlar

1. **Bitlarni o'zingiz ajrating.** `version` ni xom (raw) son sifatida ham chiqaring.
   1.4.341 uchun u `4211029` bo'ladi. Endi qismlarni makroslarsiz, o'zingiz ajrating.
   Ikkita yangi operator kerak:
   - `a >> n` — bitlarni o'ngga `n` ta surish. Eng o'ngdagi `n` ta bit tushib qoladi.
     O'nlik sonda bir xona surish 10 ga bo'lishga o'xshaydi: `20261017` → `2026101`.
   - `a & mask` — faqat `mask` da 1 turgan bitlarni qoldirish, qolganlari 0 bo'ladi.
     O'nlik sonda "faqat oxirgi 2 xonani qoldirish"ga o'xshaydi.

   Major uchun javob: `(version >> 22) & 0x7F`. `0x7F` — o'n oltilik (hex) son, ikkilikda
   7 ta bir: `1111111`. Minor va patch'ni o'zingiz yozing va makroslar natijasi bilan
   solishtiring. Keyin VS Code'da `VK_API_VERSION_MINOR` ustiga borib **F12** ni bosing:
   makros `vulkan_core.h` da qanday yozilgan, siznikiga o'xshaydimi?

2. **Header va loader.** Header versiyasini ham xuddi shu makroslar bilan chiqaring:
   `VK_HEADER_VERSION_COMPLETE`. Ikkala raqam bir xilmi? Qaysi biri kompilyatsiya paytida
   `app.exe` ichida "qotib qoladi", qaysi biri har safar ishga tushganda aniqlanadi?
   `app.exe` ni eski drayverli boshqa kompyuterda ishga tushirsangiz, qaysi raqam o'zgaradi?

3. **GLFW Vulkan'ni ko'ryaptimi?** `glfwInit()` dan keyin `glfwVulkanSupported()` ni
   tekshiring. U `GLFW_FALSE` qaytarsa, xato chiqarib, dasturdan chiqing. Chiqishdan oldin
   nimani tozalash kerak?

4. **GLFW'ga qaysi kengaytmalar kerak?** Vulkan oyna bilan ishlashi uchun kengaytmalar
   (extension) yoqilishi kerak. Qaysilari ekanini GLFW biladi. `glfwInit()` dan keyin:
   ```cpp
   uint32_t count = 0;
   const char** names = glfwGetRequiredInstanceExtensions(&count);
   ```
   `names` — matnlar ro'yxati: `names[0]` birinchi kengaytma nomi, `names[1]` ikkinchisi va
   hokazo, jami `count` ta. Ularni `for` sikl bilan konsolga chiqaring. Nechta chiqdi? Nomlarida
   qanday so'zlar bor? Nega aynan shular kerakligini taxmin qiling (06-darsda tekshiramiz).

5. **Compiler yoki linker?** `target_link_libraries` dan `Vulkan::Vulkan` ni vaqtincha olib
   tashlab, build qiling. Xato qaysi bosqichda chiqdi? Nega kompilyator `vulkan.h` ni baribir
   topdi? (Maslahat: MSYS2 header'larni qaysi papkaga o'rnatgan va `g++` qaysi papkalarni
   hech kim aytmasa ham o'zi ko'radi?) Keyin qaytarib qo'ying.

6. **`vulkaninfo`.** PowerShell'da `vulkaninfo --summary` ni ishga tushiring (bu dastur NVIDIA
   drayveri bilan birga o'rnatilgan). Natijadan toping: `Vulkan Instance Version`, GPU nomi
   (`deviceName`), `deviceType`, `apiVersion`. Ularni o'z dasturingiz chiqargan raqam bilan
   solishtiring. `Instance Layers` ro'yxatida qaysi layer'lar bor?

---

## O'zingizni tekshiring

1. CPU va GPU qanday farq qiladi? Qanday masalalar GPU'ga mos keladi va nega SPH ulardan biri?
2. Unda nega hamma narsani GPU'da qilmaymiz? (Masalan, fayl o'qish yoki menyu mantiqini.)
3. Nega CPU va GPU orasida har kadrda ko'p ma'lumot ko'chirib turish yomon?
4. *"CPU GPU'ni kutmaydi"* — bu nima degani? Bundan qanday muammo kelib chiqishi mumkin?
5. Vulkan — kutubxonami? `vkCreateBuffer` ning haqiqiy kodi qayerda yozilgan?
6. Loader nima ish qiladi? Nega dastur drayver bilan to'g'ridan-to'g'ri gaplashmaydi?
7. `VkPhysicalDevice` va `VkDevice` qanday farq qiladi?
8. `vkCmdDraw` chaqirilganda GPU darhol chizadimi?
9. Vulkan drayveri xatolarni tekshirmasa, ularni qanday topamiz?

---

**Oldingi dars:** [02 — Oyna va asosiy sikl](02_oyna_va_asosiy_sikl.md) ·
**Keyingi dars:** [04 — Instance va validation layer](04_instance_va_validation_layer.md)
