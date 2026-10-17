# 04-dars. Instance va validation layer

## Maqsad

Dars oxirida:
- Birinchi haqiqiy Vulkan obyekti — **`VkInstance`** ni yaratasiz va to'g'ri yo'q qilasiz
- **Validation layer**'ni yoqasiz va u xatoni qanday ushlashini o'z ko'zingiz bilan ko'rasiz
- **Debug messenger** orqali layer xabarlarini o'zingiz yozgan funksiyaga yo'naltirasiz
- 03-darsda va'da qilingan savolga javob olasiz: `sType` va `pNext` nima uchun kerak
- C++ dan: `std::vector`, reference (`&`), exception (`throw` / `try` / `catch`) va funksiya
  pointer'i bilan tanishasiz

Bu darsda kod ko'p, lekin har bir bo'lagi kichik. Qadamlarni ketma-ket bajaring va har bir
qadamdan keyin build qilib ko'ring.

---

## Nazariya

### 1. Instance — Vulkan bilan "shartnoma"

`VkInstance` — dasturingiz bilan Vulkan loader orasidagi aloqa. U yaratilganda loader:
- dasturingiz haqida ma'lumot oladi (nomi, qaysi Vulkan versiyasiga mo'ljallangani)
- so'ralgan **layer**'larni yuklaydi
- so'ralgan **kengaytma** (extension)larni yoqadi

Qolgan hamma narsa (GPU tanlash, surface, device) instance'dan boshlanadi. 03-darsdagi xaritada
u eng tepada turgani shuning uchun.

Instance yaratish uchun ikkita "blanka" to'ldiriladi:

| Struct | Nima haqida |
|---|---|
| `VkApplicationInfo` | Dastur haqida: nomi, versiyasi va **qaysi Vulkan versiyasiga mo'ljallangani** (`apiVersion`) |
| `VkInstanceCreateInfo` | Instance haqida: yuqoridagi `VkApplicationInfo` ga pointer, layer'lar va kengaytmalar ro'yxati |

`apiVersion = VK_API_VERSION_1_3` degani: *"dasturim Vulkan 1.3 gacha bo'lgan imkoniyatlarni
ishlatadi"*. Loader va drayveringiz 1.4 ni ham biladi, lekin bizga 1.3 yetadi.

### 2. `sType` va `pNext`: nega har bir struct o'zini tanishtiradi

03-darsda `sType` haqida *"nega kerakligini 04-darsda ko'ramiz"* degan edik. Javob `pNext`
maydonida.

Vulkan yillar davomida kengayib boradi. Eski struct'ga yangi maydon qo'shib bo'lmaydi: eski
dasturlar buziladi. Shuning uchun har bir struct'da `pNext` degan "ilmoq" bor. Unga **qo'shimcha**
struct ulash mumkin, unga yana boshqasini ulash mumkin va hokazo. Bu **pNext zanjiri** deyiladi:

```
 VkInstanceCreateInfo                    VkDebugUtilsMessengerCreateInfoEXT
 ┌─────────────────────────────────┐     ┌──────────────────────────────────────────┐
 │ sType = ..._INSTANCE_CREATE_INFO│     │ sType = ..._DEBUG_UTILS_MESSENGER_...    │
 │ pNext ──────────────────────────┼────►│ pNext = nullptr     (zanjir oxiri)       │
 │ pApplicationInfo                │     │ messageSeverity, pfnUserCallback, ...    │
 │ ...                             │     └──────────────────────────────────────────┘
 └─────────────────────────────────┘
```

`pNext` ning tipi `const void*`: *"istalgan narsaga pointer"*. Drayver zanjir bo'ylab yurganda
keyingi struct qanday tipda ekanini bilmaydi. Shuning uchun u avval **birinchi maydonni**, ya'ni
`sType` ni o'qiydi: *"aha, bu debug messenger blankasi ekan"*. Har bir struct `sType` bilan
boshlanishining sababi shu.

**O'xshatish:** pochta qutisiga turli xatlar tashlanadi. Pochtachi har bir konvertni ochishdan
oldin ustidagi yorliqqa qaraydi: *"soliq idorasiga"*, *"bankka"*. `sType` — o'sha yorliq.

