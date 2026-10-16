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
- Validation layer: `C:\msys64\mingw64\bin\VkLayer_khronos_validation.dll`. Yoqish uchun
  `$env:VK_LAYER_PATH="C:\msys64\mingw64\bin"` kerak. Loader'ning `EOSOverlayVkLayer-Win64.json`
  topilmadi degan xabari zararsiz (Epic Games overlay'i), e'tibor bermaslik kerak.
- GPU: NVIDIA GeForce RTX 3060 (Vulkan 1.4 drayver), monitor 165 Hz.
- `../fluid_sim` da uchragan va darslarda ogohlantirish kerak bo'lgan tuzoq: `--target-env=vulkan1.3`
  bilan kompilyatsiya qilinganda GLSL'dagi `discard` `OpDemoteToHelperInvocation` ga aylanadi.
  Bunga `VkPhysicalDeviceVulkan13Features::shaderDemoteToHelperInvocation` ni yoqish kerak.

## Joriy holat

*(Claude har bir darsdan keyin yangilaydi.)*

- **2026-10-16:** `docs/00_yol_xaritasi.md`, `01_muhit_va_kompilyatsiya.md`,
  `02_oyna_va_asosiy_sikl.md`, `lugat.md` va shu `CLAUDE.md` yozildi. Foydalanuvchi hali kod
  yozishni boshlamagan. Papkada faqat `docs/` va `CLAUDE.md` bor.
- **Keyingi qadam:** foydalanuvchi 01 va 02-darslarni tugatib, VS Code'dagi Claude extension orqali
  shu papkadan qaytadi. Unda:
  1. 01–02 natijalarini tekshirish: `CMakeLists.txt` (`find_package(glfw3)`, `-Wall -Wextra`),
     `src/main.cpp` (GLFW oyna, asosiy sikl, delta time, FPS sarlavhada, Esc callback),
     `src/vec2.hpp/.cpp` (`add`, `scale`, `length` hamda mashqdagi `subtract`, `dot`).
     Mashqlar: Space bilan pauza, framebuffer size callback, kursor pozitsiyasi, delta time
     tajribasi, `glfwWaitEvents` savoli.
  2. Keyin `docs/03_gpu_va_vulkan.md` ni yozish. Asosan nazariya: CPU va GPU farqi (parallelizm),
     API → loader → driver → GPU zanjiri, Vulkan obyektlari xaritasi (instance, physical/logical
     device, queue, surface, swapchain, command buffer, pipeline, buffer, descriptor), nega Vulkan
     shunchalik batafsil (explicit). Kichik amaliyot: `find_package(Vulkan)`, `Vulkan::Vulkan` ni
     ulash va `vkEnumerateInstanceVersion` bilan Vulkan versiyasini chiqarish.
  3. `02_oyna_va_asosiy_sikl.md` pastidagi "Keyingi dars" ni 03-darsga havola qilish.
