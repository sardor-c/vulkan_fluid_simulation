# 05-dars. Physical device, logical device va queue

## Maqsad

Dars oxirida:
- Kompyuterdagi GPU'lar ro'yxatini olasiz, ularning xossalarini o'qiysiz va mosini **tanlaysiz**
- **Queue family** nima ekanini tushunasiz va graphics + compute ishlay oladigan navbatni topasiz
- **Logical device** (`VkDevice`) yaratib, Vulkan 1.3 imkoniyatlarini (`dynamicRendering`,
  `synchronization2`) yoqasiz
- Buyruqlar yuboriladigan **`VkQueue`** ni olasiz
- C++ dan: **destructor** va **RAII**, `std::optional`, nusxalashni taqiqlash (`= delete`)

04-darsda *"xato bo'lganda tozalash ishlamaydi, keyingi darslarda hal qilamiz"* degan edik.
Bu darsda sizning `FluidSimulation2D` klassingiz yordamida shu muammoni hal qilamiz.

---

## Nazariya

### 1. Physical device va logical device

03-darsdagi xaritani eslang:

```
 VkInstance
  └── VkPhysicalDevice ─── haqiqiy GPU. Yaratilmaydi, ro'yxatdan tanlanadi
        └── VkDevice ───── logical device: shu GPU bilan ochilgan "ish sessiyasi"
              └── VkQueue ─ buyruqlar navbati (device bilan birga tug'iladi)
```

| | `VkPhysicalDevice` | `VkDevice` |
|---|---|---|
| Nima | Haqiqiy GPU haqidagi ma'lumot | Shu GPU bilan ishlash uchun ochilgan sessiya |
| Qayerdan | `vkEnumeratePhysicalDevices` ro'yxatidan | `vkCreateDevice` bilan **yaratiladi** |
| Yo'q qilinadimi | Yo'q, uni instance boshqaradi | Ha, `vkDestroyDevice` |
| Nima uchun | GPU haqida so'rash: nomi, imkoniyatlari, navbatlari | Deyarli **hamma narsa** shundan yaratiladi: buffer, pipeline, command pool... |

**O'xshatish:** physical device — oshxona binosi va jihozlari. Uni ko'rib chiqasiz: *"pechlari
nechta, muzlatkichi bormi?"*. Logical device — shu oshxonani ijaraga olish shartnomasi. Shartnomada
*"menga 1 ta deraza va shu jihozlar kerak"* deb yozasiz. Shundan keyingina ishlay olasiz.

### 2. GPU haqida nimalarni so'rash mumkin

| Ma'lumot | Funksiya | Misol |
|---|---|---|
| **Properties** (xossalar) | `vkGetPhysicalDeviceProperties` | `deviceName`, `deviceType` (diskret/integrated), `apiVersion`, `limits` |
| **Features** (imkoniyatlar) | `vkGetPhysicalDeviceFeatures2` | `dynamicRendering`, `synchronization2`, `geometryShader` |
| **Queue families** | `vkGetPhysicalDeviceQueueFamilyProperties` | Qanday navbatlar bor (3-bo'lim) |
| **Memory** | `vkGetPhysicalDeviceMemoryProperties` | VRAM hajmi va turlari (09-dars) |

Bularning hech biri `VkResult` qaytarmaydi: ma'lumot o'qish xato bilan tugamaydi.

Kompyuteringizda bitta GPU bor (`NVIDIA GeForce RTX 3060`). Lekin kodni xuddi bir nechta GPU
bordek yozamiz: noutbukda odatda ikkita bo'ladi (protsessor ichidagi integrated va alohida
diskret GPU), va biz **diskretini** afzal ko'ramiz.

### 3. Queue family: navbatlar oilasi

03-darsda queue'ni *"buyurtma uzatiladigan deraza"* degan edik. GPU'da bunday derazalar ko'p
va ular **oilalarga** bo'lingan. Bitta oiladagi derazalar bir xil turdagi buyurtmalarni qabul
qiladi. Sizning RTX 3060 dagi oilalar (`vulkaninfo` dan):