Bu darsda `pNext` zanjirini bir marta ishlatamiz (6-bo'lim).

### 3. Layer va extension: farqi nima

Ikkalasi ham instance yaratilganda yoqiladi, lekin vazifasi har xil:

| | **Layer** (qatlam) | **Extension** (kengaytma) |
|---|---|---|
| Nima | Dastur bilan drayver orasiga qo'yiladigan **oraliq dastur** | Vulkan'ga qo'shiladigan **yangi funksiya va tiplar** |
| Nima qiladi | Chaqiruvlarni kuzatadi, tekshiradi, yozib boradi | Yangi imkoniyat beradi |
| Misol | `VK_LAYER_KHRONOS_validation` | `VK_KHR_surface`, `VK_EXT_debug_utils` |
| Funksionallikni o'zgartiradimi | Yo'q, faqat kuzatadi (yoki biroz sekinlashtiradi) | Ha, yangi narsa qo'shadi |

```
 app.exe ──► loader ──► [validation layer] ──► drayver ──► GPU
                         │
                         └─ har bir chaqiruvni tekshiradi; xato topsa
                            "xabar" yuboradi (debug messenger orqali sizga)
```

Bizga kerak bo'ladigan kengaytmalar:
- GLFW talab qiladiganlar: `VK_KHR_surface`, `VK_KHR_win32_surface` (03-darsning 4-mashqida
  ko'rgansiz). Ular 06-darsda oynaga chizish uchun kerak.
- `VK_EXT_debug_utils` — validation layer xabarlarini o'z funksiyamizga olish uchun.

### 4. Loader layer'ni qayerdan topadi

Har bir layer ikki fayldan iborat: kod (`VkLayer_khronos_validation.dll`) va uning "pasporti"
(`VkLayer_khronos_validation.json`). JSON faylda layer'ning nomi va DLL qayerdaligi yozilgan.
Loader pasportlarni shu joylardan qidiradi:

| Qayerdan | Kim ishlatadi |
|---|---|
| Windows registry (`HKLM\SOFTWARE\Khronos\Vulkan\...`) | Vulkan SDK, Steam, drayverlar o'rnatilganda o'zini shu yerga yozadi |
| `VK_LAYER_PATH` muhit o'zgaruvchisi | Standart qidiruvni **almashtiradi**: faqat shu papkaga qaraladi |
| `VK_ADD_LAYER_PATH` muhit o'zgaruvchisi | Standart qidiruvga papka **qo'shadi** |

Sizda MSYS2'ning validation layer'i `C:\msys64\mingw64\bin` da turibdi. Kompyuteringizda
`VK_ADD_LAYER_PATH=C:\msys64\mingw64\bin` foydalanuvchi muhit o'zgaruvchisi sifatida **doimiy**
o'rnatilgan, shuning uchun loader uni har doim topadi. Tekshirish uchun PowerShell'da:

```powershell
$env:VK_ADD_LAYER_PATH
```

Layer'lar yana ikki turga bo'linadi. **Implicit** layer'lar so'ralmasa ham o'zi yuklanadi
(masalan, `vulkaninfo` da ko'rgan Steam overlay va NVIDIA layer'lari). **Explicit** layer'lar
esa faqat dastur so'raganda yuklanadi. Validation — explicit.

### 5. Validation layer va debug messenger

03-darsda aytgandek, Vulkan drayveri xatolarni tekshirmaydi. Validation layer esa har bir
chaqiruvni Vulkan spetsifikatsiyasidagi minglab qoidalar bilan solishtiradi. Qoida buzilsa,
xabar tayyorlaydi. Xabarda qoidaning raqami (**VUID**, masalan `VUID-vkDestroyInstance-instance-00629`)
va spetsifikatsiyadagi asl matni bo'ladi.

Bu xabarni olish uchun **debug messenger** yaratamiz: *"xabar bo'lsa, mana shu funksiyamni
chaqir"*. Bu 02-darsdagi GLFW callback'i bilan bir xil g'oya.

Xabarlar muhimligiga qarab 4 darajada keladi. Har bir daraja — bitta **bit**:

```
 VERBOSE  = 0x0001 = 0000 0000 0000 0001   juda batafsil (loader nima qilayotgani)
 INFO     = 0x0010 = 0000 0000 0001 0000   ma'lumot uchun
 WARNING  = 0x0100 = 0000 0001 0000 0000   shubhali joy
 ERROR    = 0x1000 = 0001 0000 0000 0000   qoida buzildi

 WARNING | ERROR   = 0001 0001 0000 0000   ← ikkala "kalit" ham yoqilgan
```

Bitlar chiroq kalitlari paneliga o'xshaydi. 03-darsdagi `&` operatori *"shu kalit yoqilganmi?"*
deb tekshiradi. Yangi `|` operatori esa bir nechta kalitni **birga yoqadi**. Biz messenger'ga
faqat WARNING va ERROR ni yuborishni aytamiz, aks holda VERBOSE xabarlar ekranni to'ldirib
yuboradi.

### 6. Messenger instance'dan keyin tug'iladi. Instance yaratilayotganda nima bo'ladi?

Debug messenger instance'ning "bolasi": u instance yaratilgandan **keyin** yaratiladi va
instance'dan **oldin** yo'q qilinadi. Unda `vkCreateInstance` va `vkDestroyInstance` ning
o'zidagi xatolarni kim eshitadi?

```
 vaqt ─────────────────────────────────────────────────────────────────────►
       vkCreateInstance   messenger yaratildi ... ish ...   messenger o'chdi   vkDestroyInstance
       ▲                  ├──────── messenger eshitadi ────────┤               ▲
       └── bu yerda hech kim eshitmaydi ───────────────────────────────────────┘
```

Yechim: messenger blankasini `VkInstanceCreateInfo::pNext` ga ulab qo'yamiz (2-bo'limdagi
rasm). Shunda layer **instance yaratilishi va yo'q qilinishi** paytida ham shu blankadagi
funksiyani chaqiradi.

### 7. Funksiya pointer'i va `vkGetInstanceProcAddr`

02-darsda pointer — **ma'lumot** manzilini saqlovchi o'zgaruvchi edi. Funksiyaning kodi ham
xotirada turadi, demak uning ham manzili bor. **Funksiya pointer'i** — funksiya manzilini
saqlovchi o'zgaruvchi. Uni xuddi funksiyaning o'zidek chaqirsa bo'ladi.

Nega kerak? `vkCreateDebugUtilsMessengerEXT` — kengaytma funksiyasi. Kengaytma kompyuterda
bo'lmasligi ham mumkin, shuning uchun bu funksiya loader'ning "telefon kitobi"da (`libvulkan-1.dll.a`
import kutubxonasida) **yo'q**. Uni to'g'ridan-to'g'ri chaqirsangiz, kompilyator rozi bo'ladi
(header'da e'lon bor), lekin linker *"undefined reference"* deydi (2-mashqda sinab ko'rasiz).

Buning o'rniga funksiya manzilini dastur ishlayotganda loader'dan **so'rab olamiz**:

```
 siz:     vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
           │  "shu nomli funksiyaning raqamini bering"
           ▼
 loader:  manzil (yoki kengaytma yoqilmagan bo'lsa nullptr)
```

**O'xshatish:** telefon kitobida yo'q odamga qo'ng'iroq qilish uchun ma'lumotnomaga qo'ng'iroq
qilib, raqamini so'raysiz. Keyin o'sha raqamni terasiz.

Vulkan har bir funksiya pointer'i uchun tayyor tip nomini beradi: `PFN_` + funksiya nomi
(*Pointer to FuNction*). Masalan, `PFN_vkCreateDebugUtilsMessengerEXT`.

### 8. C++: `std::vector`

Vulkan'da ko'p narsa **ro'yxat** bo'ladi: layer'lar, kengaytmalar, GPU'lar. Ro'yxatning uzunligi
oldindan noma'lum. **`std::vector`** — o'lchami o'zgara oladigan massiv. Uni cho'ziluvchan javon
deb tasavvur qiling:

```cpp
#include <vector>

std::vector<int> a;          // bo'sh ro'yxat
a.push_back(10);             // oxiriga qo'shish: [10]
a.push_back(20);             // [10, 20]
int x = a[1];                // indeks bo'yicha olish: 20
size_t n = a.size();         // elementlar soni: 2

std::vector<int> b(5);       // 5 ta element, hammasi 0: [0, 0, 0, 0, 0]
int* p = b.data();           // birinchi elementning manzili
```

`.data()` nega kerak? Vulkan C tilida yozilgan va `std::vector` ni bilmaydi. U *"birinchi
elementning manzili + elementlar soni"* ni kutadi. `.data()` va `.size()` aynan shuni beradi.

### 9. C++: reference (`&`) va ro'yxat bo'ylab yurish

```cpp
for (const VkLayerProperties& layer : layers) {
    // layer — ro'yxatdagi navbatdagi element
}
```

Bu **range-for**: *"`layers` dagi har bir element uchun"*. `&` bu yerda **reference** (havola)
degani: o'zgaruvchining ikkinchi nomi, nusxasi emas. `VkLayerProperties` ~520 bayt. Har aylanishda
uni nusxalash o'rniga aslining o'ziga qaraymiz. `const` esa *"faqat o'qiyman, o'zgartirmayman"*
degani.

E'tibor bering: `&` ikki xil ma'noda keladi.

| Qayerda | Ma'nosi | Misol |
|---|---|---|
| Ifodada, o'zgaruvchi oldida | **Manzil** (02-dars) | `glfwGetCursorPos(window, &mouseX, &mouseY)` |
| E'londa, tip yonida | **Reference** (yangi) | `const VkLayerProperties& layer` |

### 10. C++: C satrlarini solishtirish

`VkLayerProperties` ichida layer nomi `char layerName[256]` — oddiy C satri (harflar massivi).
Ikkita C satrini `==` bilan solishtirsangiz, **matnlar emas, manzillar** solishtiriladi: *"ikkalasi
xotiraning bir joyidami?"*. Javob deyarli doim "yo'q". Matnni solishtirish uchun `<cstring>` dagi
`std::strcmp` ishlatiladi: matnlar bir xil bo'lsa, `0` qaytaradi.

```cpp
std::strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0   // to'g'ri
layer.layerName == "VK_LAYER_KHRONOS_validation"                   // NOTO'G'RI: manzillar
```

### 11. C++: exception (`throw`, `try`, `catch`)

03-darsda siz `throw std::runtime_error(...)` yozdingiz. Bu **exception** ("favqulodda holat").
U qanday ishlashini tushunib olaylik, chunki bu darsdan boshlab xatolarni shu usulda
qayta ishlaymiz.

`throw` — yong'in signalizatsiyasi. Signal chalinganda:
1. Funksiya **o'sha zahoti** to'xtaydi. Undan keyingi qatorlar bajarilmaydi.
2. C++ chaqiruvlar zanjiri bo'ylab yuqoriga ko'tariladi va eng yaqin `catch` ni qidiradi.
3. `catch` topilsa, dastur o'sha yerdan davom etadi. Topilmasa, dastur **halokatli tugaydi**
   (`terminate called after throwing an instance of 'std::runtime_error'`).

```
 main()      try { run(); }  catch (...) { xabar chiqarish } ◄──────┐
               └─ run()                                              │
                    └─ createInstance()                              │
                         └─ vkCheck(...)                             │
                              throw std::runtime_error("...") ───────┘
                              (oradagi barcha funksiyalarning qolgan qismi bajarilmaydi)
```

Shuning uchun 03-darsdagi kodingizda `throw` dan keyingi `glfwTerminate();` va `return 1;` hech
qachon bajarilmasdi. `catch` ham yo'q edi, demak `glfwVulkanSupported()` `GLFW_FALSE` qaytarganda
dastur halokatli tugardi.

Exception'ning afzalligi: xato chuqurdagi funksiyada yuz bersa ham, uni bitta joyda (`main` da)
ushlash mumkin. Har bir funksiyadan `bool` qaytarib, har qadamda tekshirish shart emas.

> `throw` bo'lganda pastdagi tozalash kodi (`vkDestroyInstance`, `glfwTerminate`) ishlamaydi.
> Hozircha buni qabul qilamiz: xato bo'lsa dastur baribir tugaydi va Windows jarayonning
> barcha resurslarini o'zi qaytarib oladi. Buni chiroyli hal qilish yo'lini keyingi darslarda
> ko'ramiz.

---

## Amaliyot

Oxirida `main.cpp` taxminan shunday tuzilgan bo'ladi (yuqoridan pastga):

```
 #include'lar
 enableValidation, isPaused          global o'zgaruvchilar
 vkCheck()                           VkResult ni tekshirish
 printVulkanVersion()                03-darsdan
 isLayerAvailable()                  layer o'rnatilganmi
 getRequiredExtensions()             kengaytmalar ro'yxati
 debugCallback()                     layer xabarlarini chiqarish
 makeDebugMessengerInfo()            messenger blankasi
 createInstance()                    VkInstance yaratish
 createDebugMessenger()              messenger yaratish
 destroyDebugMessenger()             messenger'ni yo'q qilish
 run()                               avvalgi main'ning ichi
 main()                              try { run(); } catch ...
```

Har bir funksiya o'zidan **pastdagilar**ga emas, **yuqoridagilar**ga tayanadi (01-dars: kompilyator
funksiyani ishlatilishidan oldin ko'rgan bo'lishi kerak).

### 1-qadam: 03-darsdan qolganlarni tuzating

1. **`Vulkan::Vulkan` ni qaytaring.** 5-mashqdan keyin `target_link_libraries` da faqat `glfw`
   qolib ketgan. Hozir loyiha linker bosqichida yiqiladi va `build/app.exe` yo'q.
2. **Maskalarni tuzating.** `printVulkanVersion` dagi har bir maskada birlar soni bitta kam
   (sharhda batafsil). Tuzatib, natijani makroslar bilan solishtiring. Keyin kodda
   **makroslarni** qoldiring: ular o'qishga oson va xato qilib bo'lmaydi.
3. `#include <cstdlib>` kerak emas. Uning o'rniga quyidagilar kerak bo'ladi:
   ```cpp
   #include <cstring>     // std::strcmp
   #include <stdexcept>   // std::runtime_error
   #include <vector>      // std::vector
   ```

### 2-qadam: `main` ni `run` ga ko'chiring va exception'larni ushlang

`main` ichidagi hamma narsani yangi `void run()` funksiyasiga ko'chiring. `return 1;` lar
o'rniga `throw std::runtime_error("...")` yozing (`void` funksiya son qaytarmaydi).
`glfwVulkanSupported` tekshiruvidagi `throw` dan keyingi ikki qatorni o'chiring: ular hech qachon
bajarilmaydi. `glfwGetRequiredInstanceExtensions` va kengaytmalarni chiqaradigan siklni ham
o'chiring: ular endi 5-qadamdagi funksiyaga ko'chadi.

Yangi `main`:

```cpp
int main() {
    try {
        run();
    } catch (const std::exception& e) {
        std::cerr << "XATO: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

| Kod | Ma'nosi |
|---|---|
| `try { ... }` | *"Shu blok ichida exception bo'lishi mumkin"* |
| `catch (const std::exception& e)` | Har qanday standart exception'ni ushlaydi. `std::runtime_error` ham `std::exception` ning bir turi |
| `e.what()` | `throw` paytida berilgan xabar matni |

Build qilib ko'ring: dastur avvalgidek ishlashi kerak.

### 3-qadam: `vkCheck` — xatoni tekshiruvchi yordamchi

Har bir Vulkan chaqiruvidan keyin `if (result != VK_SUCCESS)` yozib o'tirmaslik uchun:

```cpp
// Vulkan funksiyasi xato qaytarsa, exception otadi.
// what — qaysi funksiya ekanini xabarda ko'rsatish uchun
void vkCheck(VkResult result, const char* what) {
    if (result != VK_SUCCESS) {
        throw std::runtime_error(std::string(what) + " xato qaytardi: " + std::to_string(result));
    }
}
```

`std::string(what)` — C satridan `std::string` yasaymiz, shunda `+` bilan matn ulash ishlaydi.

Endi `printVulkanVersion` ni ham shunga o'tkazing: u `void` bo'lsin va xatoni `vkCheck` bilan
tekshirsin. `run()` da esa shunchaki `printVulkanVersion();` deb chaqiriladi.

### 4-qadam: layer o'rnatilganmi?

Bu yerda Vulkan'ning eng ko'p takrorlanadigan usuli bor: **ikki chaqiruvli ro'yxat olish**.

```cpp
// Kompyuterda shu nomli layer o'rnatilganmi?
bool isLayerAvailable(const char* name) {
    // 1-chaqiruv: nechta layer bor?
    uint32_t count = 0;
    vkCheck(vkEnumerateInstanceLayerProperties(&count, nullptr), "vkEnumerateInstanceLayerProperties");

    // 2-chaqiruv: shuncha joy ajratib, ro'yxatning o'zini olamiz
    std::vector<VkLayerProperties> layers(count);
    vkCheck(vkEnumerateInstanceLayerProperties(&count, layers.data()), "vkEnumerateInstanceLayerProperties");

    for (const VkLayerProperties& layer : layers) {
        if (std::strcmp(layer.layerName, name) == 0) {
            return true;
        }
    }
    return false;
}
```

```
 1-chaqiruv:  vkEnumerate...(&count, nullptr)        "nechta?"        → count = 5
              std::vector<...> layers(count)          5 ta joy tayyorlaymiz
 2-chaqiruv:  vkEnumerate...(&count, layers.data())  "shu joyga yoz"  → layers to'ldi
```

**O'xshatish:** restoranga qo'ng'iroq qilasiz: *"Necha kishisiz?"* — *"Beshta"*. Keyin besh
kishilik stol tayyorlanadi. Vulkan ham avval sonini aytadi, joyni esa **siz** ajratasiz.
Xotira ustidan nazorat sizda qoladi (03-dars: explicit).

Bu usul keyingi darslarda ko'p marta uchraydi: GPU'lar, queue'lar, surface formatlari,
swapchain rasmlari.

### 5-qadam: kengaytmalar ro'yxati

```cpp
// Validation yoqilgan yoki yoqilmaganiga qarab qo'shiladi
const bool enableValidation = true;

// Instance uchun kerak bo'lgan kengaytmalar
std::vector<const char*> getRequiredExtensions() {
    uint32_t count = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

    std::vector<const char*> extensions;
    for (uint32_t i = 0; i < count; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

    if (enableValidation) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // "VK_EXT_debug_utils"
    }
    return extensions;
}
```

`enableValidation` ni faylning yuqorisiga, `isPaused` yoniga qo'ying. Sikl sizga tanish:
03-darsda xuddi shunday sikl bilan nomlarni chiqargansiz. Endi ularni `std::vector` ga yig'yapmiz.
`VK_EXT_DEBUG_UTILS_EXTENSION_NAME` — header'dagi makros. Nomni qo'lda yozganda xato qilish
oson, makros bilan esa kompilyator tekshiradi.

### 6-qadam: debug callback va messenger blankasi

```cpp
// Validation layer (yoki loader) xabar yuborganda shu funksiya chaqiriladi
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT /*type*/,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* /*userData*/) {
    const char* level = "INFO";
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        level = "XATO";
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        level = "OGOHLANTIRISH";
    }

    std::cerr << "[Vulkan " << level << "] " << data->pMessage << "\n" << std::endl;
    return VK_FALSE;
}

