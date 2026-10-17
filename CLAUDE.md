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
- **Keyingi qadam:** foydalanuvchi *"04-dars tayyor"* deb qaytadi. Unda:
  1. Tekshirish: 03 tuzatishlari (`Vulkan::Vulkan`, maskalar → makroslar, `<cstdlib>` olib
     tashlangan), `run()` + `try/catch`, `vkCheck`, ikki chaqiruvli `isLayerAvailable` (`strcmp`),
     `getRequiredExtensions` (`push_back` sikli), `debugCallback` (`VKAPI_ATTR`/`VKAPI_CALL`,
     `VK_FALSE`), `pNext` ga `debugInfo`, `vkGetInstanceProcAddr`, tozalash tartibi
     (window → messenger → instance → glfwTerminate). "Ataylab xato" (messenger'ni yo'q qilmaslik →
     `VUID-vkDestroyInstance-instance-00629`) ko'rilganini so'rash. Mashqlar: layer ro'yxati va
     `VK_ADD_LAYER_PATH=""`, to'g'ridan-to'g'ri chaqiruv → linker xatosi, `#ifdef NDEBUG` +
     `build-release/` (`.gitignore` ga qo'shilganmi), `VK_CHECK` (`#x`), INFO/VERBOSE.
  2. Keyin `docs/05_physical_va_logical_device.md` (yoki shunga yaqin nom): `vkEnumeratePhysicalDevices`
     (ikki chaqiruv), `VkPhysicalDeviceProperties` (nomi, `apiVersion` ≥ 1.3, deviceType), queue
     family'lar (graphics + compute; present 06-darsda), `VkDeviceCreateInfo`,
     `VkPhysicalDeviceVulkan13Features` (`dynamicRendering`, `synchronization2`, keyinroq
     `shaderDemoteToHelperInvocation`) `pNext` orqali, `vkGetDeviceQueue`. `main.cpp` juda
     kattalashadi: Vulkan kodini alohida faylga (`vulkan_context.hpp/.cpp` yoki shunga o'xshash)
     va struct'ga ko'chirishni shu darsda boshlash, destructor bilan tozalash muammosini hal qilish.
