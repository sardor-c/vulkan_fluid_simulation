# 06-dars. Surface va swapchain

## Maqsad

Dars oxirida:
- Oyna bilan Vulkan'ni **surface** orqali bog'laysiz
- Queue family'ning **present** (ekranga chiqarish) qila olishini tekshirasiz
- Birinchi **device extension** — `VK_KHR_swapchain` ni yoqasiz
- **Swapchain** yaratasiz: ekranga navbat bilan chiqariladigan rasmlar to'plami. Uning formati,
  present mode'i, o'lchami va rasmlar sonini o'zingiz tanlaysiz
- Swapchain rasmlari uchun **image view** lar yaratasiz
- C++ dan: parametrni `const&` bilan uzatish, struct qaytaruvchi funksiya, `std::clamp`,
  `std::numeric_limits`

Bu darsda ham ekranda hali hech narsa chizilmaydi. Lekin 07-darsda oynani rangga bo'yash uchun
kerak bo'lgan hamma narsa tayyor bo'ladi.

> Bu dars ikkala kompyuterda ham sinalishi kerak. Natijalar (rasm formatlari, present mode'lar,
> o'lcham) Windows va Mac'da **har xil** chiqadi. Darsda ikkalasi ham ko'rsatilgan.

---

## Nazariya

### 1. Surface: oynaning Vulkan'dagi "yuzasi"

Vulkan o'zi oyna haqida hech narsa bilmaydi. U faqat GPU bilan ishlaydi: rasm chizadi, hisoblaydi.
Rasmni ekrandagi oynaga chiqarish esa operatsion tizimning ishi, va har bir OS'da bu boshqacha:
Windows'da Win32, macOS'da Metal (`CAMetalLayer`), Linux'da X11 yoki Wayland.

Shuning uchun Vulkan'da oyna bilan ishlash alohida kengaytmalarga ajratilgan. Bu qism **WSI**
(*Window System Integration*, "oyna tizimi bilan bog'lanish") deyiladi:

```
 Sizning dastur
      │
      ▼
 VkSurfaceKHR  ◄── "oynaning Vulkan'dagi vakili". Platformaga bog'liq emas
      │
      ├── Windows: VK_KHR_win32_surface  ── HWND (Win32 oyna)
      └── macOS:   VK_EXT_metal_surface  ── CAMetalLayer (Metal qatlami)
```

Surface yaratish platformaga xos kod talab qiladi. Lekin buni GLFW biz uchun qiladi:
`glfwCreateWindowSurface` bitta funksiya, u ichida kerakli platforma kodini chaqiradi. 04-darsda
`glfwGetRequiredInstanceExtensions` qaytargan ro'yxat aynan shu uchun kerak edi:

| Kompyuter | GLFW so'ragan instance kengaytmalari |
|---|---|
| Windows | `VK_KHR_surface`, `VK_KHR_win32_surface` |
| Mac | `VK_KHR_surface`, `VK_EXT_metal_surface` |

**O'xshatish:** surface — rassom bilan ko'rgazma zali orasidagi ramka. Rassom (GPU) qaysi
devorga (Windows yoki Mac) osilishini bilmaydi. U faqat ramka o'lchamiga mos rasm chizadi.

Surface **instance**'dan yaratiladi va instance bilan yo'q qilinadi (`vkDestroySurfaceKHR(instance, ...)`).
U device'ga bog'liq emas. Shuning uchun uni **GPU tanlashdan oldin** yaratamiz: 2-bo'limda
nega kerakligini ko'rasiz.

### 2. Present: "bu navbat ekranga chiqara oladimi?"

05-darsda GRAPHICS va COMPUTE ni birga bajaradigan family'ni qidirdik va *"present'ni 06-darsda
qo'shamiz"* degan edik. **Present** — tayyor rasmni oynaga chiqarish. Hamma family ham buni qila
olmaydi, va javob **qaysi surface** ekaniga ham bog'liq: masalan, bitta GPU bir monitorga
chiqara oladi, boshqasiga esa yo'q. Shuning uchun so'rovga surface ham beriladi:

```cpp
VkBool32 presentSupport = VK_FALSE;
vkGetPhysicalDeviceSurfaceSupportKHR(gpu, familyIndex, surface, &presentSupport);
```

Bu so'rov uchun surface allaqachon bo'lishi kerak. Shuning uchun tartib quyidagicha:

```
 createInstance → setupDebugMessenger → createSurface → pickPhysicalDevice → ...
                                         ▲                   │
                                         └── findQueueFamily surface'ni ishlatadi
```

| Kompyuter | Present qila oladigan family'lar |
|---|---|
| Windows (RTX 3060) | 0 (GRAPHICS bor family). Boshqalarida odatda yo'q |
| Mac (M1 Pro) | 4 ta family'ning hammasi (har birida bitta navbat, hammasi GRAPHICS + COMPUTE + TRANSFER) |

Ko'p GPU'da graphics family present ham qiladi. Lekin spetsifikatsiya buni **kafolatlamaydi**.
Kafolat bo'lmagani uchun tekshiramiz. Bizga GRAPHICS + COMPUTE + PRESENT ning hammasini bajara
oladigan **bitta** family kerak: unda bitta navbat bilan ishlaymiz va sinxronizatsiya oddiy bo'ladi.

### 3. Device extension: `VK_KHR_swapchain`

04-darsda **instance** kengaytmalarini yoqdik. Kengaytmalarning ikkinchi turi ham bor:

| | Instance extension | Device extension |
|---|---|---|
| Nimaga tegishli | Loader va umuman Vulkan | Aniq bir GPU drayveri |
| Qayerda yoqiladi | `VkInstanceCreateInfo` | `VkDeviceCreateInfo` |
| Ro'yxat | `vkEnumerateInstanceExtensionProperties` | `vkEnumerateDeviceExtensionProperties(gpu, ...)` |
| Misollar | `VK_KHR_surface`, `VK_EXT_debug_utils`, `VK_KHR_portability_enumeration` | `VK_KHR_swapchain`, `VK_KHR_portability_subset` |

Swapchain'ni GPU yaratadi va boshqaradi, shuning uchun u **device** kengaytmasi. Siz 05-darsda
Mac uchun device kengaytmalari ro'yxatini olib, `VK_KHR_portability_subset` ni yoqqansiz. Endi
o'sha ro'yxatga yana bitta majburiy kengaytma qo'shiladi.

Kompyuter serverlarida ishlaydigan GPU'larda oyna umuman bo'lmasligi mumkin. Shuning uchun swapchain
Vulkan yadrosiga kirmaydi. U kengaytma, va uni GPU tanlashda **tekshiramiz**.

### 4. Swapchain: rasmlar konveyeri

Ekranga chiqayotgan rasm ustiga to'g'ridan-to'g'ri chizish mumkin emas: monitor rasmni
yuqoridan pastga qator-qator o'qiydi. Siz shu payt chizsangiz, ekranning yarmida eski kadr,
yarmida yangisi ko'rinadi. Bu **tearing** ("yirtilish") deyiladi.

Yechim: bir nechta rasm. Monitor bittasini ko'rsatib turganda GPU boshqasiga chizadi, keyin
ular almashadi. Mana shu rasmlar to'plami **swapchain** (*swap* — almashtirish, *chain* — zanjir):

```
            ┌──────────────── swapchain (3 ta image) ────────────────┐
            │                                                         │
            │   image 0            image 1             image 2        │
            │  [ ekranda ]   [ GPU chizyapti ]   [ navbatda kutyapti ] │
            │                                                         │
            └─────────────────────────────────────────────────────────┘
   07-darsda har kadr:
     1. acquire  — "menga bo'sh image ber"        (vkAcquireNextImageKHR)
     2. chizish  — o'sha image'ga buyruqlar yozish
     3. present  — "tayyor, ekranga chiqar"       (vkQueuePresentKHR)
```

**O'xshatish:** restoranda likopchalar. Ofitsiant bitta likopchani zalga olib chiqadi, oshpaz
shu payt ikkinchisiga ovqat soladi, uchinchisi yuvilib, navbatda turadi. Bitta likopcha bo'lsa,
oshpaz har safar mijoz ovqatni tugatishini kutardi.

Bu darsda swapchain'ni faqat **yaratamiz**. Acquire va present 07-darsda bo'ladi.

### 5. Swapchain sozlamalari: to'rtta savol

Swapchain yaratishdan oldin surface'dan uchta narsani so'raymiz. 04-darsdagi ikki chaqiruvli
usul bu yerda ham ishlaydi:

| So'rov | Funksiya | Javob |
|---|---|---|
| Imkoniyatlar | `vkGetPhysicalDeviceSurfaceCapabilitiesKHR` | Rasmlar soni chegarasi, o'lcham, transform |
| Formatlar | `vkGetPhysicalDeviceSurfaceFormatsKHR` | Qanday piksel formatlari va rang fazolari bor |
| Present mode'lar | `vkGetPhysicalDeviceSurfacePresentModesKHR` | Rasmlar ekranga qanday tartibda chiqadi |

Javoblarga qarab to'rtta qaror qabul qilamiz.

#### a) Format: piksel qanday saqlanadi

`VkSurfaceFormatKHR` ikki qismdan iborat:

| Maydon | Ma'nosi | Biz tanlaymiz |
|---|---|---|
| `format` | Bitta piksel xotirada qanday turadi | `VK_FORMAT_B8G8R8A8_UNORM` |
| `colorSpace` | Ranglar qanday talqin qilinadi | `VK_COLOR_SPACE_SRGB_NONLINEAR_KHR` |

`B8G8R8A8_UNORM` ni bo'laklab o'qiymiz:
- `B8 G8 R8 A8` — har bir pikselda 4 ta kanal (ko'k, yashil, qizil, shaffoflik), har biri 8 bit.
  Tartib B-G-R: Windows va macOS ekranlari tarixan shu tartibni ishlatadi
- `UNORM` — *unsigned normalized*: 0..255 butun son shader'da 0.0..1.0 kasr son bo'lib ko'rinadi

Xuddi shu ro'yxatda `B8G8R8A8_SRGB` ham bor. Farqi: `SRGB` formatga yozganda GPU rangni
avtomatik "gamma" konvertatsiyasidan o'tkazadi. Biz ranglarni Sebastian Lague loyihasidagidek
tayyor ekran ranglari sifatida beramiz, shuning uchun konvertatsiya kerak emas: `UNORM`. Bu mavzuga
24-darsda (chiroyli rendering) qaytamiz. Kerakli format topilmasa, ro'yxatdagi birinchisini olamiz.

Sizdagi ro'yxatlar (`vulkaninfo` dan):

| Kompyuter | Formatlar soni | Ro'yxat boshi |
|---|---|---|
| Windows (RTX 3060) | odatda 3–5 ta (sizda boshqacha bo'lishi mumkin) | `B8G8R8A8_UNORM`, `B8G8R8A8_SRGB`, `A2B10G10R10_UNORM` |
| Mac (M1 Pro) | 60 ta | `B8G8R8A8_UNORM`, `B8G8R8A8_SRGB`, `R16G16B16A16_SFLOAT`, ... |

Mac'da formatlar ko'p, chunki Apple ekranlari ko'p rang fazolarini qo'llaydi (Display P3,
DCI-P3, BT.709...). Bizga bularning hech biri kerak emas.

#### b) Present mode: rasmlar ekranga qachon chiqadi

| Mode | Qanday ishlaydi | Tearing | Kafolatlanganmi |
|---|---|---|---|
| `FIFO` | Navbat: monitor yangilanganda navbatdagi birinchi rasm chiqadi. Bu **vsync** | Yo'q | **Ha, har doim bor** |
| `FIFO_RELAXED` | FIFO, lekin kech qolgan kadr darhol chiqadi | Ba'zan | Yo'q |
| `MAILBOX` | Navbatda faqat eng yangi rasm turadi, eskisi tashlanadi | Yo'q | Yo'q |
| `IMMEDIATE` | Rasm darhol chiqadi, monitorni kutmaydi | **Ha** | Yo'q |

*FIFO* — *First In, First Out* ("birinchi kelgan birinchi ketadi"), oddiy navbat.

| Kompyuter | Mavjud mode'lar |
|---|---|
| Windows (RTX 3060) | odatda `FIFO`, `FIFO_RELAXED`, `MAILBOX`, `IMMEDIATE` (o'zingiz tekshirasiz) |
| Mac (M1 Pro) | faqat `FIFO` va `IMMEDIATE` |

Biz `FIFO` ni tanlaymiz: u **hamma joyda bor** va kadrlar soni monitor chastotasiga teng bo'ladi
(Windows'da 165 Hz, MacBook'da ProMotion ekrani 120 Hz gacha). Simulatsiya uchun bu yetarli.
Ammo kodni "vsync o'chirilsa nima bo'ladi?" degan savolga ham tayyor qilib yozamiz (7-qadam).

#### c) Extent: rasm o'lchami pikselda

Bu yerda **Retina** tuzog'i bor. GLFW'da oyna o'lchami ikki xil o'lchanadi:

| | Funksiya | Windows | Mac (Retina) |
|---|---|---|---|
| **Screen coordinates** (point) | `glfwGetWindowSize` | 1280 × 720 | 1280 × 720 |
| **Piksel** (framebuffer) | `glfwGetFramebufferSize` | 1280 × 720 | **2560 × 1440** |

Retina ekranda bitta "point" 2 × 2 = 4 ta jismoniy pikseldan iborat. Shuning uchun matn tiniq
ko'rinadi. Swapchain rasmlari **piksel**da o'lchanadi. Agar `WIDTH`, `HEIGHT` (1280 × 720) ni
berib yuborsangiz, Mac'da rasm 4 marta kam pikselli bo'lib, cho'zilib, xira ko'rinadi.

Surface capabilities ichida `currentExtent` bor: surface'ning hozirgi o'lchami. Ko'p
platformalarda u to'g'ridan-to'g'ri javob bo'ladi. Ba'zi platformalarda (masalan, Wayland)
esa u **maxsus qiymat** `0xFFFFFFFF` ga teng bo'ladi. Bu *"o'lchamni o'zing tanla"* degani.
Unda o'lchamni `glfwGetFramebufferSize` dan olamiz va `minImageExtent..maxImageExtent` oralig'iga
siqamiz.

#### d) Rasmlar soni

Capabilities ichida `minImageCount` va `maxImageCount` bor. `maxImageCount == 0` bo'lsa, "yuqori
chegara yo'q" degani. Biz `minImageCount + 1` ni so'raymiz: drayver bitta rasmni ichki ishlar
uchun ushlab tursa, bizni kutib qolmasligi uchun:

| Kompyuter | min | max | Biz so'raymiz |
|---|---|---|---|
| Windows (RTX 3060) | odatda 2 | odatda 8 | 3 |
| Mac (M1 Pro) | 2 | 3 | 3 |

Bu **minimal** son: drayver ko'proq rasm yaratishga haqli. Shuning uchun swapchain yaratilgach,
rasmlar sonini qaytadan so'raymiz.

### 6. Image va image view

| | `VkImage` | `VkImageView` |
|---|---|---|
| Nima | Xotiradagi rasmning o'zi: piksellar | Shu rasmga "qanday qarash kerak" degan tavsif |
| Kim yaratadi | Swapchain rasmlarini swapchain o'zi yaratadi | Biz, `vkCreateImageView` bilan |
| Kim yo'q qiladi | Swapchain (biz **tegmaymiz**) | Biz, `vkDestroyImageView` bilan |

Vulkan rasmga hech qachon to'g'ridan-to'g'ri murojaat qilmaydi. Chizishda ham, shader'da o'qishda
ham doim **view** orqali ishlaydi. View aytadi:
- rasmni qanday tur sifatida ko'rish: 2D, 3D, kub
- qaysi format bilan talqin qilish
- rasmning **qaysi qismi**: qaysi mip-level'lar (kichraytirilgan nusxalar), qaysi qatlamlar
  (layer'lar), rang yoki chuqurlik (depth)

**O'xshatish:** image — katta xarita. View — undan qirqib olingan, ramkaga solingan bo'lak:
*"faqat Toshkent viloyati, rangli holda"*. Bitta xaritaga bir nechta ramka qilish mumkin. Bizda
esa har bir swapchain rasmiga bitta oddiy view kerak: butun rasm, 2D, rang.

07-darsda dynamic rendering'ga *"shu view'ga chiz"* deymiz.

### 7. C++: parametrni `const&` bilan uzatish

Funksiyaga `std::vector` ni oddiygina uzatsangiz, u **nusxalanadi**:

```cpp
void print(std::vector<VkSurfaceFormatKHR> formats);          // har chaqiruvda butun ro'yxat nusxasi
void print(const std::vector<VkSurfaceFormatKHR>& formats);   // nusxa yo'q: asl ro'yxatga ishora
```

| Yozuv | Nusxa olinadimi | Funksiya ichida o'zgartirsa bo'ladimi |
|---|---|---|
| `T x` | Ha | Ha, lekin faqat nusxa o'zgaradi |
| `T& x` | Yo'q | Ha, asl o'zgaradi |
| `const T& x` | Yo'q | Yo'q |

Qoida: `int`, `float`, `VkFormat`, handle'lar kabi kichik tiplarni oddiy (nusxa bilan)
uzating. `std::vector`, `std::string`, katta struct'larni esa **o'qish uchun** `const&` bilan
uzating. `const` kompilyatorga *"bu funksiya ma'lumotni o'zgartirmaydi"* deydi va tasodifiy
o'zgartirishdan himoya qiladi.

### 8. C++: struct qaytaradigan funksiya, `std::clamp`, `std::numeric_limits`

Surface haqidagi uchta javobni bitta struct'ga yig'ib qaytaramiz:

```cpp
struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapchainSupport support = querySwapchainSupport(gpu);   // uchala javob birga keladi
```

Struct'ni qiymat bilan qaytarish arzon: kompilyator nusxa olmasdan, natijani to'g'ridan-to'g'ri
qabul qiluvchi o'zgaruvchiga quradi.

```cpp
#include <algorithm>   // std::clamp
#include <limits>      // std::numeric_limits

std::clamp(x, lo, hi);                      // x ni [lo, hi] oralig'iga siqadi
std::clamp(5000u, 1u, 4096u)   == 4096      // juda katta → yuqori chegara
std::clamp(0u, 1u, 4096u)      == 1         // juda kichik → quyi chegara

std::numeric_limits<uint32_t>::max()        // uint32_t ning eng katta qiymati: 0xFFFFFFFF
```

`0xFFFFFFFF` ni qo'lda yozish ham mumkin. Lekin `numeric_limits` nima ekanini nomi bilan
aytadi: "uint32_t ning maksimumi".

---

## Amaliyot

### 1-qadam: 05-darsdan qolganlarni tuzating

Avval 05-dars kodidagi kamchiliklar. Ko'pi kichik, lekin 1-chisi mantiqiy xato.

1. **Diskret GPU afzal ko'rilmayapti.** `pickPhysicalDevice` dagi shart:

   ```cpp
   if (isDeviceSuitable(item) && (physicalDevice == VK_NULL_HANDLE))
   ```

   Bu *"birinchi mos GPU"* ni tanlaydi. Noutbukda ro'yxat boshida integrated GPU tursa, diskret
   GPU hech qachon tanlanmaydi. Darsdagi shart: mos bo'lsa **va** (hali hech narsa tanlanmagan
   **yoki** bu diskret GPU bo'lsa). Sizning ikki kompyuteringizda bitta GPU bor, shuning uchun
   xato ko'rinmaydi. Lekin kod uni boshqa kompyuterda ko'rsatib qo'yadi. Tuzating. Shu bilan
   birga `GPU: ...` qatoriga `mos` / `mos emas` so'zini qo'shing.

2. **`cleanup` da `debugMessenger = VK_NULL_HANDLE;` yo'q.** Boshqa hamma handle null qilingan,
   faqat shu qolib ketgan. Hozir `cleanup` bir marta chaqirilgani uchun zarari yo'q. Lekin 05-darsda
   *"ikkinchi marta chaqirilsa ham zarar qilmasin"* degan qoidaga kelishgan edik.

3. **`printQueueFamilies` ro'yxatni nusxa bilan oladi.** Parametrni
   `const std::vector<VkQueueFamilyProperties>& families` qiling (Nazariya, 7-bo'lim). Bundan
   tashqari, u `findQueueFamily` ichidan chaqiriladi. `findQueueFamily` esa ikki marta ishlaydi:
   `isDeviceSuitable` da va `pickPhysicalDevice` oxirida. Shuning uchun jadval konsolga ikki marta
   chiqadi. Bugun `findQueueFamily` ga present tekshiruvi qo'shiladi. Chiqarishni undan olib,
   `pickPhysicalDevice` ning GPU sikliga ko'chiring: har bir GPU uchun bir marta chiqsin.

4. **Ishlatilmaydigan kod.** `utils::isLayerAvailable` hech qayerda chaqirilmaydi:
   `checkValidationLayerSupport` xuddi shu ishni qiladi. 05-darsning 1-qadamida bittasini
   qoldirishga kelishgan edik. `main.cpp` dagi `//FluidSimulation2D copy = ...` izohini ham
   o'chiring: mashq tugadi.

5. *(ixtiyoriy)* 05-darsning 1-mashqi (GPU pasporti: `deviceType` matnga, `driverVersion`,
   `maxComputeWorkGroupInvocations`, `maxComputeWorkGroupSize`) bajarilmagan. Uni 17-darsdan oldin
   albatta bajaring. Mac'dagi natija ayniqsa qiziq.

### 2-qadam: yangi a'zolar va e'lonlar

`fluid_sim_2d.h` ga, klassdan **oldin**, struct qo'shing:

```cpp
// Surface haqidagi uchta javob: swapchain'ni shularga qarab sozlaymiz
struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
```

Klassning `private:` o'zgaruvchilariga (`window`, `instance`, `debugMessenger` dan keyin):

```cpp
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    // ... physicalDevice, device, queueFamilyIndex, queue ...
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent{};
    std::vector<VkImage> swapchainImages;           // swapchain'niki: yo'q qilmaymiz
    std::vector<VkImageView> swapchainImageViews;   // bizniki: yo'q qilamiz
```

Funksiyalarga:

```cpp
    void createSurface();

    bool checkDeviceExtensionSupport(VkPhysicalDevice gpu);
    SwapchainSupport querySwapchainSupport(VkPhysicalDevice gpu);

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void createSwapchain();
    void createImageViews();
```

`fluid_sim_2d.cpp` dagi anonim namespace'ga (`validationLayers` yoniga):

```cpp
    // GPU'da albatta bo'lishi kerak bo'lgan device kengaytmalari
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // true: FIFO (monitor chastotasida). false: imkon qadar tezroq (7-qadam)
    constexpr bool enableVsync = true;
```

Va `.cpp` ning boshiga `#include <algorithm>` va `#include <limits>`.

### 3-qadam: surface

```cpp
void FluidSimulation2D::createSurface() {
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    std::cout << utils::createInfoLog("Surface yaratildi!");
}
```

Bor-yo'g'i bitta qator. GLFW platformani o'zi aniqlaydi va `vkCreateWin32SurfaceKHR` yoki
`vkCreateMetalSurfaceEXT` ni chaqiradi. `glfwCreateWindowSurface` `VkResult` qaytaradi, shuning
uchun `VK_CHECK` ishlaydi. Bu funksiya `GLFW_INCLUDE_VULKAN` (03-dars) tufayli e'lon qilingan.

`initVulkan()` da `setupDebugMessenger();` dan keyin, `pickPhysicalDevice();` dan **oldin**
chaqiring (Nazariya, 2-bo'lim).

`cleanup()` ga, `device` blokidan **keyin**, `debugMessenger` blokidan **oldin** (surface
messenger'dan keyin yaratildi, demak undan oldin yo'q qilinadi):

```cpp
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);   // instance'ning funksiyasi
        surface = VK_NULL_HANDLE;
    }
```

Build qilib, `Surface yaratildi!` chiqishini va yopganda validation xabari yo'qligini tekshiring.

### 4-qadam: present support

`findQueueFamily` dagi siklni o'zgartiring: family endi uchta shartni bajarishi kerak:

```cpp
    for (uint32_t i = 0; i < count; i++) {
        bool hasGraphicsCompute = (families[i].queueFlags & required) == required;

        VkBool32 presentSupport = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport));

        if (hasGraphicsCompute && presentSupport) {
            return i;
        }
    }
```

Funksiya ustiga izoh yozing: *"GRAPHICS, COMPUTE va PRESENT ni birga bajara oladigan birinchi
family"*. Endi bitta navbat hamma ishni qiladi: chizish, hisob-kitob va ekranga chiqarish.

### 5-qadam: device kengaytmalari

**a) Tekshirish.** `checkDeviceExtensionSupport(gpu)` ni o'zingiz yozing. U
`checkValidationLayerSupport` ning deyarli nusxasi:

```cpp
bool FluidSimulation2D::checkDeviceExtensionSupport(VkPhysicalDevice gpu) {
    // 1. vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, ...) — ikki chaqiruv, VK_CHECK bilan
    // 2. deviceExtensions dagi har bir nom uchun: ro'yxatda bormi? (std::strcmp)
    // 3. Bittasi topilmasa → false. Hammasi topilsa → true
}
```

**b) Yoqish.** `createLogicalDevice` da hozir `enabledExtensions` bo'sh ro'yxatdan boshlanadi va
unga faqat portability qo'shiladi. Endi u majburiy kengaytmalar bilan boshlansin:

```cpp
    std::vector<const char*> enabledExtensions = deviceExtensions;   // nusxa: VK_KHR_swapchain
    // ... sizning portability_subset siklingiz o'zgarishsiz ...
```

Bu yerda nusxa olish **ataylab**: `deviceExtensions` ning o'zi `const`, biz esa unga Mac'da yana
bitta nom qo'shamiz.

### 6-qadam: surface haqida so'rash

Yangi tushuncha, shuning uchun to'liq kod:

```cpp
// Surface bu GPU bilan nimalarni qila olishini so'raydi: imkoniyatlar, formatlar, present mode'lar
SwapchainSupport FluidSimulation2D::querySwapchainSupport(VkPhysicalDevice gpu) {
    SwapchainSupport support;

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &support.capabilities));

    uint32_t formatCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr));
    support.formats.resize(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, support.formats.data()));

    uint32_t modeCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &modeCount, nullptr));
    support.presentModes.resize(modeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &modeCount, support.presentModes.data()));

    return support;
}
```

| Kod | Ma'nosi |
|---|---|
| `support.formats.resize(formatCount)` | Ro'yxatni kerakli uzunlikka keltirish. Avval `std::vector<T> v(count)` bilan qilgan ishimiz, faqat vektor allaqachon mavjud |
| capabilities uchun bitta chaqiruv | Bu ro'yxat emas, bitta struct: ikki chaqiruv kerak emas |

`isDeviceSuitable` oxirini kengaytiring. Endi tartib:

```cpp
    // 3. Kerakli navbat bormi? (graphics + compute + present)
    if (!findQueueFamily(gpu).has_value()) {
        return false;
    }

    // 4. VK_KHR_swapchain bormi?
    if (!checkDeviceExtensionSupport(gpu)) {
        return false;
    }

    // 5. Surface bilan kamida bitta format va bitta present mode bormi?
    SwapchainSupport support = querySwapchainSupport(gpu);
    return !support.formats.empty() && !support.presentModes.empty();
```

5-shart 4-dan **keyin** turibdi: swapchain kengaytmasi yo'q GPU'dan swapchain haqida so'rash
ma'nosiz.

### 7-qadam: uchta tanlov

**a) Format** (to'liq kod):

```cpp
VkSurfaceFormatKHR FluidSimulation2D::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return formats[0];   // isDeviceSuitable ro'yxat bo'sh emasligini tekshirgan
}
```

**b) Present mode** — o'zingiz yozing:

```cpp
VkPresentModeKHR FluidSimulation2D::choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
    // 1. enableVsync true bo'lsa → VK_PRESENT_MODE_FIFO_KHR (u har doim bor, ro'yxatni tekshirish shart emas)
    // 2. Aks holda: ro'yxatda MAILBOX bo'lsa → MAILBOX
    //               bo'lmasa, IMMEDIATE bo'lsa → IMMEDIATE
    //               u ham bo'lmasa → FIFO
}
```

Mac'da `enableVsync = false` qilsangiz, qaysi mode tanlanadi? Windows'da-chi? (Nazariya, 5b
jadvali.) Hozircha `true` qoldiring: farqni 07-darsda, kadrlar ekranga chiqa boshlaganda ko'ramiz.

**c) Extent** (to'liq kod, Retina uchun muhim):

```cpp
VkExtent2D FluidSimulation2D::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    // Odatiy holat: surface o'z o'lchamini aniq aytadi
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    // Maxsus holat (0xFFFFFFFF): o'lchamni o'zimiz tanlaymiz. PIKSELDA, point'da emas!
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D extent{};
    extent.width = std::clamp(static_cast<uint32_t>(width),
                              capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);
    extent.height = std::clamp(static_cast<uint32_t>(height),
                               capabilities.minImageExtent.height,
                               capabilities.maxImageExtent.height);
    return extent;
}
```

Windows va macOS'da odatda birinchi `return` ishlaydi. Ikkinchi yo'l Linux (Wayland) kabi
platformalar uchun. Lekin u ham to'g'ri bo'lishi kerak: u yerda `WIDTH`/`HEIGHT` emas,
`glfwGetFramebufferSize` ishlatilganiga e'tibor bering.

### 8-qadam: swapchain

```cpp
void FluidSimulation2D::createSwapchain() {
    SwapchainSupport support = querySwapchainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(support.formats);
    VkPresentModeKHR presentMode = choosePresentMode(support.presentModes);
    VkExtent2D extent = chooseExtent(support.capabilities);

    // Minimaldan bitta ko'p, lekin maksimumdan oshmasin (0 = chegara yo'q)
    uint32_t imageCount = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
        imageCount = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain));

    // Drayver imageCount dan ko'proq yaratgan bo'lishi mumkin: haqiqiy sonini so'raymiz
    uint32_t count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr));
    swapchainImages.resize(count);
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &count, swapchainImages.data()));

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;

    std::cout << utils::createInfoLog(
        "Swapchain yaratildi: " + std::to_string(extent.width) + "x" + std::to_string(extent.height) +
        ", " + std::to_string(count) + " ta image");
}
```

| Maydon | Ma'nosi |
|---|---|
| `minImageCount` | Kamida nechta rasm (5d) |
| `imageFormat`, `imageColorSpace` | Tanlangan format (5a). Ikkalasi ham beriladi |
| `imageExtent` | Pikseldagi o'lcham (5c) |
| `imageArrayLayers = 1` | Har bir rasmda nechta qatlam. Stereo (VR) uchun 2, bizga 1 |
| `imageUsage = COLOR_ATTACHMENT` | Rasm nima uchun ishlatiladi: unga **rang chizamiz** (07-dars) |
| `imageSharingMode = EXCLUSIVE` | Rasmni bir vaqtda faqat bitta queue family ishlatadi. Bizda bitta family, shuning uchun bu oddiy va eng tez variant. Graphics va present alohida family'larda bo'lganda `CONCURRENT` kerak bo'lardi. Bitta family tanlaganimizning yana bir foydasi |
| `preTransform = currentTransform` | Rasmni burish/aylantirish kerak emas, hozirgi holat qoldirilsin (telefonlarda ekran burilganda kerak) |
| `compositeAlpha = OPAQUE` | Oyna shaffof emas: rasmdagi alfa kanal OS'ga ta'sir qilmaydi |
| `presentMode` | 5b |
| `clipped = VK_TRUE` | Boshqa oyna bilan yopilgan piksellar chizilmasa ham bo'ladi |
| `oldSwapchain` | Swapchain qayta yaratilganda eskisi beriladi (07-dars). Hozir yo'q |

`swapchainImages` dagi image'larni biz yaratmadik, shuning uchun ularni **yo'q qilmaymiz**: ular
swapchain bilan birga yo'q bo'ladi. `physicalDevice` va `queue` bilan bir xil holat (05-dars).

> **macOS:** `B8G8R8A8_UNORM` va `FIFO` MoltenVK'da bor, `OPAQUE` ham qo'llanadi (`vulkaninfo`
> da `supportedCompositeAlpha` ga qarang). Extent esa **2560 × 1440** chiqishi kerak (Retina).
> Agar 1280 × 720 chiqsa, Claude'ga ayting.

### 9-qadam: image view'lar

```cpp
void FluidSimulation2D::createImageViews() {
    swapchainImageViews.resize(swapchainImages.size(), VK_NULL_HANDLE);

    for (size_t i = 0; i < swapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchainImageFormat;
        // components: {} bilan 0 = VK_COMPONENT_SWIZZLE_IDENTITY, ya'ni kanallar o'z joyida

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]));
    }
}
```

| Kod | Ma'nosi |
|---|---|
| `resize(n, VK_NULL_HANDLE)` | Avval hammasini null bilan to'ldiramiz. Sikl o'rtasida `throw` bo'lsa, `cleanup` null'larni tanib oladi |
| `viewType = 2D` | Oddiy tekis rasm |
| `aspectMask = COLOR` | Rasmning rang qismi (chuqurlik yoki stencil emas) |
| `baseMipLevel = 0, levelCount = 1` | Faqat asl o'lcham. Swapchain rasmlarida kichraytirilgan nusxalar yo'q |
| `baseArrayLayer = 0, layerCount = 1` | Bitta qatlam (`imageArrayLayers = 1` edi) |

### 10-qadam: hammasini ulang

`initVulkan()`:

```cpp
void FluidSimulation2D::initVulkan() {
    utils::printVulkanVersion();
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
}
```

`cleanup()` tartibi, yaratilishga aniq teskari:

```
 yaratish:     glfwInit → window → instance → messenger → surface → device → swapchain → image view'lar
 yo'q qilish:  image view'lar → swapchain → device → surface → messenger → instance → window → glfwTerminate
```

`device` blokidan **oldin** qo'shing:

```cpp
    for (VkImageView view : swapchainImageViews) {
        vkDestroyImageView(device, view, nullptr);   // VK_NULL_HANDLE bo'lsa, hech narsa qilmaydi
    }
    swapchainImageViews.clear();
    swapchainImages.clear();   // faqat ro'yxatni tozalaymiz: image'lar swapchain bilan birga ketadi

    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }
```

Vulkan'ning `vkDestroy...`
funksiyalari `VK_NULL_HANDLE` ni xavfsiz qabul qiladi. Shuning uchun image view sikli ichida
alohida `if` kerak emas. Ro'yxat bo'sh bo'lmasa, `device` ham albatta yaratilgan bo'ladi.

---

## Tekshiruv

Build va ishga tushirish:

```bash
cmake --build build
./build/app          # Mac (zsh)
.\build\app.exe      # Windows (PowerShell)
```

Konsolda avvalgi xabarlardan tashqari taxminan shunday qatorlar chiqadi:

**Mac (M1 Pro):**
```
----[ INFO ]-----Surface yaratildi!
GPU: Apple M1 Pro | mos
----[ INFO ]-----Tanlangan GPU: Apple M1 Pro. queueFamilyIndex: 0
----[ INFO ]-----Logical device va queue yaratildi
----[ INFO ]-----Swapchain yaratildi: 2560x1440, 3 ta image
```

**Windows (RTX 3060):**
```
----[ INFO ]-----Surface yaratildi!
GPU: NVIDIA GeForce RTX 3060 | diskret | mos
----[ INFO ]-----Tanlangan GPU: NVIDIA GeForce RTX 3060. queueFamilyIndex: 0
----[ INFO ]-----Logical device va queue yaratildi
----[ INFO ]-----Swapchain yaratildi: 1280x720, 3 ta image
```

Oyna avvalgidek ochiladi. Uning ichi bo'sh (qora yoki oq) bo'lishi normal: biz hali hech narsa
chizmayapmiz va ekranga chiqarmayapmiz. Yopganda `[ ERROR ]` yoki `[ WARNING ]` chiqmasligi kerak.

> **Oynani hozircha kattalashtirmang.** Swapchain'ning o'lchami qat'iy: u yaratilgan paytdagi
> oyna o'lchamida qoladi. Hozir buning zarari yo'q, chunki hech narsa chiqarmayapmiz. 07-darsda
> esa oyna o'lchami o'zgarganda swapchain'ni **qayta yaratamiz** (4-mashq shunga tayyorgarlik).

### Ataylab xato qiling

1. `cleanup()` dagi image view siklini vaqtincha izohga oling. Yopganda validation layer
   `vkDestroyDevice` paytida yo'q qilinmagan `VkImageView` obyektlari haqida xabar beradi.
   Nechta xabar chiqdi va nega aynan shuncha? Keyin qaytaring.
2. `surface` blokini `cleanup()` ning eng **boshiga** ko'chiring (swapchain'dan oldin). Qaysi
   `VUID` chiqadi va u nima deydi? Keyin qaytaring.
3. `createLogicalDevice` da `enabledExtensions` ni yana bo'sh ro'yxatdan boshlang (swapchain
   kengaytmasini yoqmang). Nima bo'ldi: validation xabarimi, `VK_CHECK` xatosimi yoki dastur
   yiqildimi? Natijani yozing va qaytaring.

### Mumkin bo'lgan xatolar

| Xato | Sabab |
|---|---|
| `'glfwCreateWindowSurface' was not declared` | `GLFW_INCLUDE_VULKAN` `#include <GLFW/glfw3.h>` dan **keyin** yozilgan yoki yo'q |
| `'clamp' is not a member of 'std'` | `#include <algorithm>` yo'q (yoki C++17 yoqilmagan) |
| `Mos GPU topilmadi!` (avval topilardi) | `createSurface` `pickPhysicalDevice` dan keyin chaqirilgan: `surface` hali `VK_NULL_HANDLE`, present tekshiruvi o'tmaydi |
| `vkCreateSwapchainKHR xato qaytardi: -1000000000` | `VK_ERROR_SURFACE_LOST_KHR`: surface yo'q yoki noto'g'ri |
| `VUID-VkSwapchainCreateInfoKHR-imageExtent-...` | Extent `minImageExtent..maxImageExtent` oralig'idan tashqarida |
| `VUID-vkDestroySurfaceKHR-surface-01266` | Surface undan yaratilgan swapchain'dan oldin yo'q qilingan |
| Mac: swapchain `1280x720` | Extent `WIDTH`/`HEIGHT` dan yoki `glfwGetWindowSize` dan olingan |

---

## Mashqlar

1. **Surface pasporti.** `createSwapchain` boshida ikki kompyuterda ham chiqaring: formatlar soni,
   present mode'lar ro'yxati **nomlari bilan** (`switch` bilan `presentModeName(VkPresentModeKHR)`
   funksiyasini yozing: FIFO, FIFO_RELAXED, MAILBOX, IMMEDIATE, boshqa), `minImageCount`,
   `maxImageCount` va tanlangan present mode. Natijani Nazariyadagi jadvallar bilan solishtiring.
   Windows'dagi haqiqiy natijani Claude'ga yuboring: jadval to'g'rilanadi.

2. **Point va piksel.** `createSwapchain` da `glfwGetWindowSize`, `glfwGetFramebufferSize` va
   `capabilities.currentExtent` ni yonma-yon chiqaring. Mac va Windows'da qanday farq bor?
   Mac'da *System Settings → Displays* da boshqa o'lchamni ("More Space") tanlab, qayta ishga
   tushiring. Qaysi son o'zgardi, qaysisi yo'q?

3. **Rasmlar soni.** `imageCount` ni `minImageCount` (ya'ni `+ 1` siz) qilib ko'ring. Keyin `100`
   qilib ko'ring (maksimum tekshiruvini vaqtincha olib tashlab). Ikki holatda nima bo'ldi? Keyin
   qaytaring.

4. **Swapchain eskiradi.** `mainLoop` da har kadrda `glfwGetFramebufferSize` ni o'qing va
   `swapchainExtent` bilan solishtiring. Farq paydo bo'lsa, **bir marta** sariq ogohlantirish
   chiqaring: *"oyna o'lchami o'zgardi: swapchain eskirdi"* (ikkala o'lchami bilan). Oynani
   kattalashtirib ko'ring. Windows'da oynani kichraytirib (minimize), vazifalar panelida
   yashiring: framebuffer o'lchami qanday bo'ladi? Bu 07-darsda kerak bo'ladi.

5. **`enableVsync = false`.** Vaqtincha `false` qiling va 1-mashqdagi chiqish bilan qaysi present
   mode tanlanganini ikkala kompyuterda ko'ring. Keyin `true` ga qaytaring.

---

## O'zingizni tekshiring

1. Nega Vulkan oyna bilan ishlashni yadroga emas, kengaytmalarga ajratgan?
2. Surface nimaga tegishli: instance'gami yoki device'gami? Nega u GPU tanlashdan oldin yaratiladi?
3. Instance extension va device extension farqi nima? `VK_KHR_surface` va `VK_KHR_swapchain`
   qaysi turga kiradi?
4. Nega present support'ni alohida tekshiramiz, GRAPHICS bayrog'i yetarli emasmi?
5. Tearing nima va FIFO uni qanday oldini oladi? MAILBOX FIFO'dan nimasi bilan farq qiladi?
6. Retina ekranda nega swapchain o'lchami oyna o'lchamidan 2 marta katta?
7. `currentExtent.width == 0xFFFFFFFF` nimani anglatadi?
8. Nega `minImageCount + 1` so'raymiz va swapchain yaratilgach rasmlar sonini qaytadan so'raymiz?
9. Image va image view qanday farq qiladi? Nega swapchain image'larini yo'q qilmaymiz, view'larni
   esa yo'q qilamiz?
10. Nega `imageSharingMode = EXCLUSIVE` bizga mos keladi?
11. `void f(std::vector<int> v)` va `void f(const std::vector<int>& v)` qanday farq qiladi?

---

**Oldingi dars:** [05 — Physical va logical device](05_physical_va_logical_device.md) ·
**Keyingi dars:** 07 — Command buffer va sinxronizatsiya *(06-dars tugagach yoziladi)*