// Messenger blankasi: qaysi xabarlar va qaysi funksiyaga
VkDebugUtilsMessengerCreateInfoEXT makeDebugMessengerInfo() {
    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = debugCallback;
    return info;
}
```

| Kod | Ma'nosi |
|---|---|
| `VKAPI_ATTR ... VKAPI_CALL` | Funksiya Vulkan kutgan "chaqirish qoidasi" bilan yaratilsin degan belgilar. 64-bit Windows'da amalda hech narsa qilmaydi, lekin boshqa platformalar uchun shart. Yozib qo'yamiz va unutamiz |
| `VkBool32` | Vulkan'ning `bool` i (32 bitli son: `VK_TRUE` / `VK_FALSE`) |
| `severity & ..._ERROR_BIT_EXT` | *"ERROR kaliti yoqilganmi?"* (Nazariya, 5-bo'lim) |
| `data->pMessage` | `->` — pointer orqali struct maydoniga murojaat. `(*data).pMessage` ning qisqa yozilishi |
| `return VK_FALSE;` | *"Chaqiruvni to'xtatma, davom et"*. Spetsifikatsiya doim `VK_FALSE` qaytarishni buyuradi |
| `info.messageSeverity = A \| B` | Qaysi darajadagi xabarlar kerak: WARNING va ERROR |
| `info.messageType` | Qaysi turdagi xabarlar: umumiy, validation qoidalari, unumdorlik maslahatlari |
| `info.pfnUserCallback = debugCallback;` | Funksiya nomi `()` siz yozilsa, uning **manzili** olinadi. Bu funksiya pointer'i (Nazariya, 7-bo'lim) |

Parametr nomlari izohda (`/*type*/`), chunki ular ishlatilmayapti (02-dars).

### 7-qadam: instance yaratish

```cpp
VkInstance createInstance() {
    if (enableValidation && !isLayerAvailable("VK_LAYER_KHRONOS_validation")) {
        throw std::runtime_error("VK_LAYER_KHRONOS_validation topilmadi. VK_ADD_LAYER_PATH o'rnatilganmi?");
    }

    // 1. Dastur haqida
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Fluid Sim Tutorial";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.pEngineName = "Yo'q";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // 2. Kengaytmalar va layer'lar
    std::vector<const char*> extensions = getRequiredExtensions();
    std::vector<const char*> layers;
    if (enableValidation) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    // 3. Instance blankasi
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();

    // 4. Instance yaratilishi va yo'q qilinishidagi xabarlar uchun (Nazariya, 6-bo'lim)
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = makeDebugMessengerInfo();
    if (enableValidation) {
        createInfo.pNext = &debugInfo;
    }

    VkInstance instance = VK_NULL_HANDLE;
    vkCheck(vkCreateInstance(&createInfo, nullptr, &instance), "vkCreateInstance");
    return instance;
}
```

| Kod | Ma'nosi |
|---|---|
| `VK_MAKE_API_VERSION(0, 0, 1, 0)` | 03-darsdagi bitlarni **joylaydigan** makros: variant 0, versiya 0.1.0. Bu sizning dasturingiz versiyasi, Vulkan'niki emas |
| `apiVersion = VK_API_VERSION_1_3` | Dastur mo'ljallangan Vulkan versiyasi |
| `static_cast<uint32_t>(extensions.size())` | `size()` 64 bitli `size_t` qaytaradi, Vulkan esa `uint32_t` kutadi. `static_cast` — *"bilaman, aylantir"* deb aniq aytish |
| `extensions.data()` | Birinchi nomning manzili. Vulkan shu manzildan boshlab `count` ta nomni o'qiydi |
| `createInfo.pNext = &debugInfo;` | Messenger blankasini zanjirga ulash |

**Muhim:** `appInfo`, `extensions`, `layers`, `debugInfo` — hammasi `createInstance` ning lokal
o'zgaruvchilari. `vkCreateInstance` ularni chaqiruv paytida o'qib oladi, shuning uchun funksiyadan
chiqqanda ular yo'qolib ketsa ham muammo yo'q. Lekin `vkCreateInstance` chaqirilguncha ular
tirik bo'lishi shart: `createInfo` ular ichiga pointer orqali "qarab turibdi".

### 8-qadam: debug messenger'ni yaratish va yo'q qilish

```cpp
VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance) {
    // Kengaytma funksiyasining manzilini loader'dan so'raymiz (Nazariya, 7-bo'lim)
    auto createFn = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (createFn == nullptr) {
        throw std::runtime_error("vkCreateDebugUtilsMessengerEXT topilmadi");
    }

    VkDebugUtilsMessengerCreateInfoEXT info = makeDebugMessengerInfo();
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    vkCheck(createFn(instance, &info, nullptr, &messenger), "vkCreateDebugUtilsMessengerEXT");
    return messenger;
}

void destroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger) {
    auto destroyFn = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (destroyFn != nullptr) {
        destroyFn(instance, messenger, nullptr);
    }
}
```

| Kod | Ma'nosi |
|---|---|
| `vkGetInstanceProcAddr(instance, "...")` | *"Shu nomli funksiyaning manzilini ber"*. Kengaytma yoqilmagan bo'lsa, `nullptr` |
| `reinterpret_cast<PFN_...>(...)` | `vkGetInstanceProcAddr` "umumiy" funksiya pointer'ini qaytaradi. Biz unga *"buni aynan shu turdagi funksiya deb qara"* deymiz |
| `auto createFn = ...` | Tipni kompilyator o'zi aniqlaydi (o'ng tomondagi `PFN_...` dan). Uzun nomni ikki marta yozmaslik uchun |
| `createFn(instance, &info, nullptr, &messenger)` | Pointer orqali chaqiruv: xuddi oddiy funksiyadek |

### 9-qadam: `run()` da hammasini ulang

`glfwVulkanSupported` tekshiruvidan keyin, oyna yaratishdan oldin:

```cpp
    VkInstance instance = createInstance();
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    if (enableValidation) {
        debugMessenger = createDebugMessenger(instance);
        std::cout << "Validation layer yoqildi" << std::endl;
    }
