# Yo'l xaritasi: C++, Vulkan va suyuqlik fizikasi

Bu kursning maqsadi — **noldan boshlab** GPU'da ishlaydigan suyuqlik simulatsiyasini o'zingiz
qurish va shu jarayonda quyidagilarni o'rganish:

- **C++** — loyiha uchun kerak bo'ladigan qismlari, kerak bo'lganda, amalda
- **Vulkan** — GPU bilan to'g'ridan-to'g'ri gaplashish
- **Shaderlar (GLSL)** — GPU'da ishlaydigan dasturlar
- **Suyuqlik fizikasi (SPH)** — suvni minglab zarrachalar orqali hisoblash

Yakuniy natija — `fluid_sim` papkasidagi dasturga o'xshash simulatsiya, lekin har bir qatorini
**o'zingiz yozgan va tushungan** bo'lasiz.

---

## Qanday ishlaymiz

1. Men har bir dars uchun `docs/` papkasiga fayl yozaman: **nazariya + amaliy qadamlar + mashqlar**.
2. Siz darsni o'qib, kodni `fluid_sim_tutorial/` ichida **o'zingiz yozasiz**.
   Iloji boricha copy-paste qilmang, qo'lda yozing: barmoqlar orqali ham o'rganiladi.
3. Tugatgach menga yozing, masalan: *"02-dars tayyor"*. Savollar va xatolarni ham yozing.
   Men kodingizni ko'rib chiqaman, xatolarni tushuntiraman va keyingi darsni yozaman.
4. Qiynalsangiz, `../fluid_sim/` papkasidagi tayyor loyihaga qarashingiz mumkin. U **namuna**:
   avval o'zingiz urinib ko'ring, keyin solishtiring.

Har bir dars fayli bir xil tuzilishda:

| Bo'lim | Nima bor |
|---|---|
| **Maqsad** | Dars oxirida nimaga ega bo'lasiz |
| **Nazariya** | Tushunchalar, soddalashtirilgan, misollar bilan |
| **Amaliyot** | Qadamma-qadam nima yozish kerak |
| **Tekshiruv** | Hammasi to'g'ri bo'lsa nimani ko'rishingiz kerak |
| **Mashqlar** | O'zingizni sinash uchun kichik vazifalar |
| **O'zingizni tekshiring** | Nazariy savollar (javobini bilsangiz, keyingi darsga o'ting) |

Yangi atamalar `lugat.md` faylida jamlanib boradi.

---

## Nega aynan shu tartibda?

Vulkan — juda batafsil API. Ekranga bitta uchburchak chizish uchun ham yuzlab qator kod kerak.
Shuning uchun:

1. **Avval asboblar va oyna** — tez natija, muhitni sozlab olamiz.
2. **Keyin Vulkan bilan zarrachalarni chizishni** o'rganamiz. Bu eng qiyin "tog'", lekin uni
   kichik qadamlarga bo'lamiz: har bir darsda ekranda yangi narsa ko'rinadi.
3. **Fizikani CPU'da** yozamiz. Oddiy C++ kodida tushunish va xato topish ancha oson.
   Chizish uchun 2-moduldagi Vulkan kodimizdan foydalanamiz.
4. **Keyin fizikani GPU'ga ko'chiramiz** (compute shaderlar). Shunda 4 ming emas,
   yuz minglab zarracha bo'ladi.
5. **3D** — oxirida, chunki u yuqoridagilarning hammasiga tayanadi.

Sebastian Lague ham videosida shu yo'ldan borgan: avval CPU, keyin GPU.

---

## Modullar va darslar

### 0-modul. Tayyorgarlik
| # | Dars | Natija |
|---|---|---|
| 01 | Muhit, kompilyatsiya va CMake | Birinchi C++ dasturi CMake bilan build qilinadi |
| 02 | Oyna va asosiy sikl (GLFW) | Ekranda oyna ochiladi, FPS ko'rinadi |

