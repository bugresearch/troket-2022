/* Pin Değişkenleri */
int role_1 = 2; // 1. Ayrılma Röle
int role_2 = 3; // 2. Ayrılma Röle
int buzzer = 4; // Aşağı düştüğünde çalacak
int blink_led = 5; // Yanıp sönerek başarılı bir çalışma sürecini test eder
int segment_KEYS[] = [6,7,8,9,10,11,12]; // Segment ledin pinleri

/* Durum Değişkenleri */
bool atesleme, yakitbitti, tepenoktasi, ayrilma, roketparasut, roketindi;
atesleme = false;
yakitbitti = false;
tepenoktasi = false;
ayrilma = false;
roketparasut = false;
roketindi = false;

/* Diğer Değişkenler */
double eskiivme; // İvmenin Artar veya Azalır Durumda olduğunu öğrenmek için kullanılacaktır

void setup() {
  /* PinMode Ayarları */
  pinMode(role_1,OUTPUT);
  pinMode(role_2,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(blink_led,OUTPUT);
  for(int i =0; i < 7; i++){
    pinMode(segment_KEYS[İ],OUTPUT);
  }

  Serial.begin(9600); //Seri Haberleşme Başlatıldı

  /* Bilgisayarın Açıldığına Dair Veri Gönderimi */
  veriGonder("<status:200>");

  /* Açılış Testi */
  bool acilis_testi_sonuc = acilisTesti();
  if(bool == false){
    veriGonder("<status:500>");
    die();
  }
  veriGonder("<status:201>");

  digitalWrite(blink_led, HIGH); // Durum Ledi Yakıldı
}

void loop() {
  /*
   * OK -> Bilgisayar Açıldığında Yer İstasyonuna Açıldığına Dair Veri Gönder
   * OK -> Bataryaların bağlı ve yeterli olduğunu, tüm sensörlerden başarıyla veri alındığını denetle
   * OK -> Roketin Üzerinde Bulunan Blink Led, Yanıp Sönmeli
   * OK -> Sensör, Pil ve Konum Verilerini Yer İstasyonu İle Paylaş
   * OK -> ATEŞLEMEYİ BEKLE
   * OK -> EĞER ATEŞLEME BAŞLAMIŞSA:
   * OK -> İvmenin Azalma Konumuna Geçişini Bekle, İvme azalır konuma geçtiğinde yer istasyonuna yakıtın bittiği verisini gönder
   * OK -> İvmenin 0 olmasını bekle, ivme sıfır olduğunda tilt sensörü ve barometreden gelen veriler ışığında ateşlemeye karar ver
   * OK -> Ayrılmanın ardından yer istasyonuna ayrılma verisini gönder
   * İvmenin azalır haraket ettiği anda yer istasyonuna paraşüt açıldığına dair veriyi gönder
   * Eğer ivme 0 olursa roketin yere indiğine dair veriyi yer istasyonuna ilet
   * Buzzer ile sürekli aynı sesi çıkararak roketin bulunmasını kolaylaştır
   * Bu sırada her iki GPS Modülünden de GPS sinyalleri yer istasyonuna iletilecek, yer istasyonu ilgili sinyallerin ortalamasını alarak konum haritasını çizecek
   */
   sensorveriGonder();

   // Ateşleme Yapıldı Mı?
   if(!atesleme){
    double[] ivme = ivmeSensoru();
    double[] irtifa = basincSensoru();

    bool ivme_bool = ivme > 5;
    bool irtifa_bool = irtifa > 5;

    if(ivme_bool == true and irtifa_bool == true){
      atesleme = true;
      veriGonder = "<status:202>";
      eskiivme = ivme;
    }
   }

   // Yakıt Bitti Mi?
   if(atesleme == true and yakitbitti == false){
    double[] ivme = ivmeSensoru();
    if(ivme < eskiivme){
      yakitbitti = true;
    }
   }

   //İvme 0 mı?
   if(yakitbitti == true and tepenoktasi == false){
    double[] ivmeveri = ivneSensoru();
    bool tiltveri = tiltSensor();
    double[] basincsensoru = basincSensoru();
    if(ivmeveri < 10 and tiltveri == true and basincsensoru < 2500){
      tepenoktasi = true;
      veriGonder("<status:210>");

      // Ayrılma İşlemi
      digitalWrite(role_1, HIGH);
      digitalWrite(role_2, HIGH);
      ayrilma = true;
      veriGonder("<status:211>");
    }
   }

   // Paraşüt Açıldı Mı?
   if(ayrilma == true and roketparasut == false){
    if(eskiivme != null){
      double[] ivme = ivmeSensoru();
      if(ivme < eskiivme){
        roketparasut = true;
        veriGonder("<status:220>");
      }
      eskiivme = ivme;
      delay(10);
    }else{
      eskiivme = ivmeSensoru();
    }
   }

   // Aşağıya İndi Mi?
   if(roketparasut == true and roketindi == false){
    double[] ivme = ivmeSensoru();
    if(ivme == 0){
      roketindi = true;
      veriGonder("<status:230");
    }
   }

   // Roket İndi Buzzerr
   if(roketindi){
    buzzerPlay(0);
   }
}

void sensorveriGonder(){
  
}

/* Verilen Dizinin Elemanlarını Segmentten Yakar */
void ledWrite(int dizi[]){
  for(int i = 0; i < dizi.lenght; i++){
    digitalWrite(segment_KEYS[dizi[i]], HIGH);
  }
}

void segmentDisplay(int sayi){
  /* Segmenti Sıfırlama */
  for(int i = 0; i < 7; i++){
    digitalWrite(segment_KEYS[i], LOW)
  }
  /* Segment Yakma */
  switch(sayi){
    case 0:
      ledWrite(0,1,2,3,4,5);
    case 1:
      ledWrite(1,2);
    case 2:
      ledWrite(0,1,3,4,6);
     case 3:
      ledWrite(0,1,2,3,6);
     case 4:
      ledWrite(1,2,5,6);
     case 5:
      ledWrite(0,2,3,5);
     case 6:
      ledWrite(0,1,2,4,5,6);
     case 7:
      ledWrite(0,1,2);
     case 8:
      ledWrite(0,1,2,3,4,5,6);
    case 9:
      ledWrite(0,1,2,3,5,6);
  }
}

/* Sistem Açılışında Batarya Durumu ve Sensör Veri Okumalarını Test Eden Kod */
bool acilisTesti(){
  
}

/* Play Buzzer */
void buzzerPlay(int saniye){
  if(saniye == 0){
    while(true){
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
      delay(500);
    }
  }else{
    for(int i =0; i < saniye; i++){
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
      delay(500);
    }
  }
}

/* X,Y Kordinatlar */
String gpsKonum(){
  
}

/* İvme Ölçer İvme Verileri */
double[] ivmeSensoru(){
  
}

/* Barometre Basınç ve İrtifa Verileri */
double[] basincSensoru(){
  
}

/* Haberleşme Modülünün Veri Gönderme Fonksiyonu */
void veriGonder(String veri){
  
}

/* Tilt Sensörünün Anlık Konumunu Sorgular */
bool tiltSensor(){
  
}

/* X,Y,Z Jireskop Verileri */
double[] jiroskopSensor(){
  
}

/* SD Kart Veri Yaz */
void sdveriYaz(String veri){
  
}

/* Li-Po batarya Pil Ampermetresi */
double pilAmpermetre(){
  
}

/* Funye Ateşleme Röleleri */
void funyeAtesleme(){
  
}