```

`run()` ning oxiridagi tozalash qismi — **yaratilishga teskari tartibda**:

```cpp
    glfwDestroyWindow(window);
    if (enableValidation) {
        destroyDebugMessenger(instance, debugMessenger);
    }
    vkDestroyInstance(instance, nullptr);
    glfwTerminate();
```

```
 yaratish:     glfwInit → instance → messenger → window
 yo'q qilish:  window → messenger → instance → glfwTerminate
```

### 10-qadam: build va ishga tushirish

```powershell
cmake --build build
.\build\app.exe
```

---

## Tekshiruv

Konsolda:

```
Vulkan (loader) versiyasi: 1.4.341
Validation layer yoqildi
```

Keyin oyna ochiladi va dastur avvalgidek ishlaydi. Oynani yopganda hech qanday `[Vulkan ...]`
xabari chiqmasligi kerak: toza dastur — jim dastur.

`EOSOverlayVkLayer-Win64.json` haqidagi xabar endi `[Vulkan XATO] loader_get_json: ...` ko'rinishida,
ya'ni sizning callback'ingiz orqali ham chiqishi mumkin. Loader o'z xabarlarini ham messenger'ga
yuboradi. Bu baribir o'sha zararsiz Epic Games xabari.

### Layer haqiqatan ishlayaptimi? Ataylab xato qiling

`run()` oxiridagi `destroyDebugMessenger(instance, debugMessenger);` qatorini vaqtincha izohga
oling, build qilib, dasturni ishga tushiring va oynani yoping. Taxminan shunday xabar chiqadi:

```
[Vulkan XATO] Validation Error: [ VUID-vkDestroyInstance-instance-00629 ] ...
vkDestroyInstance(): Object Tracking - For VkInstance 0x..., VkDebugUtilsMessengerEXT 0x...
has not been destroyed. The Vulkan spec states: All child objects created using instance
must have been destroyed prior to destroying instance
```

Xabarni o'qing: qaysi qoida (`VUID-...`), qaysi funksiyada (`vkDestroyInstance`), qaysi obyekt
yo'q qilinmagan (`VkDebugUtilsMessengerEXT`). Bu xabar `pNext` ga ulangan blanka orqali keldi:
`vkDestroyInstance` paytida eshitadigan yagona "quloq" shu. Xabar ikki marta chiqsa, hayron
bo'lmang: unutilgan messenger ham hali tirik va u ham eshitadi. Keyin qatorni qaytaring.

Validation layer'siz bu xato **hech qanday belgisiz** o'tib ketardi.

### Mumkin bo'lgan xatolar

| Xato matni | Sabab |
|---|---|
| `undefined reference to 'vkEnumerateInstanceVersion'` (va boshqalar) | `Vulkan::Vulkan` hali ulanmagan (1-qadam) |
| `XATO: VK_LAYER_KHRONOS_validation topilmadi...` | Bu terminalda `VK_ADD_LAYER_PATH` yo'q. `$env:VK_ADD_LAYER_PATH` ni tekshiring, VS Code'ni qayta oching |
| `XATO: vkCreateInstance xato qaytardi: -7` | `VK_ERROR_EXTENSION_NOT_PRESENT`: kengaytma nomi xato yoki yo'q |
| `XATO: vkCreateInstance xato qaytardi: -6` | `VK_ERROR_LAYER_NOT_PRESENT`: layer nomi xato |
| `terminate called after throwing an instance of 'std::runtime_error'` | `throw` bor, lekin `main` da `try`/`catch` yo'q (2-qadam) |

---

## Mashqlar

1. **Layer'lar ro'yxati.** `isLayerAvailable` ga o'xshash `printAvailableLayers()` funksiyasini
   yozing: har bir layer'ning `layerName` va `description` maydonlarini chiqarsin. Ro'yxatni
   `vulkaninfo --summary` dagi `Instance Layers` bilan solishtiring. Keyin yangi PowerShell
   oynasida `$env:VK_ADD_LAYER_PATH = ""` qilib, dasturni o'sha oynadan ishga tushiring. Qaysi
   layer yo'qoldi va dastur nima dedi? (Bu buyruq faqat o'sha oynaga ta'sir qiladi.)

2. **Telefon kitobida yo'q.** `createDebugMessenger` ichida `vkGetInstanceProcAddr` o'rniga
   `vkCreateDebugUtilsMessengerEXT(instance, &info, nullptr, &messenger)` ni to'g'ridan-to'g'ri
   chaqirib ko'ring. Xato qaysi bosqichda chiqdi: compiler yoki linker? Nega kompilyator rozi
   bo'ldi? Keyin qaytaring.

3. **Debug va Release.** Validation layer dasturni sekinlashtiradi, shuning uchun tayyor (Release)
   dasturda o'chiriladi. CMake Release build'da kompilyatorga `-DNDEBUG` beradi, ya'ni `NDEBUG`
   makrosi e'lon qilingan bo'ladi (03-dars: `#define`). Shundan foydalaning:
   ```cpp
   #ifdef NDEBUG
   const bool enableValidation = false;
   #else
   const bool enableValidation = true;
   #endif
   ```
   Release'ni alohida papkada build qiling:
   `cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release`, keyin
   `cmake --build build-release`. `build-release\app.exe` da "Validation layer yoqildi" chiqmasligi
   kerak. `.gitignore` ga ham `build-release/` ni qo'shishni unutmang.

