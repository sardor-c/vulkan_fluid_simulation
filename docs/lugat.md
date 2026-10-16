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
| **Vulkan** | GPU bilan ishlash uchun zamonaviy, past darajali grafik va hisoblash API (03) |