### 1-modul. Vulkan asoslari: birinchi rasm
| # | Dars | Natija |
|---|---|---|
| 03 | GPU va Vulkan qanday ishlaydi | Nazariya: CPU va GPU farqi, Vulkan obyektlari xaritasi |
| 04 | Instance va validation layer | Vulkan ishga tushadi, xatolarni ko'rsatuvchi "yordamchi" yoqiladi |
| 05 | Physical device, logical device, queue | GPU tanlanadi, unga buyruq yuborish yo'li ochiladi |
| 06 | Surface va swapchain | Oyna bilan Vulkan bog'lanadi |
| 07 | Command buffer va sinxronizatsiya | **Oyna rangga bo'yaladi** (birinchi Vulkan rasmi) |
| 08 | Shaderlar, GLSL va graphics pipeline | **Birinchi uchburchak** |
| 09 | Buffer'lar va GPU xotirasi | Ma'lumot CPU'dan GPU'ga uzatiladi, push constants |
| 10 | Instancing: minglab zarracha | **Ekranda minglab doiracha** |

### 2-modul. Suyuqlik fizikasi (CPU'da, 2D)
| # | Dars | Natija |
|---|---|---|
| 11 | Harakat: tezlik, tezlanish, gravitatsiya | Zarrachalar tushadi va devorlardan sakraydi |
| 12 | SPH g'oyasi: zichlik va smoothing kernel | Har bir zarracha atrofidagi zichlik hisoblanadi |
| 13 | Bosim kuchi | **Zarrachalar suyuqlikdek harakatlana boshlaydi** |
| 14 | Barqarorlik: bashorat, near-pressure, yopishqoqlik | Suyuqlik tinch, "portlamaydi" |
| 15 | Tez qo'shni qidirish (spatial grid) | Zarrachalar soni 10 baravar oshadi |
| 16 | Sichqoncha bilan ta'sir qilish | Suyuqlikni tortish va itarish mumkin |

### 3-modul. GPU'ga ko'chirish
| # | Dars | Natija |
|---|---|---|
| 17 | Compute shaderlar | GPU'da birinchi hisob-kitob |
| 18 | Barrierlar, simulatsiya GPU'da | Fizika to'liq GPU'da ishlaydi |
| 19 | Parallel algoritmlar: prefix sum, counting sort | GPU'da saralash |
| 20 | Spatial hash GPU'da, tezlikni o'lchash | **250 000 zarracha, 100+ FPS** |

### 4-modul. 3D
| # | Dars | Natija |
|---|---|---|
| 21 | 3D matematika: vektor, matritsa, kamera, proyeksiya | 3D kameradan ko'rinish |
| 22 | Depth buffer, 3D zarrachalar | Sharsimon zarrachalar |
| 23 | 3D SPH | 3D suyuqlik |
| 24 | Orbit kamera | Sichqoncha bilan kamerani boshqarish |

### 5-modul. Chiroyli rendering (keyinroq)
Ko'pik va sachrash zarrachalari, screen-space suyuqlik rendering, raymarching.
Bu modul rejasi 4-modul tugaganda tuziladi.

---

## Papka tuzilishi (oxirida taxminan shunday bo'ladi)

```
fluid_sim_tutorial/
├── CMakeLists.txt      build tavsifi
├── docs/               darslar (shu fayllar)
├── src/                C++ kod
├── shaders/            GLSL shaderlar
└── build/              build natijalari (avtomatik yaratiladi, o'zgartirmaymiz)
```

---

## Matematika haqida

Ko'p matematika talab qilinmaydi. Kerak bo'ladiganlari:

- **Vektorlar** (2D/3D): qo'shish, uzunlik, yo'nalish. 11-darsda tushuntiriladi.
- **Hosila** (derivative) g'oyasi: "funksiya qanchalik tez o'zgaradi". 13-darsda, soddalashtirib.
- **Matritsalar**: faqat 3D'da (21-dars), ular ham "nuqtani boshqa joyga o'tkazuvchi qurilma"
  sifatida tushuntiriladi.

Formulalardan qo'rqmang: har bir formula avval so'z bilan, keyin rasm bilan, keyin kod
bilan tushuntiriladi.