4. **`VK_CHECK` makrosi.** Preprocessor `#x` ni *"x ning matni"* ga aylantira oladi:
   ```cpp
   #define VK_CHECK(x) vkCheck((x), #x)
   ```
   Endi `VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));` yozsangiz, xato xabarida
   chaqiruvning to'liq matni chiqadi. `vkCheck` chaqiruvlarini shunga almashtiring. Tekshirish
   uchun `getRequiredExtensions` ga vaqtincha mavjud bo'lmagan `"VK_FOO_bar"` kengaytmasini qo'shing:
   xabar qanday ko'rinadi?

5. **Xabar darajalari.** `makeDebugMessengerInfo` dagi `messageSeverity` ga `INFO` va `VERBOSE`
   bitlarini ham qo'shing. Ishga tushganda nechta xabar keldi? Ularni kim yuboryapti: loader yoki
   layer? Nega odatda faqat WARNING va ERROR ni qoldiramiz? Keyin asl holiga qaytaring.

---

## O'zingizni tekshiring

1. `VkInstance` nima va nega u birinchi yaratiladi?
2. `pNext` nima uchun kerak? Nega u `const void*` va bunga `sType` ning qanday aloqasi bor?
3. Layer va extension qanday farq qiladi? Har biriga misol keltiring.
4. Ikki chaqiruvli ro'yxat olish usuli qanday ishlaydi? Nega Vulkan ro'yxatni birdaniga
   qaytarmaydi?
5. Nega ikkita C satrini `==` bilan solishtirib bo'lmaydi?
6. Nega messenger blankasini `VkInstanceCreateInfo::pNext` ga ham ulaymiz?
7. Nega `vkCreateDebugUtilsMessengerEXT` ni to'g'ridan-to'g'ri chaqirmasdan, `vkGetInstanceProcAddr`
   orqali olamiz?
8. `throw` bo'lganda funksiyaning qolgan qismi bilan nima bo'ladi? `catch` umuman bo'lmasa-chi?
9. Nega validation layer'ni Release build'da o'chiramiz?

---

**Oldingi dars:** [03 — GPU va Vulkan qanday ishlaydi](03_gpu_va_vulkan.md) ·
**Keyingi dars:** 05 — Physical device, logical device, queue *(04-dars tugagach yoziladi)*
