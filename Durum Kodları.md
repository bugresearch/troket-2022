## Roket Durum Kodları
### 2xx -> OK
200 -> Görev Bilgisayarı Başarıyla Çalışıyor

201 -> Kontroller Bitti, Ateşleme Bekleniyor

202 -> Ateşleme Başladı

203 -> Yakıt Tükendi	

210 -> Tepe Noktası

211 -> Ayrılma

220 -> Paraşüt Açma

230 -> Başarılı Bir Şekilde İniş Gerçekleşti

### 3xx -> REDIRECT
300 -> Ana bilgisayarda sorun tespit edildi, yedek bilgisayara geçiş yapılıyor.
301 -> Yedek Bilgisayar Başarıyla Aktive Edildi
!!! Geriye Kalan Tüm Status Kodlar  3xx şeklinde gönderilecektir. 	

### 5xx -> ERROR
500 -> Herhangi bir durumdan dolayı görev bilgisayarı sistemin ateşleme konumunna geçmesini istemiyor. Ayrıntılı açıklama için avyonik sistemdeki hata koduna bakınız veya yedek bilgisayar ile uçuşu sürdürünüz.
501 -> Ayrılma gerçekleşmedi yedek bilgisayar durumu ele alacak (Force)
502 -> Paraşüt Sistemi Açılmadı, Yedek Paraşüt Açılıyor
503  -> Yedek paraşüt açılamadı, muhtemel çarpma
504 -> KRİTİK İVME !!!
505 -> KRİTİK YÜKSEKLİK !!!
