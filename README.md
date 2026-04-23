# ARINC 653 Bölümlenmiş Zamanlayıcı (Partitioned Scheduler) — C++ Simülasyonu

Bu proje; zaman bölümleme (time partitioning), uzay bölümleme (space partitioning), deterministik yürütme ve hata izolasyonunu C++17 kullanarak gösteren basitleştirilmiş bir ARINC 653 tarzı zamanlayıcı simülasyonudur.

---

## İçindekiler

1. [Proje Yapısı](#proje-yapısı)
2. [Derleme ve Çalıştırma](#derleme-ve-çalıştırma)
3. [Beklenen Çıktı](#beklenen-çıktı)
4. [Tasarım Kararları](#tasarım-kararları)
5. [Teorik Sorular](#teorik-sorular)

---

## Proje Yapısı

```
arinc653_scheduler/
├── include/
│   ├── Partition.h        # Tüm bölümler için soyut temel sınıf
│   ├── FlightControl.h    # Yüksek kritiklik seviyeli bölüm (60 ms penceresi)
│   ├── Display.h          # Düşük kritiklik seviyeli bölüm (40 ms penceresi)
│   ├── Scheduler.h        # Sabit döngülü (fixed-cyclic) deterministik zamanlayıcı
│   └── HealthMonitor.h    # Opsiyonel hata günlüğü (fault logging) modülü
├── src/
│   ├── main.cpp           # Giriş noktası; bölümleri zamanlayıcıya bağlar
│   ├── FlightControl.cpp
│   ├── Display.cpp
│   ├── Scheduler.cpp
│   └── HealthMonitor.cpp
├── CMakeLists.txt         # CMake yapılandırması (önerilen)
├── Makefile               # Alternatif: düz make dosyası
└── README.md
```

---

### Gereksinimler

* **C++17** uyumlu derleyici: GCC 7+, Clang 5+ veya MSVC 2017+
* `make` veya `cmake` (CMake ≥ 3.14)
* Harici bağımlılık yoktur - sadece standart kütüphane kullanılır.

---

## Beklenen Çıktı

```text
========================================
  ARINC 653 Zamanlayıcı — Başlatılıyor
  Döngü süresi: 100 ms | Döngü Sayısı: 5
  Kayıtlı Bölüm Sayısı: 2
========================================

[Döngü 1 | 13:29:15]
  Flight Control çalışıyor... [irtifa=10001.5 m, hız=249.9 m/s]
  Display çalışıyor... [ekran=Frame-1]

[Döngü 2 | 13:29:15]
  Flight Control çalışıyor... [irtifa=10003.0 m, hız=249.8 m/s]
  [SAĞLIK MONİTÖRÜ] Display HATASI: 2. döngüde görüntüleme hatası oluştu!

[Döngü 3 | 13:29:15]
  Flight Control çalışıyor... [irtifa=10004.5 m, hız=249.7 m/s]
  Display çalışıyor... [ekran=Frame-3]

[Döngü 4 | 13:29:15]
  Flight Control çalışıyor... [irtifa=10006.0 m, hız=249.6 m/s]
  Display çalışıyor... [ekran=Frame-4]

[Döngü 5 | 13:29:16]
  Flight Control çalışıyor... [irtifa=10007.5 m, hız=249.5 m/s]
  Display çalışıyor... [ekran=Frame-5]

========================================
  Zamanlayıcı tamamlandı. 5 döngü bitti.
========================================
```

**Temel Gözlemler:**
* **Döngü 2:** Display (Ekran) bölümü bir istisna (exception) fırlatır; hata yakalanır ve günlüğe kaydedilir.
* **Flight Control (Uçuş Kontrolü):** 5 döngü boyunca kesintisiz çalışmaya devam eder.
* **Yürütme sırası deterministiktir:** Flight Control her zaman Display'den önce çalışır.

---

## Tasarım Kararları

### 1. Soyut `Partition` Temel Sınıfı
Tüm bölümler, yalnızca `execute(int cycle)` ve `getName()` metodlarını dışa açan `Partition` sınıfından türetilir. Zamanlayıcı bir `vector<unique_ptr<Partition>>` tutar ve somut türler hakkında bilgi sahibi değildir; bu sayede yeni bir bölüm eklemek zamanlayıcı kodunda değişiklik gerektirmez.

### 2. Sınıf Sınırları ile Sağlanan Uzay İzolasyonu
`FlightControl` kendi `irtifa` ve `hız` verilerine sahiptir; `Display` ise `ekranVerisi`ni tutar. Bunlar özel (private) üyelerdir ve global paylaşım yoktur. Bu durum, ARINC 653 bellek bölümlendirmesini yansıtır: Display bölümündeki bir hata, Flight Control verilerini bozamaz.

### 3. Deterministik Sabit Döngülü Zamanlama
Zamanlayıcı, bölümleri her döngüde kayıt sırasına göre çalıştırır; öncelik kuyruğu, durdurma (preemption) veya dinamik sıralama yoktur. Bu, ARINC 653 Ana Çerçeve / Alt Çerçeve (Major/Minor Frame) modeliyle eşleşir.

### 4. `try-catch` ile Hata İzolasyonu
Her `partition->execute()` çağrısı bir `try-catch` bloğuna alınmıştır. Bir bölümdeki hata yakalanır, Sağlık Monitörü (Health Monitor) kanalına yazılır ve bir sonraki bölümden devam edilir. Zamanlayıcının kendisi asla çökmez.

### 5. `std::this_thread::sleep_for` ile Zaman Pencereleri
Simülasyonda bölümlerin çalışma süreleri `sleep_for` ile taklit edilir. Gerçek bir ARINC 653 RTOS'ta çekirdek, bu pencereleri donanım zamanlayıcıları ile zorunlu kılar ve süresini aşan bölümü durdurur.

---

## Teorik Sorular

### A1 — IMODE'da Arayüz (Interface) Nedir?
IMODE'da **Arayüz**, bileşenler arasındaki iletişim sözleşmesini tanımlar. Bileşenin iç ayrıntılarını açıklamadan hangi verileri veya hizmetleri sunduğunu belirtir.
* **Ayrıştırma (Decoupling):** Farklı üreticilerin bileşenleri, arayüze uydukları sürece birlikte çalışabilir.
* **Doğrulama:** Entegrasyon öncesinde bileşenlerin uyumluluğu modelleme araçlarıyla kontrol edilebilir.

### B1 — JDK vs JRE vs JVM

| Terim | Kapsam | İçerik | Kimin İçin? |
| :--- | :--- | :--- | :--- |
| **JVM** | Çalışma Motoru | Bytecode yorumlayıcı, JIT derleyici, çöp toplayıcı | Tek başına kurulmaz. |
| **JRE** | Çalışma Ortamı | JVM + Standart kütüphaneler (java.lang, vb.) | Java uygulaması çalıştıran kullanıcılar. |
| **JDK** | Geliştirme Kiti | JRE + `javac` derleyicisi + hata ayıklayıcılar | Java uygulaması geliştirenler. |

Hiyerarşi şöyledir: **JDK ⊃ JRE ⊃ JVM**.

### C1 — ARINC 653'te Zaman ve Uzay Bölümleme Neden Kritik?
Modern aviyonik sistemler, tek bir CPU üzerinde birden fazla uygulamayı (uçuş kontrolü, navigasyon, ekranlar) Entegre Modüler Aviyonik (IMA) altında çalıştırır.

* **Zaman Bölümleme:** CPU açlığı sorununu çözer. Her bölüme sabit bir zaman penceresi garanti edilir. Ekran uygulamasındaki sonsuz bir döngü, Uçuş Kontrolü'ne ait CPU süresini çalamaz.
* **Uzay Bölümleme:** Bellek bozulması sorununu çözer. Her bölümün bellek alanı MMU (Memory Management Unit) ile korunur; böylece bir bölümdeki hatalı gösterici (pointer) hatası, komşu bölümün verilerini sessizce bozmak yerine bir bellek koruma hatası tetikler.

Bu iki mekanizma birlikte **Hata Sınırlandırmayı (Fault Containment)** sağlar. Uçak emniyetini etkileyebilecek yazılımlar için DO-178C Seviye A sertifikasyonunun temel şartıdır.