| Family | Queue soni | Nima qila oladi |
|---|---|---|
| **0** | 16 | GRAPHICS, COMPUTE, TRANSFER |
| 1 | 2 | TRANSFER |
| 2 | 8 | COMPUTE, TRANSFER |
| 3 | 1 | TRANSFER, VIDEO_DECODE |
| 4 | 1 | TRANSFER, VIDEO_ENCODE |
| 5 | 1 | TRANSFER, OPTICAL_FLOW (NVIDIA'ga xos) |

| Bayroq | Ma'nosi |
|---|---|
| `GRAPHICS` | Chizish buyruqlari (`vkCmdDraw`) |
| `COMPUTE` | Hisob-kitob buyruqlari (`vkCmdDispatch`, 17-dars) |
| `TRANSFER` | Ma'lumot ko'chirish (`vkCmdCopyBuffer`, 09-dars) |

Bizga **chizish** (zarrachalar) va **hisob-kitob** (fizika) kerak. Eng oddiy yo'l: ikkalasini
ham bajara oladigan **bitta** navbat olish. Sizda bu family 0. Alohida compute family (2) dan
foydalanish ("async compute") — murakkab optimizatsiya, bizga kerak emas: bitta navbat bilan
sinxronizatsiya ancha sodda bo'ladi.

Kerakli family'ni topishda 03–04-darslardagi bit operatsiyalari qo'l keladi:

```cpp
const VkQueueFlags required = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

(flags & required) == required   // IKKALA bit ham bor
(flags & required) != 0          // kamida BITTASI bor (bizga bu yetmaydi)
```

Family 2 uchun ikkinchi shart `true`, birinchisi esa `false` bo'ladi: unda COMPUTE bor, GRAPHICS yo'q.

> Oynaga chizish uchun navbat **present** (ekranga chiqarish) ham qila olishi kerak. Buni
> tekshirish uchun surface kerak, shuning uchun 06-darsda qo'shamiz.

### 4. Features: "qila oladi" va "yoqilgan" — ikki xil narsa

GPU biror imkoniyatni **qo'llab-quvvatlashi** uni ishlatish mumkin degani emas. Imkoniyat device
yaratilayotganda **yoqilishi** kerak. Yoqilmagan imkoniyatni ishlatsangiz, validation layer xato
beradi.

**O'xshatish:** yangi telefonda NFC va modem rejimi bor, lekin sozlamalarda yoqmaguningizcha
ishlamaydi. Vulkan'ning explicit tabiati (03-dars): drayver hech narsani o'zi taxmin qilmaydi.

Shuning uchun ikki qadam bor:

```
 1. So'rash:   vkGetPhysicalDeviceFeatures2(gpu, ...)   → "nimalarni qila olasan?"
 2. Yoqish:    VkDeviceCreateInfo ichida                 → "menga mana shular kerak"
```

Bizga kerak bo'lgan ikkita imkoniyat (ikkalasi Vulkan 1.3 da kelgan):

| Imkoniyat | Nima beradi | Qayerda kerak |
|---|---|---|
| `dynamicRendering` | `VkRenderPass` va `VkFramebuffer` siz chizish (03-dars eslatmasi) | 07–08-dars |
| `synchronization2` | Sinxronizatsiyaning soddaroq, tushunarliroq API'si | 07-dars |

**Faqat keraklilarini yoqing.** Hamma imkoniyatni yoqib qo'yish ham mumkin, lekin ba'zilari
drayverni sekinlashtiradi. Bundan tashqari, nimaga tayanayotganingizni ham bilmay qolasiz.

### 5. `pNext` endi javob olish uchun

04-darsda `pNext` zanjiri orqali Vulkan'ga **ma'lumot berdik**. Bu safar u orqali **ma'lumot
olamiz**. Vulkan 1.3 imkoniyatlari alohida struct'da (`VkPhysicalDeviceVulkan13Features`)
turadi va asosiy so'rov struct'iga ulanadi:

```
 VkPhysicalDeviceFeatures2              VkPhysicalDeviceVulkan13Features
 ┌──────────────────────────┐           ┌─────────────────────────────────┐
 │ sType = ..._FEATURES_2   │           │ sType = ..._VULKAN_1_3_FEATURES │
 │ pNext ───────────────────┼──────────►│ pNext = nullptr                 │
 │ features (1.0 dagilar)   │           │ dynamicRendering   = ?          │
 └──────────────────────────┘           │ synchronization2   = ?          │
                                        └─────────────────────────────────┘
             vkGetPhysicalDeviceFeatures2 zanjirdagi har bir "?" ni to'ldiradi
```

Drayver zanjir bo'ylab yuradi, har bir struct'ning `sType` iga qarab uning qaysi struct
ekanini aniqlaydi va maydonlarini to'ldiradi. 04-darsdagi *"pochtachi yorliqqa qaraydi"*
o'xshatishi bu yerda ham ishlaydi.

### 6. Queue priority

Device yaratayotganda qaysi family'dan nechta navbat kerakligini aytasiz. Har bir navbatga
**ustuvorlik** (priority) beriladi: 0.0 dan 1.0 gacha son. U bir device ichidagi navbatlar
orasida *"qaysi biriga ko'proq vaqt berilsin"* degan maslahat. Bizda bitta navbat bor, shuning
uchun bu son ahamiyatsiz. Lekin uni berish **majburiy**: `1.0f`.

### 7. C++: `std::optional` — "bo'lishi ham, bo'lmasligi ham mumkin"

Queue family topilmasligi ham mumkin. Natijani qanday qaytaramiz?

- `-1` qaytarsakmi? Indeks `uint32_t`: u manfiy bo'la olmaydi. `-1` unda `4294967295` ga aylanadi.
- `0` qaytarsakmi? `0` — haqiqiy family indeksi (sizda aynan kerakli family 0).

**`std::optional<uint32_t>`** — ichida son bo'lishi yoki bo'sh bo'lishi mumkin bo'lgan quti:

```cpp
#include <optional>

std::optional<uint32_t> a = 5;              // ichida 5 bor
std::optional<uint32_t> b = std::nullopt;   // bo'sh

if (a.has_value()) {                        // ichida biror narsa bormi?
    uint32_t x = a.value();                 // bor bo'lsa, olish
}
```

Bo'sh `optional` dan `value()` ni olishga urinsangiz, `std::bad_optional_access` exception
otiladi. Shuning uchun avval doim `has_value()` tekshiriladi.

### 8. C++: destructor va RAII

**Destructor** — klassning maxsus funksiyasi: `~FluidSimulation2D()`. Obyektning umri tugaganda
C++ uni **o'zi** chaqiradi. Umr o'zgaruvchi e'lon qilingan blokning `}` qavsida tugaydi.
Eng muhimi, exception shu blokdan o'tib ketayotganda ham destructor chaqiriladi.

Sizning `main` ingizda bu shunday ishlaydi:

```
 int main() {
     FluidSimulation2D fluidSimulation2D;   ◄── obyekt tug'ildi
     try {
         fluidSimulation2D.run();           ◄── ichida throw bo'ldi
     } catch (const std::exception& e) {
         xabar chiqarish
         return 1;  ──────┐
     }                    │
     return 0;  ──────────┤
 }  ◄─────────────────────┘ har ikki yo'lda ham shu yerda ~FluidSimulation2D() → cleanup()
```

Agar `cleanup()` ni destructor'dan chaqirsak, tozalash **har doim** bajariladi: dastur
oddiy tugaganda ham, xato bilan tugaganda ham. Bu usul **RAII** (*Resource Acquisition Is
Initialization*) deyiladi: resurs obyekt bilan birga tug'iladi va u bilan birga yo'q bo'ladi.

**O'xshatish:** xonadan chiqqanda o'chirgichni bosishni unutish mumkin. Harakat datchigi bo'lsa,
chiroq odam chiqib ketganda o'zi o'chadi. Destructor — o'sha datchik.

Lekin endi `cleanup()` **chala ishga tushgan** holatni ham to'g'ri tozalashi kerak. Masalan,
instance yaratildi, GPU topilmadi va exception otildi. Unda `device` hali `VK_NULL_HANDLE`
bo'ladi. Shuning uchun har bir resurs alohida tekshiriladi:

```cpp
if (device != VK_NULL_HANDLE) {
    vkDestroyDevice(device, nullptr);
    device = VK_NULL_HANDLE;   // ikkinchi marta chaqirilsa ham zarar qilmasin
}
```

### 9. C++: nega klassni nusxalashni taqiqlaymiz

Destructor qo'shilgach, yangi xavf paydo bo'ladi:

```cpp
FluidSimulation2D a;
FluidSimulation2D b = a;   // nusxa: b.instance == a.instance (bitta handle!)
```

Ikkala obyekt umri tugaganda ikkala destructor ham **bitta** instance'ni yo'q qiladi. Ikkinchi
`vkDestroyInstance` allaqachon o'chirilgan obyektga tegadi va dastur buziladi. Bir kalitning ikki
nusxasi bilan ikki kishi bitta kvartirani "topshirgandek" bo'ladi.

Yechim: kompilyatorga *"bu klassni nusxalab bo'lmaydi"* deymiz:

```cpp
FluidSimulation2D(const FluidSimulation2D&) = delete;              // nusxa yaratish taqiqlangan
FluidSimulation2D& operator=(const FluidSimulation2D&) = delete;   // nusxa o'zlashtirish taqiqlangan
```

Endi `b = a` yozsangiz, **kompilyatsiya xatosi** chiqadi. Xato dastur ishga tushganda emas,
build paytida topiladi. Bu eng yaxshi holat.

Bitta nozik joy: biror konstruktor qo'lda e'lon qilinsa (hatto `= delete` bilan ham), kompilyator
oddiy konstruktorni avtomatik yaratmay qo'yadi. Shuning uchun uni ham qaytarib so'raymiz:
`FluidSimulation2D() = default;`.

---

## Amaliyot

### 1-qadam: 04-darsdan qolganlarni tuzating

Sharhda batafsil yozilgan. Qisqacha:

1. `initWindow()` da `glfwCreateWindow` natijasini `nullptr` ga tekshiring (03-darsda bor edi).
2. `printVulkanVersion()` ni `void` qiling: u `false` qaytara olmaydi, xato bo'lsa `vkCheck`
   exception otadi. `if (...) return;` ni olib tashlang va funksiyani `initVulkan()` ning boshidan
   chaqiring.
3. `VK_CHECK` makrosini `namespace utils { }` blokidan tashqariga chiqaring va to'liq nom bilan
   yozing: `::utils::vkCheck(...)`.
4. `vkCreateInstance` va `utils::CreateDebugUtilsMessengerEXT` natijalarini `VK_CHECK` bilan
   tekshiring. Shunda xato kodi yo'qolmaydi.
5. `utils::isLayerAvailable` va `checkValidationLayerSupport` bir xil ishni qiladi. Bittasini qoldiring.

### 2-qadam: yangi a'zolar

`fluid_sim_2d.h` ga `#include <optional>` qo'shing. Klassning `private:` qismidagi
o'zgaruvchilarga:

```cpp
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t queueFamilyIndex = 0;
    VkQueue queue = VK_NULL_HANDLE;   // graphics + compute (06-darsda present ham)
```

va funksiyalarga:

```cpp
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice gpu);
    std::optional<uint32_t> findQueueFamily(VkPhysicalDevice gpu);
    void createLogicalDevice();
```

Parametr nomi nega `gpu`? Klassda allaqachon `physicalDevice` va `device` a'zolari bor. Parametrga
boshqa nom bersak, *"bu tekshirilayotgan nomzodmi yoki tanlab bo'linganimi?"* degan chalkashlik
bo'lmaydi.

Funksiyalar hozircha faqat e'lon qilingan, ta'rifi yo'q. Ular hali hech qayerda chaqirilmagani
uchun linker xato bermaydi (01-dars).

### 3-qadam: destructor va xavfsiz `cleanup()`

`fluid_sim_2d.h` da `public:` qismiga qo'shing:

```cpp
public:
    FluidSimulation2D() = default;
    ~FluidSimulation2D();

    // Nusxalash taqiqlangan: ikki obyekt bitta Vulkan handle'ini ikki marta yo'q qilmasin
    FluidSimulation2D(const FluidSimulation2D&) = delete;
    FluidSimulation2D& operator=(const FluidSimulation2D&) = delete;

    void run();
```

`fluid_sim_2d.cpp` da:

```cpp
FluidSimulation2D::~FluidSimulation2D() {
    cleanup();
}
```

`run()` dan `cleanup();` chaqiruvini olib tashlang: endi uni destructor chaqiradi.

`cleanup()` ni qayta yozing: har bir resursni tekshirib yo'q qiling, keyin `VK_NULL_HANDLE`
(yoki `nullptr`) qiling. Tartib — **yaratilishga aniq teskari**:

```
 yaratish:     glfwInit → window → instance → debugMessenger → device
 yo'q qilish:  device → debugMessenger → instance → window → glfwTerminate
```

Hozirgi `cleanup()` ingizda oyna birinchi bo'lib yo'q qilinyapti, holbuki u birinchi bo'lib
yaratilgan. Hozircha bu xato emas, chunki Vulkan obyektlari oynaga bog'liq emas. 06-darsda esa
surface aynan oynadan yaratiladi va oynadan oldin yo'q qilinishi kerak bo'ladi. Shuning uchun
tartibni hozirdan to'g'rilab qo'ying.

Skelet:

```cpp
void FluidSimulation2D::cleanup() {
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
    // debugMessenger: xuddi shunday (enableValidationLayers ni tekshirish shart emas:
    //                 validation o'chiq bo'lsa, u baribir VK_NULL_HANDLE bo'lib qoladi)
    // instance:       xuddi shunday
    // window:         nullptr bilan solishtiring
    glfwTerminate();   // GLFW hujjati: glfwInit dan oldin chaqirilsa ham xavfsiz
}
```

`physicalDevice` va `queue` yo'q qilinmaydi: birinchisini instance, ikkinchisini device
boshqaradi.

Build qilib, dastur avvalgidek ishlashini va yopilganda validation xabari chiqmasligini
tekshiring. `device` hali `VK_NULL_HANDLE`, shuning uchun uning bloki hozircha hech narsa qilmaydi.

### 4-qadam: kerakli queue family'ni topish

```cpp
// GRAPHICS va COMPUTE ni birga bajara oladigan birinchi queue family indeksini qaytaradi
std::optional<uint32_t> FluidSimulation2D::findQueueFamily(VkPhysicalDevice gpu) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, nullptr);

    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, families.data());

    const VkQueueFlags required = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    for (uint32_t i = 0; i < count; i++) {
        if ((families[i].queueFlags & required) == required) {
            return i;
        }
    }
    return std::nullopt;
}
```

Ikki chaqiruvli ro'yxat usuli sizga 04-darsdan tanish. Faqat bu funksiya `VkResult` qaytarmaydi,
shuning uchun `VK_CHECK` siz chaqiriladi. `return i;` — `uint32_t` avtomatik ravishda
`std::optional<uint32_t>` ga o'raladi.

### 5-qadam: GPU bizga mosmi?

```cpp
bool FluidSimulation2D::isDeviceSuitable(VkPhysicalDevice gpu) {
    // 1. Vulkan 1.3 ni qo'llaydimi?
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(gpu, &properties);
    if (properties.apiVersion < VK_API_VERSION_1_3) {
        return false;
    }

    // 2. Kerakli imkoniyatlar bormi? (Nazariya, 5-bo'lim: pNext zanjiri)
    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features13;

    vkGetPhysicalDeviceFeatures2(gpu, &features2);
    if (!features13.dynamicRendering || !features13.synchronization2) {
        return false;
    }

    // 3. Kerakli navbat bormi?
    return findQueueFamily(gpu).has_value();
}
```

`properties.apiVersion < VK_API_VERSION_1_3` — ikkita versiyani oddiy `<` bilan solishtiryapmiz.
Nega bu to'g'ri ishlaydi? 03-darsdagi bitlar rasmiga qarang (O'zingizni tekshiring, 6-savol).

### 6-qadam: GPU'ni tanlash

Endi o'zingiz yozing. Yo'l-yo'riq:

```cpp
void FluidSimulation2D::pickPhysicalDevice() {
    // 1. vkEnumeratePhysicalDevices(instance, &count, ...) — ikki chaqiruvli usul, VK_CHECK bilan.
    //    count == 0 bo'lsa: throw ("Vulkan'ni qo'llaydigan GPU topilmadi").

    // 2. Har bir gpu uchun:
    //    - vkGetPhysicalDeviceProperties bilan nomini oling
    //    - isDeviceSuitable(gpu) ni tekshiring
    //    - konsolga chiqaring: nomi, diskretmi (deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU),
    //      mosmi yoki yo'qmi
    //    - mos bo'lsa va (hali hech narsa tanlanmagan YOKI bu diskret GPU bo'lsa) → physicalDevice = gpu

    // 3. Sikldan keyin physicalDevice hali VK_NULL_HANDLE bo'lsa: throw.

    // 4. queueFamilyIndex = findQueueFamily(physicalDevice).value();
    //    Bu yerda value() xavfsiz: isDeviceSuitable family borligini tekshirib bo'lgan.

    // 5. Tanlangan GPU nomini va queueFamilyIndex ni utils::createInfoLog bilan chiqaring.
}
```

### 7-qadam: logical device va queue

```cpp
void FluidSimulation2D::createLogicalDevice() {
    // 1. Qaysi family'dan nechta navbat kerak
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = queueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    // 2. Qaysi imkoniyatlarni yoqamiz: faqat keraklilarini
    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;

    // 3. Device blankasi
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &features13;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueInfo;
    createInfo.enabledExtensionCount = 0;    // VK_KHR_swapchain 06-darsda qo'shiladi
    createInfo.pEnabledFeatures = nullptr;   // Vulkan 1.0 imkoniyatlari: hozircha kerak emas

    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    // 4. Navbat device bilan birga yaratilgan. Uning handle'ini olamiz
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
    std::cout << utils::createInfoLog("Logical device va queue yaratildi!");
}
```

| Kod | Ma'nosi |
|---|---|
| `queueCount = 1` | Shu family'dan bitta navbat kerak (family 0 da 16 ta bor) |
| `pQueuePriorities = &queuePriority` | Ustuvorliklar **massiviga** pointer. Navbat bitta, shuning uchun bitta `float` ning manzili yetadi |
| `features13.dynamicRendering = VK_TRUE` | Imkoniyatni **yoqish**. 5-qadamda faqat **so'ragan** edik |
| `createInfo.pNext = &features13` | Yoqiladigan 1.3 imkoniyatlari zanjir orqali beriladi |
| `vkGetDeviceQueue(device, family, 0, &queue)` | Family ichidagi 0-navbatning handle'i. Navbat yaratilmaydi va yo'q qilinmaydi: u device'ga tegishli |

> Eski darsliklarda (vulkan-tutorial.com) device yaratganda layer'lar ham beriladi
> (`enabledLayerCount`). Hozirgi Vulkan'da device layer'lari e'tiborga olinmaydi: layer'lar faqat
> instance'da yoqiladi. Bu maydonlarni 0 qoldiring.

### 8-qadam: hammasini ulang

`initVulkan()`:

```cpp
void FluidSimulation2D::initVulkan() {
    utils::printVulkanVersion();
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
}
```

`cleanup()` dagi `vkDestroyDevice` ni 3-qadamda yozib qo'ygansiz. Build qilib, ishga tushiring.

---

## Tekshiruv

Konsolda avvalgi xabarlardan keyin taxminan shunday qatorlar chiqadi (format sizning
`createInfoLog` ingizga bog'liq):

```
GPU: NVIDIA GeForce RTX 3060 | diskret | mos
----[ INFO ]-----Tanlangan GPU: NVIDIA GeForce RTX 3060, queue family: 0
----[ INFO ]-----Logical device va queue yaratildi!
```

Oyna avvalgidek ochiladi. Yopganda hech qanday `[ ERROR ]` yoki `[ WARNING ]` xabari chiqmasligi
kerak.

### Ataylab xato qiling

1. `cleanup()` dagi `vkDestroyDevice` blokini vaqtincha izohga oling. Yopganda 04-darsdan tanish
   `VUID-vkDestroyInstance-instance-00629` xabari chiqadi. Bu safar u `VkDevice` yo'q
   qilinmaganini aytadi. Keyin qaytaring.
2. `createLogicalDevice()` dagi `vkCreateDevice` dan keyin vaqtincha
   `throw std::runtime_error("sinov");` yozing. Dastur xato xabarini chiqarib tugashi va
   **hech qanday validation xabari chiqmasligi** kerak. Demak destructor device, messenger va
   instance'ni to'g'ri tozaladi. 04-darsdagi kod bilan bunday bo'lmasdi. Keyin olib tashlang.

### Mumkin bo'lgan xatolar

| Xato | Sabab |
|---|---|
| `use of deleted function 'FluidSimulation2D::FluidSimulation2D(const FluidSimulation2D&)'` | Qayerdadir klass nusxalanyapti. Odatda funksiyaga `&` siz uzatilganda bo'ladi |
| `no matching function for call to 'FluidSimulation2D::FluidSimulation2D()'` | `FluidSimulation2D() = default;` yozilmagan (Nazariya, 9-bo'lim) |
| `'vkCheck' was not declared in this scope` | `VK_CHECK` makrosi hali `::utils::vkCheck` ga o'zgartirilmagan (1-qadam) |
| `vkCreateDevice xato qaytardi: -8` | `VK_ERROR_FEATURE_NOT_PRESENT`: GPU qo'llamaydigan imkoniyat yoqilmoqda |

---

## Mashqlar

1. **GPU pasporti.** Tanlangan GPU haqida ko'proq ma'lumot chiqaring: `deviceType` ni matnga
   aylantiruvchi funksiya yozing (`switch` bilan: DISCRETE, INTEGRATED, VIRTUAL, CPU, OTHER),
   `driverVersion`, va `properties.limits` ichidan `maxComputeWorkGroupInvocations` va
   `maxComputeWorkGroupSize[0..2]`. Oxirgi ikkitasi 17-darsda compute shader yozishda kerak bo'ladi.

2. **Barcha oilalar.** Har bir queue family'ni chiqaring: indeksi, `queueCount` va bayroqlari
   (`GRAPHICS`, `COMPUTE`, `TRANSFER` so'zlari bilan). Natijani Nazariyadagi jadval bilan
   solishtiring. Nima uchun faqat TRANSFER qila oladigan family'lar mavjud deb o'ylaysiz?

3. **Yana bitta imkoniyat.** `isDeviceSuitable` da `features13.shaderDemoteToHelperInvocation` ni
   ham o'qib, konsolga chiqaring (yoqmang!). Bu imkoniyat 08-darsda shader'dagi `discard`
   buyrug'i uchun kerak bo'ladi va o'sha yerda nega kerakligini ko'ramiz.

4. **Mos GPU yo'q bo'lsa.** Vaqtincha `isDeviceSuitable` da shartni bajarib bo'lmaydigan qiling
   (masalan, `apiVersion` ni `VK_MAKE_API_VERSION(0, 2, 0, 0)` bilan solishtiring). Dastur qanday
   xabar berdi? Destructor hammasini tozaladimi? Keyin qaytaring.

5. **Nusxa olishga urinib ko'ring.** `main` da `FluidSimulation2D copy = fluidSimulation2D;` deb
   yozing. Xato qaysi bosqichda chiqdi va nima deyapti? Keyin o'chiring.

---

## O'zingizni tekshiring

1. Physical device va logical device qanday farq qiladi? Nega physical device yo'q qilinmaydi?
2. Queue family nima? Nega bizga GRAPHICS va COMPUTE ni birga bajaradigan family kerak?
3. `(flags & required) == required` va `(flags & required) != 0` qanday farq qiladi?
4. Nega queue family indeksini `std::optional` da qaytaramiz, `-1` yoki `0` bilan emas?
5. Imkoniyatni "so'rash" va "yoqish" nega ikki alohida qadam?
6. Nega Vulkan versiyalarini oddiy `<` bilan solishtirsa bo'ladi? (03-darsdagi bitlar rasmini eslang.)
7. Destructor qachon chaqiriladi? Exception paytida-chi?
8. Nega `cleanup()` har bir handle'ni `VK_NULL_HANDLE` ga tekshiradi?
9. Nega klassni nusxalashni taqiqladik? Taqiqlamaganimizda nima bo'lardi?
10. Nega device instance'dan oldin yo'q qilinadi?

---

**Oldingi dars:** [04 — Instance va validation layer](04_instance_va_validation_layer.md) ·
**Keyingi dars:** 06 — Surface va swapchain *(05-dars tugagach yoziladi)*
