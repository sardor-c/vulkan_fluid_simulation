# Lug'at

Darslarda uchraydigan atamalar. Har bir yangi dars bilan to'ldirib boriladi.
Qavs ichida — atama birinchi marta tushuntirilgan dars.

## C++ va build

| Atama | Ma'nosi |
|---|---|
| **Compiler** (kompilyator) | `.cpp` faylni mashina kodiga (object file) tarjima qiluvchi dastur. Bizda `g++` (01) |
| **Preprocessor** | Kompilyatsiyadan oldin `#include`, `#define` kabi `#` buyruqlarini bajaradi (01) |
| **Linker** | Object fayllar va kutubxonalarni bitta `.exe` ga yig'adi, funksiya manzillarini bog'laydi (01) |
| **Object file** (`.o`) | Bitta `.cpp` faylning kompilyatsiya natijasi. Hali to'liq dastur emas (01) |
| **Header** (`.hpp`) | E'lonlar (declaration) yoziladigan fayl. Boshqa fayllar uni `#include` qiladi (01) |
| **Declaration** (e'lon) | "Shunday funksiya/tip mavjud" degan ma'lumot, tanasiz (01) |
| **Definition** (ta'rif) | Funksiyaning haqiqiy kodi (tanasi) (01) |
| **Library** (kutubxona) | Tayyor, kompilyatsiya qilingan kod. Static (`.a`) yoki dynamic (`.dll`) (01) |
| **CMake** | `CMakeLists.txt` dan build fayllarini yaratuvchi tizim ("me'mor") (01) |
| **Ninja** | CMake yaratgan reja bo'yicha kompilyatorni chaqiruvchi tez build asbobi ("quruvchi") (01) |
| **Target** | CMake'dagi build qilinadigan narsa: dastur yoki kutubxona (masalan, `app`, `glfw`) (01) |
| **Configure / Build** | CMake'ning 2 bosqichi: reja tuzish (`cmake -S . -B build`) va yasash (`cmake --build build`) (01) |
| **Debug / Release** | Build turlari: xato topish uchun qulay / tez ishlaydigan (01) |
| **Warning** (ogohlantirish) | Kompilyator "bu yer shubhali" deyapti. Xato emas, lekin ko'pincha xatoga olib keladi (01) |
| **Namespace** | Nomlar maydoni. Masalan, standart kutubxona `std::` ichida (01) |
| **struct** | Bir nechta qiymatni bitta tipga birlashtirish (masalan, `Vec2 { x, y }`) (01) |
| **float / double** | Kasr sonlar: 32 bit (~7 xona aniqlik) / 64 bit (~15 xona). GPU asosan `float` ishlatadi (01) |
| **Pointer** (ko'rsatkich) | Xotiradagi manzilni saqlovchi o'zgaruvchi: `int* p = &a;` (02) |
| **`nullptr`** | "Hech narsaga ko'rsatmaydigan" pointer (02) |
| **Handle** (tutqich) | Ichi yashirin obyektga ishora. Masalan, `GLFWwindow*`, `VkDevice` (02) |
| **Lambda** | Joyida yoziladigan nomsiz funksiya: `[](int x) { return x * 2; }` (02) |
| **Callback** | Biror kutubxonaga berib qo'yiladigan funksiya; hodisa bo'lganda kutubxona uni o'zi chaqiradi (02) |
| **`#define`** | Preprocessor'ga nom (makros yoki "bayroq") e'lon qilish. Masalan, `#define GLFW_INCLUDE_VULKAN` (03) |
| **Makros** (macro) | Preprocessor kompilyatsiyadan oldin matn sifatida almashtiradigan nom. Masalan, `VK_API_VERSION_MAJOR(v)` (03) |
| **`uint32_t`** | Manfiy bo'lmaydigan, aniq 32 bitli butun son (`<cstdint>`). Vulkan hamma joyda shuni ishlatadi (03) |
| **Bit** | Ikkilik xona: 0 yoki 1. 32 bitli son — 32 ta shunday xona (03) |
| **`>>` / `&`** | Bitlarni o'ngga surish / niqob (mask) bilan kerakli bitlarni ajratib olish (03) |
| **Hex** (`0x7F`) | O'n oltilik sanoq tizimidagi son. Bitlar bilan ishlashda qulay: har bir raqam 4 bit (03) |
| **Imported target** (`Vulkan::Vulkan`) | Paket e'lon qilgan tayyor CMake target: header'lar va kutubxona birga (03) |
| **`std::vector`** | O'lchami o'zgara oladigan massiv: `push_back`, `size()`, `data()`, `[i]` (04) |
| **Reference** (`Tip&`) | O'zgaruvchining ikkinchi nomi, nusxa emas. E'londa tip yonida keladi; ifodadagi `&x` (manzil) bilan adashtirmang (04) |
| **Range-for** | Ro'yxatning har bir elementi bo'ylab yurish: `for (const T& x : list)` (04) |
| **C satri** (`char[]`, `const char*`) | Oxiri 0 bilan tugaydigan harflar massivi. `==` manzilni solishtiradi, matnni `std::strcmp` solishtiradi (04) |
| **Exception** | Favqulodda holat signali. `throw` funksiyani to'xtatadi, eng yaqin `catch` ushlaydi, `catch` bo'lmasa dastur halokatli tugaydi (04) |
| **`try` / `catch`** | Exception bo'lishi mumkin bo'lgan blok / uni ushlaydigan blok (04) |
| **`std::runtime_error`** | Matnli xabar bilan standart exception. Xabar `e.what()` bilan olinadi (04) |
| **`static_cast`** | Bir tipni boshqasiga aniq aylantirish, masalan `size_t` → `uint32_t` (04) |
| **`reinterpret_cast`** | "Bu manzilga boshqa tip deb qara". Funksiya pointer'larini aylantirishda kerak (04) |
| **`auto`** | Tipni kompilyator o'zi aniqlaydi (04) |
| **Funksiya pointer'i** | Funksiya manzilini saqlovchi o'zgaruvchi. Uni funksiyaning o'zidek chaqirsa bo'ladi (04) |
| **`->`** | Pointer orqali struct maydoniga murojaat: `p->x` = `(*p).x` (04) |
| **`\|`** (bitli OR) | Bir nechta bit-bayroqni birga yoqish: `WARNING_BIT \| ERROR_BIT` (04) |
| **`#ifdef` / `NDEBUG`** | Makros e'lon qilingan bo'lsagina kodni qoldirish. `NDEBUG` Release build'da e'lon qilinadi (04) |
| **Stringify** (`#x`) | Makros ichida argumentni matnga aylantirish: `VK_CHECK(x)` → `"x"` (04) |
| **Klass** (`class`) | Ma'lumot (a'zo o'zgaruvchilar) va funksiyalarni bitta tipga birlashtirish. `private` a'zolar faqat klass ichidan ko'rinadi (05) |
| **Destructor** (`~Klass()`) | Obyekt umri tugaganda C++ o'zi chaqiradigan funksiya. Exception paytida ham chaqiriladi (05) |
| **RAII** | Resurs obyekt bilan birga olinadi va destructor'da qaytariladi: tozalashni unutib bo'lmaydi (05) |
| **`= delete`** | Funksiyani taqiqlash. Masalan, nusxa konstruktorini o'chirib, klassni nusxalab bo'lmaydigan qilish (05) |
| **`= default`** | Kompilyator yaratadigan standart variantni aniq so'rash, masalan `Klass() = default;` (05) |
| **`std::optional`** | Qiymat bo'lishi yoki bo'lmasligi mumkin bo'lgan quti: `has_value()`, `value()`, `std::nullopt` (05) |

## Simulatsiya va grafika

| Atama | Ma'nosi |
|---|---|
| **Vektor** | Yo'nalish va kattalikka ega miqdor (pozitsiya, tezlik). 2D'da `(x, y)` (01) |
| **Event** (hodisa) | OS'dan kelgan xabar: tugma bosildi, sichqoncha siljidi, oyna o'zgardi (02) |
| **Main loop** (asosiy sikl) | Input → update → render ni har kadrda takrorlovchi sikl (02) |
| **Frame** (kadr) | Asosiy siklning bitta aylanishi (02) |
| **FPS** | Frames per second: soniyadagi kadrlar soni (02) |
| **Delta time** (`dt`) | Oxirgi kadrdan beri o'tgan vaqt (soniya). Harakat FPS'ga bog'liq bo'lmasligi uchun kerak (02) |
| **Polling** | Holatni har kadrda so'rab turish: "tugma hozir bosilganmi?" (02) |
| **Vsync** | Kadrlarni monitor yangilanish chastotasiga moslash (07-darsda batafsil) (02) |
| **GLFW** | Oyna, klaviatura va sichqoncha bilan ishlash uchun kutubxona (02) |

## GPU va Vulkan

| Atama | Ma'nosi |
|---|---|
| **CPU** | Markaziy protsessor: kam (6–16), lekin juda kuchli yadrolar. Ketma-ket, murakkab ishlar uchun (03) |
| **GPU** | Videoprotsessor: minglab oddiy yadrolar. Bir xil ishni juda ko'p ma'lumotga parallel qo'llaydi (03) |
| **Yadro** (core) | Protsessorning mustaqil ishlay oladigan bitta "ishchi"si (03) |
| **Parallelizm** | Ko'p ishni bir vaqtda bajarish. SPH'da har bir zarracha alohida hisoblanadi, shuning uchun GPU'ga mos (03) |
| **RAM / VRAM** | CPU xotirasi / GPU'ning o'z xotirasi. Ular alohida (03) |
| **PCIe** | CPU va GPU'ni bog'lovchi shina. VRAM'dan ancha sekin, shuning uchun ma'lumot ko'chirishni kamaytiramiz (03) |
| **Asinxronlik** | CPU buyruqlarni GPU'ga uzatib, natijani kutmasdan ishini davom ettiradi (03) |
| **Sinxronizatsiya** | CPU va GPU (yoki GPU ishlari) bir-birini to'g'ri vaqtda kutishini ta'minlash (07-darsda batafsil) (03) |
| **Vulkan** | GPU bilan ishlash uchun zamonaviy, past darajali grafik va hisoblash API. Kutubxona emas, spetsifikatsiya (03) |
| **Spetsifikatsiya** | Funksiyalar qanday ishlashi kerakligini yozib qo'ygan standart. Kodni drayver yozadi (03) |
| **Khronos Group** | Vulkan, OpenGL va boshqa standartlarni ishlab chiqadigan tashkilot (03) |
| **OpenGL** | Vulkan'dan oldingi grafik API. Ko'p narsani drayver o'zi hal qiladi (03) |
| **Explicit** (aniq) | Hamma narsani dasturchi o'zi aytadi, drayver taxmin qilmaydi. Vulkan shunday (03) |
| **Loader** (`vulkan-1.dll`) | Dasturdan kelgan Vulkan chaqiruvlarini kerakli drayverga yo'naltiruvchi kutubxona (03) |
| **Layer** (qatlam) | Dastur bilan drayver orasiga qo'yiladigan ixtiyoriy oraliq qatlam (03) |
| **Validation layer** | Har bir Vulkan chaqiruvini tekshirib, xatolarni aytib beruvchi layer (04-darsda yoqamiz) (03) |
| **Drayver** (ICD) | GPU ishlab chiqaruvchisi yozgan Vulkan'ning haqiqiy kodi. NVIDIA'da `nvoglv64.dll` (03) |
| **Extension** (kengaytma) | Standartning asosiy qismidan tashqari imkoniyat. Nomi `KHR` yoki `EXT` bilan tugaydi (03) |
| **Header / loader / device versiyasi** | Kompilyator biladigan / o'rnatilgan loader qo'llaydigan / drayver shu GPU uchun qo'llaydigan Vulkan versiyasi (03) |
| **`VkResult`** | Vulkan funksiyasi qaytaradigan natija kodi. `VK_SUCCESS` (0) — muvaffaqiyat, manfiy — xato (03) |
| **CreateInfo** | Obyekt yaratish uchun to'ldiriladigan struct ("ariza blankasi"). Birinchi maydoni `sType` (03) |
| **`sType`** | Struct o'z turini o'zi aytadigan maydon (03) |
| **Instance** (`VkInstance`) | Dastur bilan Vulkan orasidagi aloqa. Birinchi yaratiladigan obyekt (03) |
| **Physical device** (`VkPhysicalDevice`) | Haqiqiy GPU. Yaratilmaydi, ro'yxatdan tanlanadi (03) |
| **Logical device** (`VkDevice`) | Tanlangan GPU bilan ochilgan "ish sessiyasi". Qolgan obyektlarning ko'pi shundan yaratiladi (03) |
| **Queue** (navbat) | Command buffer'lar GPU'ga shu yerdan uzatiladi (03) |
| **Surface** (`VkSurfaceKHR`) | Oynaning Vulkan'dagi "yuzasi": Vulkan qayerga chizishini biladi (03) |
| **Swapchain** | Ekranga navbat bilan chiqariladigan 2–3 ta rasm (03) |
| **Command buffer** | GPU uchun yozilgan buyruqlar ro'yxati. `vkCmd...` funksiyalari unga yozadi, darhol bajarmaydi (03) |
| **Shader** | GPU'da ishlaydigan kichik dastur (GLSL'da yoziladi, 08-dars) (03) |
| **Pipeline** | Chizish yoki hisob-kitobning to'liq sozlamasi: qaysi shader'lar, qanday holatlar (03) |
| **Buffer** (`VkBuffer`) | GPU xotirasidagi ma'lumot bo'lagi (masalan, zarrachalar pozitsiyasi) (03) |
| **Descriptor** | Shader'ga "kerakli ma'lumot qaysi buffer'da" ekanini aytadigan yorliq (03) |
| **Fence / Semaphore** | "GPU ishni tugatdi" signallari: fence CPU uchun, semaphore GPU ishlari orasida (07-darsda batafsil) (03) |
| **Dynamic rendering** | Vulkan 1.3 imkoniyati: `VkRenderPass` va `VkFramebuffer` siz chizish (03) |
| **`VkApplicationInfo`** | Dastur haqidagi blanka: nomi, versiyasi va mo'ljallangan Vulkan versiyasi (`apiVersion`) (04) |
| **`pNext` zanjiri** | Struct'ga qo'shimcha struct'larni ulash usuli. Har bir bo'g'in `sType` bilan o'zini tanishtiradi (04) |
| **`VK_NULL_HANDLE`** | "Hech narsaga ishora qilmaydigan" Vulkan handle'i (`nullptr` ning Vulkan'dagi o'xshashi) (04) |
| **`VkBool32`** | Vulkan'ning `bool` i: 32 bitli son, `VK_TRUE` yoki `VK_FALSE` (04) |
| **Explicit / implicit layer** | Faqat dastur so'raganda yuklanadigan / o'zi yuklanadigan layer (04) |
| **Layer manifest** | Layer'ning JSON "pasporti": nomi va DLL qayerdaligi (04) |
| **`VK_LAYER_PATH` / `VK_ADD_LAYER_PATH`** | Loader layer qidiradigan papka: standart qidiruvni almashtiradi / unga qo'shadi (04) |
| **`VK_EXT_debug_utils`** | Validation xabarlarini o'z funksiyamizga olish imkonini beruvchi kengaytma (04) |
| **Debug messenger** | Layer xabar yuborganda bizning callback'imizni chaqiradigan obyekt (`VkDebugUtilsMessengerEXT`) (04) |
| **VUID** | Validation xabaridagi spetsifikatsiya qoidasining raqami, masalan `VUID-vkDestroyInstance-instance-00629` (04) |
| **Severity** (daraja) | Xabar muhimligi: VERBOSE, INFO, WARNING, ERROR. Har biri bitta bit (04) |
| **Ikki chaqiruvli ro'yxat** | Avval `count` ni olish, keyin shuncha joy ajratib, ro'yxatning o'zini olish (`vkEnumerate...`) (04) |
| **`vkGetInstanceProcAddr`** | Funksiya manzilini nomi bo'yicha loader'dan so'rab olish. Kengaytma funksiyalari uchun kerak (04) |
| **`PFN_...`** | Vulkan funksiya pointer'i tipi: *Pointer to FuNction*, masalan `PFN_vkCreateDebugUtilsMessengerEXT` (04) |
| **Diskret / integrated GPU** | Alohida videokarta (o'z VRAM'i bilan) / protsessor ichidagi GPU. Biz diskretini afzal ko'ramiz (05) |
| **Properties** (xossalar) | GPU haqidagi o'zgarmas ma'lumot: nomi, turi, `apiVersion`, `limits` (05) |
| **Limits** | GPU chegaralari, masalan `maxComputeWorkGroupInvocations` (05) |
| **Features** (imkoniyatlar) | GPU qila oladigan qo'shimcha narsalar. Qo'llab-quvvatlash ≠ yoqilgan: device yaratilganda yoqiladi (05) |
| **`dynamicRendering`** | Vulkan 1.3 imkoniyati: render pass'siz chizish. Device yaratilganda yoqiladi (05) |
| **`synchronization2`** | Vulkan 1.3 imkoniyati: sinxronizatsiyaning soddaroq API'si (07-darsda) (05) |
| **Queue family** | Bir xil imkoniyatli navbatlar guruhi (GRAPHICS, COMPUTE, TRANSFER...). RTX 3060 da family 0 hammasini qiladi (05) |
| **Queue priority** | Navbatga beriladigan 0.0–1.0 ustuvorlik. Bitta navbatda ahamiyatsiz, lekin majburiy (05) |
| **`vkGetDeviceQueue`** | Device bilan birga yaratilgan navbatning handle'ini olish. Navbat alohida yo'q qilinmaydi (05) |
| **Async compute** | Hisob-kitobni alohida compute navbatida grafika bilan parallel bajarish. Murakkab optimizatsiya, bizda yo'q (05) |
