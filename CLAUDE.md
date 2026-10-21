# CLAUDE.md — fluid_sim_tutorial

> Bu fayl Claude uchun yo'riqnoma. Claude Code uni har bir suhbat boshida avtomatik o'qiydi.
> Foydalanuvchi uni o'qib, kelishuvlar to'g'ri yozilganini tekshirishi mumkin.

## Bu loyiha nima

Foydalanuvchi **C++, Vulkan, shaderlar (GLSL) va suyuqlik fizikasini (SPH)** o'rganmoqda.
Buning uchun u [SebLague/Fluid-Sim](https://github.com/SebLague/Fluid-Sim) (Unity) simulatsiyasining
Vulkan versiyasini **o'z qo'li bilan, noldan** quryapti.

- **Tayyor namuna:** `../fluid_sim/`. Uni Claude avvalgi sessiyada yozgan: 2D va 3D, Vulkan 1.3
  (dynamic rendering, synchronization2), GPU'da compute SPH, counting sort + prefix sum,
  spatial hash, orbit kamera. Tuzilishi `../fluid_sim/README.md` da yozilgan. Darslarni yozishda
  undan namuna sifatida foydalaning, lekin foydalanuvchi kodini undan nusxa ko'chirishga undamang.
- **Kurs rejasi:** `docs/00_yol_xaritasi.md` — 6 modul, ~24 dars (Tayyorgarlik → Vulkan asoslari →
  CPU'da 2D SPH → GPU compute → 3D → chiroyli rendering).

## Asosiy qoida: Claude nima qiladi va nima qilmaydi

Foydalanuvchining so'zlari: *"sen faqat dars fayllarini yozib bor, qolgan barcha ishni o'zim qilaman"*.

**Claude QILADI:**
- `docs/` ga dars fayllarini yozadi (`docs/NN_nomi.md`)
- `docs/lugat.md` (atamalar lug'ati) ni har darsdan keyin to'ldiradi
- Shu fayldagi **"Joriy holat"** bo'limini yangilab boradi
- Foydalanuvchi kodini **o'qib**, tekshiradi va tushuntiradi
- Faqat o'qiydigan buyruqlarni ishga tushirishi mumkin (`git status`, `git diff`, `git log`, fayllar ro'yxati)

**Claude QILMAYDI:**
- `src/`, `shaders/`, `CMakeLists.txt` va boshqa papka yoki fayllarni yaratmaydi va o'zgartirmaydi
- Build qilmaydi, dasturni ishga tushirmaydi (natijani foydalanuvchidan so'raydi: xato matni, skrinshot)
- Foydalanuvchi kodini o'zi tuzatmaydi. Nima noto'g'ri, **nega** noto'g'ri va qanday tuzatish
  kerakligini tushuntiradi. Chatda kichik kod parchalarini ko'rsatishi mumkin

Agar foydalanuvchi biror aniq holatda boshqacha so'rasa (masalan, "shu faylni sen tuzat"),
faqat o'sha holat uchun bajaring. Qoida o'zgarmaydi.

## Muloqot

- Faqat **o'zbek tilida (lotin)**. Texnik atamalar inglizcha qoladi va o'zbekcha tushuntiriladi
  (masalan, "swapchain — ekranga chiqariladigan rasmlar navbati").
- **Soddalashtirib**, o'xshatishlar (analogiya) bilan, bosqichma-bosqich. Foydalanuvchi "nega"ni
  chuqur tushunmoqchi.
- Bir vaqtda bitta dars. Ortiqcha ma'lumot bilan bosib tashlamang.
- Foydalanuvchi darslarni juda oson yoki juda qiyin desa, tezlikni moslang.
- `C:\dev` da `cpp-tutorial`, `openGl-tutorial`, `vulkan-tutorial` papkalari bor. Ehtimol oldin
  biroz o'rgangan. Lekin buni taxmin qilmang: darajasini javoblari va kodiga qarab aniqlang.

## Har bir dars sikli

1. Foydalanuvchi yozadi: *"NN-dars tayyor"*, savollar va xatolar bilan.
2. Claude kodini o'qiydi (`CMakeLists.txt`, `src/`, keyinroq `shaders/`) va dars maqsadlari hamda
   mashqlar bajarilganini tekshiradi. So'ng sharh beradi:
   - nima yaxshi chiqqan
   - qaysi xatolar yoki zaif joylar bor va nega
   - "O'zingizni tekshiring" savollariga javob bergan bo'lsa, ularni tekshiradi
3. Muammo bo'lsa, foydalanuvchi o'zi tuzatadi va sikl takrorlanadi.
4. Hammasi joyida bo'lsa, Claude:
   - keyingi dars faylini yozadi (**foydalanuvchining haqiqiy kodiga tayanib**: u ishlatgan nomlar
     va tuzilish bilan mos bo'lishi kerak)
   - `docs/lugat.md` ni to'ldiradi
   - oldingi darsning pastki qismidagi "Keyingi dars" havolasini haqiqiy faylga ulaydi
   - shu fayldagi "Joriy holat" ni yangilaydi

## Dars fayllari formati

- Fayl nomi: `NN_qisqa_nom.md`. O'zbekcha, apostrofsiz, kichik harflar, `_` bilan
  (masalan, `03_gpu_va_vulkan.md`).
- Bo'limlar, shu tartibda: **Maqsad**, **Nazariya**, **Amaliyot** (qadamlar), **Tekshiruv**
  (natija qanday bo'lishi kerak), **Mashqlar**, **O'zingizni tekshiring** (savollar), pastda
  oldingi/keyingi darsga havolalar.
- Diagrammalar ASCII ko'rinishida (mermaid emas), tushunchalar jadval bilan.
- Kod: C++17, 4 bo'shliq bilan indent, koddagi izohlar o'zbekcha. Yangi tushuncha birinchi marta
  chiqqanda to'liq kod va qatorma-qator tushuntirish beriladi. Keyingi darslarda ko'proq skelet
  va yo'l-yo'riq beriladi, foydalanuvchi o'zi to'ldiradi.
- Build: CMake + Ninja, `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`, target nomi `app`.
  Release'ga simulatsiya tezligi kerak bo'lganda o'tiladi.

## Muhit

Foydalanuvchi **ikki kompyuterda navbatma-navbat** ishlaydi: Windows PC va MacBook. Ikkalasi ham
faol. Qaysi birida ekaningizni sessiya muhitidagi platformadan bilasiz (`win32` / `darwin`).

### Ikki kompyuterda ishlash qoidalari

- **Sinxronlash GitHub orqali** (`https://github.com/sardor-c/vulkan_fluid_simulation.git`).
  Sessiya boshida `git status -sb` va `git log --oneline -3` ni ko'ring, foydalanuvchidan esa
  boshqa kompyuterda ishlagan-ishlamaganini va `git pull` qilganini so'rang. `pull`, `commit` va
  `push` ni foydalanuvchi o'zi bajaradi (Claude faqat o'qiydigan buyruqlarni ishlatadi).
- Har bir dars sikli oxirida (yangi dars yozilgach) eslatib qo'ying: kompyuter almashtirishdan oldin
  `commit` + `push` qilinsin. Aks holda darslar va kod ikki kompyuterda har xil bo'lib qoladi.
- Kod **ikkala platformada ham** build bo'lishi kerak. Platformaga xos qismlar `#ifdef __APPLE__`
  (yoki `_WIN32`) ichida yoziladi. Kodni tekshirganda ikkala platformani ham hisobga oling
  (masalan, portability kengaytmalari).
- Darslarda buyruqlar farq qilsa, **ikkalasini ham** yozing: PowerShell va zsh
  (`.\build\app.exe` / `./build/app`, `$env:X = "..."` / `export X=...`). Natija GPU'ga bog'liq
  bo'lsa (GPU nomi, queue family'lar), *"sizda boshqacha bo'lishi mumkin"* deb ayting yoki ikkala
  kompyuter uchun ham ko'rsating.
- `build/` va `build-release/` har bir kompyuterda o'zi (`.gitignore` da). Ularni kompyuterlar
  orasida hech qachon ko'chirmang.
- `.vscode/c_cpp_properties.json` git'da turibdi va ikkala kompyuterga boradi. Unda ikkita
  konfiguratsiya bo'lishi kerak: `"Win32"` va `"Mac"`. cpptools bu nomlarni taniydi va platformaga
  qarab kerakligini o'zi tanlaydi.
- Qator oxirlari: repo'da hamma fayl LF. Windows'da `core.autocrlf=true` (global va system), Mac'da
  sozlanmagan bo'lsa ham LF. Ishonch uchun `.gitattributes` (`* text=auto`) tavsiya qilingan.
- `../fluid_sim` namunasi ikkala kompyuterda alohida nusxa (Mac'ga zip bilan ko'chirilgan,
  sinxronlanmaydi). U o'zgarmaydi, shuning uchun bu muammo emas.
- "Joriy holat" yozuvlarida mashq qaysi kompyuterda bajarilganini ham belgilang.

### macOS (MacBook) — tekshirilgan (2026-10-21)

- MacBook Pro, **Apple M1 Pro** (arm64, 16 yadroli GPU), macOS 27.0.1. Ekran: Liquid Retina XDR
  (3456 × 2234, Retina, ProMotion 120 Hz gacha). zsh, VS Code + Claude extension.
- Asboblar: Apple clang 21.0 (Xcode), cmake 4.4.3, ninja 1.13.2, glslc (shaderc 2026.4),
  spirv-tools, glslang.
- Vulkan **Homebrew** orqali (LunarG SDK yo'q, `VULKAN_SDK` o'rnatilmagan): `glfw`, `molten-vk`,
  `vulkan-loader`, `vulkan-headers`, `vulkan-validationlayers`, `vulkan-tools`,
  `vulkan-utility-libraries`, `shaderc`, `glslang`, `spirv-tools`, `spirv-headers`. CMake ularni
  `/opt/homebrew` dan topadi (`/opt/homebrew/lib/libvulkan.dylib`, `glfw3_DIR=/opt/homebrew/lib/cmake/glfw3`).
- `vulkaninfo`: instance 1.4.357. `VK_LAYER_KHRONOS_validation` 1.4.357 **qo'shimcha sozlamasiz**
  topiladi (`VK_ADD_LAYER_PATH` kerak emas). GPU0: `Apple M1 Pro`, `INTEGRATED_GPU`, MoltenVK 1.4.2,
  `apiVersion` 1.4.357. `dynamicRendering`, `synchronization2`, `shaderDemoteToHelperInvocation`
  — hammasi `true`. `VK_KHR_portability_subset` va `VK_KHR_swapchain` bor.
- Queue family'lar: **4 ta**, har birida 1 ta navbat, hammasi GRAPHICS | COMPUTE | TRANSFER.
- Surface (`VK_EXT_metal_surface`): 60 ta format (boshi `B8G8R8A8_UNORM` + `SRGB_NONLINEAR`),
  present mode'lar faqat **FIFO va IMMEDIATE** (MAILBOX yo'q), `minImageCount` 2,
  `maxImageCount` 3, compositeAlpha: OPAQUE, POST_MULTIPLIED, INHERIT.
- `.vscode/c_cpp_properties.json` da `"Mac"` konfiguratsiyasi bor (`macos-clang-arm64`,
  `build/compile_commands.json`).
- Loyiha Mac'da Debug'da build bo'ladi (`build/app`, 2026-10-21). 04-dars portability qismi
  (`#ifdef __APPLE__`) va 05-darsdagi `VK_KHR_portability_subset` ishlaydi.

macOS'da Vulkan bo'yicha tuzoqlar (darslarda hisobga oling):
- Instance uchun `VK_KHR_portability_enumeration` + `VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR`
  (usiz `vkCreateInstance` → -9). Foydalanuvchi kodida **bor**.
- Device: `VK_KHR_portability_subset` yoqilishi majburiy (`VUID-VkDeviceCreateInfo-pProperties-04451`).
  Makrosi `vulkan_beta.h` da, shuning uchun matn bilan yoziladi. Foydalanuvchi kodida **bor**
  (`createLogicalDevice`, device kengaytmalari ro'yxatidan qidiradi).
- Apple Silicon — `INTEGRATED_GPU` va **unified memory**. 03-darsdagi "RAM va VRAM alohida" faqat
  diskret GPU uchun to'g'ri: 09-darsda (buffer'lar) buni alohida ayting (`HOST_VISIBLE | DEVICE_LOCAL`).
- Retina: oyna 1280 × 720 point = 2560 × 1440 piksel. 06-darsda extent uchun `currentExtent` /
  `glfwGetFramebufferSize` ishlatildi. Viewport/scissor va sichqoncha koordinatalarida (16-dars)
  ham shu farqni eslang: `glfwGetCursorPos` point'da qaytaradi.
- Present mode'lar: MAILBOX yo'q. Vsync o'chirilsa, Mac'da IMMEDIATE tanlanadi.
- `discard` → `shaderDemoteToHelperInvocation`: MoltenVK 1.4.2 da `true`.
- Buyruqlar: zsh, `./build/app`, muhit o'zgaruvchilari `export` bilan. 01–04-darslardagi
  PowerShell/MSYS2 buyruqlari Mac'ga to'g'ri kelmaydi.
- `../fluid_sim` namunasi Mac'da ham bor (`/Users/mac/dev/VULKAN_SIMULATIONS/fluid_sim`), lekin faqat
  Windows'da sinalgan: portability yo'q (`vk_core.cpp`), `executableDir()` `/proc/self/exe` ga tayanadi.
  Namuna sifatida o'qish uchun yaroqli.

### Windows PC — tekshirilgan

- Windows 11, PowerShell. VS Code + Claude extension.
- MSYS2 MinGW64: `C:\msys64\mingw64\bin` PATH'da bor. U yerda: g++ 16.2, cmake 4.4, ninja, glslc,
  glslangValidator, spirv-val, GLFW 3 (dynamic, `glfw3.dll`), Vulkan headers (1.4.357) va loader.
- Validation layer: `C:\msys64\mingw64\bin\VkLayer_khronos_validation.dll` (+ `.json` manifest).
  Foydalanuvchi muhitida `VK_ADD_LAYER_PATH=C:\msys64\mingw64\bin` **User** darajasida doimiy
  o'rnatilgan, shuning uchun layer qo'shimcha sozlamasiz topiladi. `VK_LAYER_PATH` o'rnatilmagan.
  Registry'da explicit layer yozuvlari yo'q. Loader'ning `EOSOverlayVkLayer-Win64.json`
  topilmadi degan xabari zararsiz (Epic Games overlay'i), e'tibor bermaslik kerak.
- `libvulkan-1.dll.a` (MSYS2 import kutubxonasi) kengaytma funksiyalarini (masalan,
  `vkCreateDebugUtilsMessengerEXT`) eksport qilmaydi: ular faqat `vkGetInstanceProcAddr` orqali olinadi.
- GPU: NVIDIA GeForce RTX 3060 (Vulkan 1.4 drayver), monitor 165 Hz.
- `../fluid_sim` da uchragan va darslarda ogohlantirish kerak bo'lgan tuzoq: `--target-env=vulkan1.3`
  bilan kompilyatsiya qilinganda GLSL'dagi `discard` `OpDemoteToHelperInvocation` ga aylanadi.
  Bunga `VkPhysicalDeviceVulkan13Features::shaderDemoteToHelperInvocation` ni yoqish kerak.

## Joriy holat

*(Claude har bir darsdan keyin yangilaydi.)*

- **2026-10-16:** `docs/00_yol_xaritasi.md`, `01_muhit_va_kompilyatsiya.md`,
  `02_oyna_va_asosiy_sikl.md`, `lugat.md` va shu `CLAUDE.md` yozildi. Foydalanuvchi hali kod
  yozishni boshlamagan. Papkada faqat `docs/` va `CLAUDE.md` bor.
- **2026-10-17:** 01–02 tekshirildi. Hamma mashqlar bajarilgan: `CMakeLists.txt` (glfw3,
  `-Wall -Wextra`, o'zi qo'shgan `CMAKE_EXPORT_COMPILE_COMMANDS` + `.vscode/c_cpp_properties.json`),
  `main.cpp` (oyna, sikl, dt, FPS, Esc, Space pauza global `isPaused` bilan, framebuffer size
  callback, kursor, `distance` tajribasi, `glfwWaitEvents`), `vec2` (`add`, `scale`, `length`,
  `subtract`, `dot`). Kamchiliklar 03-darsning 1-qadamida tuzatishga berildi: `glfwWaitEvents`
  va `distance` qolib ketgan, FPS 0.5 s oraliqda `frameCount` sifatida chiqadi (yarmi ko'rinadi).
  Foydalanuvchi K&R qavs uslubini (`int main() {`) va `std::endl` ishlatadi, 03-dars shunga
  moslandi. "O'zingizni tekshiring" savollariga javob yozmadi.
  `docs/03_gpu_va_vulkan.md` yozildi, `lugat.md` ga "GPU va Vulkan" bo'limi qo'shildi, 02 → 03
  havolasi ulandi.
- **Muhit faktlari (2026-10-17):** `vulkaninfo.exe` System32 da bor (NVIDIA drayveri bilan kelgan).
  Loader 1.4.341, header 1.4.357, drayver 610.47. `app.exe` System32 dagi `vulkan-1.dll` ni
  yuklaydi (`PATH` dagi MSYS2 nusxasini emas).
- **2026-10-18:** 03 tekshirildi. Bajarilgan: 02 commit qilingan (`fb503f8`), `glfwPollEvents`
  qaytarilgan, `distance` o'chirilgan, FPS `frameCount / (now - fpsTimer)`, intelliSenseMode
  `windows-gcc-x64`, `#define GLFW_INCLUDE_VULKAN`, `printVulkanVersion()` (makros o'rniga o'zi
  yozgan maskalar bilan), `glfwVulkanSupported`, kengaytmalar ro'yxatini chiqarish. Kamchiliklar
  (04-darsning 1–2-qadamlariga kiritildi): (a) 5-mashqdan keyin `Vulkan::Vulkan` qaytarilmagan,
  build linker'da yiqiladi, `build/app.exe` yo'q; (b) maskalarda birlar soni bittadan kam
  (`0b111111` 6 ta, `0b111111111` 9 ta, `0b11111111111` 11 ta; kerak 7/10/12), 1.4.341 uchun
  tasodifan to'g'ri chiqadi; (c) `throw std::runtime_error` dan keyin `glfwTerminate(); return 1;`
  bajarilmaydigan kod, `catch` yo'q (halokatli tugaydi). 2, 4, 5, 6-mashqlar natijalari va
  "O'zingizni tekshiring" javoblari yozilmadi.
  `docs/04_instance_va_validation_layer.md` yozildi. Tanlangan yondashuv: kod hali `main.cpp` da,
  erkin funksiyalar (`vkCheck`, `isLayerAvailable`, `getRequiredExtensions`, `debugCallback`,
  `makeDebugMessengerInfo`, `createInstance`, `createDebugMessenger`, `destroyDebugMessenger`),
  `main` → `try { run(); } catch (const std::exception&)`. Global `const bool enableValidation`.
  Xatolar exception bilan; `throw` bo'lganda tozalash o'tkazib yuboriladi (darsda ochiq aytilgan,
  "keyingi darslarda hal qilamiz" deb va'da berilgan → destructor/RAII). Callback xabari
  `[Vulkan XATO/OGOHLANTIRISH]` formatida, `std::cerr` ga. Lug'atga C++ va Vulkan atamalari qo'shildi.
- **2026-10-19:** 04 tekshirildi. Foydalanuvchi darsdagi erkin funksiyalar o'rniga o'zi
  **klass** tuzilmasini tanladi (vulkan-tutorial.com dagi `HelloTriangleApplication` uslubi).
  **Bundan keyingi darslar shu tuzilmaga tayanadi:**
  - `src/main.cpp`: `FluidSimulation2D fluidSimulation2D;` `try` dan **oldin** e'lon qilingan,
    `try { fluidSimulation2D.run(); } catch (const std::exception& e)` → `utils::createErrorLog`.
  - `src/fluid_sim_2d.h/.cpp`: `class FluidSimulation2D`. `run()` → `initWindow()`, `initVulkan()`,
    `mainLoop()`, `cleanup()`. A'zolar: `window`, `instance`, `debugMessenger`. Metodlar:
    `createInstance`, `checkValidationLayerSupport`, `getRequiredExtensions`, `checkRequiredExtensions`
    (o'zi qo'shgan), `makeDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT&)`,
    `setupDebugMessenger`, `static debugCallback`. Anonim namespace'da: `WIDTH`, `HEIGHT`,
    `validationLayers` (vector), `enableValidationLayers` (`#ifdef NDEBUG`).
  - `src/utils.h/.cpp`: `namespace utils` — `CreateDebugUtilsMessengerEXT` /
    `DestroyDebugUtilsMessengerEXT` (proxy), `vkCheck`, `VK_CHECK` makrosi, `printVulkanVersion`,
    `isLayerAvailable`, `printAvailableLayers`, `createInfoLog` / `createErrorLog` /
    `createWarningLog` (rangli). `src/color.h`: ANSI ranglar (`color::QIZIL`, `YASHIL`...).
    Header'lar `.h` kengaytmali (faqat `vec2.hpp` boshqacha). Xabarlar `utils::createInfoLog` bilan.
  - Mashqlar: `printAvailableLayers`, `#ifdef NDEBUG` + `build-release/` (`.gitignore` da bor),
    `VK_CHECK`, INFO/VERBOSE sinab ko'rilgan. "Ataylab xato", `VK_ADD_LAYER_PATH=""`, linker xatosi
    mashqlari natijasi yozilmagan.
  - Kamchiliklar (05-darsning 1-qadamiga berildi): `glfwCreateWindow` `nullptr` tekshiruvi yo'q;
    `printVulkanVersion()` `bool`, `initWindow` da `if (!...) return;` (yolg'on yo'l, `window`
    nullptr qolishi mumkin); `VK_CHECK` `namespace utils` ichida va `vkCheck` ni to'liq nomsiz
    chaqiradi (makros namespace'ni bilmaydi → `utils` dan tashqarida kompilyatsiya xatosi);
    `vkCreateInstance` va messenger natijalari `VK_CHECK` siz (xato kodi yo'qoladi);
    `isLayerAvailable` va `checkValidationLayerSupport` takror; `glfwVulkanSupported` olib
    tashlangan; `cleanup` tartibi qat'iy teskari emas (window birinchi yo'q qilinadi).
  - **VS Code muammosi:** `.vscode/settings.json` da `"C_Cpp.errorSquiggles": "disabled"` (user
    sozlamasidagi `"enabled"` ni bekor qiladi). `c_cpp_properties.json` →
    `build/compile_commands.json`, unda 4 ta `.cpp` bor. CMake Tools 1.24 va cpptools 1.34 o'rnatilgan.
    cpptools g++ ning `-Wall -Wextra` ogohlantirishlarini ko'rsatmasligi tushuntirildi (CMake Tools
    build yoki `$gcc` problem matcher).
  `docs/05_physical_va_logical_device.md` yozildi: yangi a'zolar `physicalDevice`, `device`,
  `queueFamilyIndex`, `queue`; metodlar `pickPhysicalDevice` (skelet, o'zi yozadi),
  `isDeviceSuitable(gpu)`, `findQueueFamily(gpu)` → `std::optional<uint32_t>`,
  `createLogicalDevice` (features13 `pNext` orqali: `dynamicRendering`, `synchronization2`;
  `shaderDemoteToHelperInvocation` faqat so'raladi, 08-darsda yoqiladi). Destructor `cleanup()` ni
  chaqiradi, `run()` dan `cleanup()` olib tashlanadi, `cleanup` har bir handle'ni tekshiradi va
  null qiladi, tartib: device → messenger → instance → window → glfwTerminate. Nusxalash
  `= delete`, `FluidSimulation2D() = default`.
- **Muhit faktlari (2026-10-19):** kompyuterda bitta GPU. RTX 3060 queue family'lari: 0 (16 ta,
  GRAPHICS|COMPUTE|TRANSFER|SPARSE), 1 (2, TRANSFER), 2 (8, COMPUTE|TRANSFER), 3 (VIDEO_DECODE),
  4 (VIDEO_ENCODE), 5 (OPTICAL_FLOW_NV). `dynamicRendering`, `synchronization2`,
  `shaderDemoteToHelperInvocation` — hammasi `true`.
- **2026-10-19 (kech):** `.vscode/settings.json` (errorSquiggles disabled) o'chirildi. Hammasi
  `aea9f61` ("04 complete") ga commit qilingan va push qilingan. Foydalanuvchi bundan buyon
  **Windows va Mac'da navbatma-navbat** ishlaydi ("Ikki kompyuterda ishlash qoidalari"). 05-darsni
  qaysi birida bajarishi noma'lum. 05-darsga macOS eslatmalari qo'shildi (portability subset,
  boshqacha GPU va queue family'lar).
- **2026-10-21 (Mac):** Mac'dagi birinchi sessiya. Mac muhiti tekshirildi va "Muhit → macOS"
  qayta yozildi (M1 Pro, Homebrew Vulkan, hammasi bor). 05-dars **Mac'da** bajarilgan, build
  bo'ladi. O'zgarishlar hali commit qilinmagan (`20d609c` dan keyin 5 ta fayl o'zgargan).
  05 tekshirildi. Yaxshi: 04-dars tuzatishlari (nullptr tekshiruvi, `printVulkanVersion` `void`,
  `VK_CHECK` → `::utils::vkCheck` namespace'dan tashqarida, instance/messenger `VK_CHECK` bilan),
  destructor + xavfsiz `cleanup` (tartib to'g'ri), `= delete`/`= default`, `pickPhysicalDevice`
  (ikki chaqiruv, `count == 0`), `findQueueFamily` (`== required`), `isDeviceSuitable`,
  `createLogicalDevice` + `VK_KHR_portability_subset` (device kengaytmalari ro'yxatidan, o'zi
  yozgan), 2-mashq (`utils::printQueueFamilies`, rangli), 3-mashq (shaderDemote chiqariladi),
  5-mashq (nusxa izohi `main.cpp` da qolgan). Kamchiliklar (06-darsning 1-qadamiga berildi):
  diskret GPU afzal ko'rilmaydi (`physicalDevice == VK_NULL_HANDLE` sharti faqat birinchisini
  oladi) va `mos`/`mos emas` chiqmaydi; `cleanup` da `debugMessenger` null qilinmaydi;
  `printQueueFamilies` vektorni nusxa bilan oladi va `findQueueFamily` ichida bo'lgani uchun
  ikki marta chiqadi; `utils::isLayerAvailable` ishlatilmaydi; 1-mashq (GPU pasporti) bajarilmagan.
  4-mashq, "ataylab xato" natijalari va "O'zingizni tekshiring" javoblari yozilmagan. Dastur
  chiqishini (konsol) foydalanuvchi ko'rsatmagan.
  `docs/06_surface_va_swapchain.md` yozildi. Tanlovlar: `struct SwapchainSupport` (klassdan oldin,
  header'da); anonim namespace'da `deviceExtensions` (`VK_KHR_SWAPCHAIN_EXTENSION_NAME`) va
  `constexpr bool enableVsync = true`; yangi a'zolar `surface`, `swapchain`, `swapchainImageFormat`,
  `swapchainExtent`, `swapchainImages`, `swapchainImageViews`; metodlar `createSurface`,
  `checkDeviceExtensionSupport(gpu)` (o'zi yozadi), `querySwapchainSupport(gpu)`,
  `chooseSurfaceFormat` (`B8G8R8A8_UNORM` + `SRGB_NONLINEAR`, namunadagidek UNORM),
  `choosePresentMode` (o'zi yozadi: vsync → FIFO, aks holda MAILBOX → IMMEDIATE → FIFO),
  `chooseExtent` (`currentExtent`, aks holda `glfwGetFramebufferSize` + `std::clamp`),
  `createSwapchain` (min+1, EXCLUSIVE, OPAQUE, `oldSwapchain` null), `createImageViews`.
  `findQueueFamily` ga present support qo'shildi (surface `pickPhysicalDevice` dan oldin).
  `cleanup` tartibi: view'lar → swapchain → device → surface → messenger → instance → window →
  glfwTerminate. **Oyna o'lchami o'zgarishi va swapchain'ni qayta yaratish 07-darsga qoldirildi**
  (06 dagi 4-mashq: framebuffer o'lchami `swapchainExtent` dan farq qilsa ogohlantirish).
  Lug'atga C++ (`const T&`, `resize`, `std::clamp`, `std::numeric_limits`) va Vulkan (MoltenVK,
  portability subset, WSI, present, surface format, present mode, extent, point/piksel, image,
  image view, subresource range, sharing mode...) atamalari qo'shildi. 05 → 06 havolasi ulandi.
- **2026-10-21 (Mac, keyinroq):** 06-darsning 1-qadami (05 tuzatishlari) tekshirildi. Tuzatilgan:
  `debugMessenger` null qilinadi, `main.cpp` dagi izoh o'chirilgan, `printQueueFamilies(VkPhysicalDevice)`
  endi o'zi so'raydi va faqat tanlangan GPU uchun bir marta chiqadi. Qolgan: `utils::isLayerAvailable`.
  Foydalanuvchi **`isDeviceSuitable` o'rniga ball tizimini** o'ylab topdi: `int rateGpu(gpu)`
  (apiVersion < 1.3 yoki family yo'q → 0; `dynamicRendering` +100000, `synchronization2` +100000,
  diskret +100000; `pickPhysicalDevice` da `rate >= 200000 && rate > highScore`). Xato tushuntirildi:
  majburiy shartlar va afzallik bitta yig'indida → diskret + bitta feature = 200000 ham o'tadi →
  `vkCreateDevice` -8. Tavsiya: majburiy shartlar `isDeviceSuitable` ga qaytsin (rateGpu boshida
  `if (!isDeviceSuitable(gpu)) return 0;`), ball faqat afzallik uchun, chegara `score > 0`. Shunda
  06-darsdagi `isDeviceSuitable` qadamlari (present, `VK_KHR_swapchain`, swapchain support) o'zgarishsiz
  ishlaydi. Foydalanuvchi tuzatishi kerak.
- **Keyingi qadam:** foydalanuvchi 06-darsni bajarib qaytadi (Mac yoki Windows'da). Unda:
  0. Git sinxronligi. Windows'da bo'lsa: Mac'dagi 05/06 kodi Windows'da (MinGW g++) ham build
     bo'lishini va 06 natijalarini (formatlar, present mode'lar, min/max image count) so'rang.
     06-darsdagi Windows jadvallari ("odatda ...") taxminiy: haqiqiy natija bilan to'g'rilang.
  1. Tekshirish: 1-qadam tuzatishlari (diskret afzalligi, `debugMessenger` null, `const&`,
     `printQueueFamilies` joyi, `isLayerAvailable`), surface tartibi (`pickPhysicalDevice` dan
     oldin), present support, `checkDeviceExtensionSupport`, `enabledExtensions = deviceExtensions`,
     `choosePresentMode`, extent Mac'da 2560x1440 chiqyaptimi, `cleanup` tartibi, mashqlar va uch
     "ataylab xato" natijalari. Konsol chiqishini so'rang.
  2. Keyin `docs/07_command_buffer_va_sinxronizatsiya.md`: command pool/buffer, fence va
     semaphore'lar (frames in flight), acquire → record (dynamic rendering, `vkCmdBeginRendering`,
     clear color, image layout o'tishlari synchronization2 bilan: `vkCmdPipelineBarrier2`) →
     submit (`vkQueueSubmit2`) → present. Oyna rangga bo'yaladi. Swapchain'ni qayta yaratish
     (`VK_ERROR_OUT_OF_DATE_KHR` / `SUBOPTIMAL`, framebuffer size callback + flag, minimize'da 0x0
     kutish, `vkDeviceWaitIdle`). Mac'da present mode farqi (MAILBOX yo'q) va 120 Hz ProMotion.
