import random
import numpy
import pandas
import datetime
import time
import csv
# // Buzzer Pin: Aşağıda ses çıkarıcak bulunması için 
# // Röleler 2 tane: Ayrılmayı Sağlıyor 
# // Segment 8 led: Status Code 
# // Blink Led: Açık olduğunu gösteriyor 
# // Tilt Sensörü: Burnun eğilip eğilmediğinin analizi 
# // Basınç Sensörü (İrtifa Ölçer) 
# // Isı Sıcaklık Modülü 
# // Haberleşme Modülü (GPRS) (iptal edildi) 
# // İvme Sensörü 
# // GPS Var

class fakeData:
    yuzey_alani = 5
    g = 9.80023
    agirlik = g * 43
    yakit_miktari = 362.4
    yuzeyin_isi_iletimi = 1.163
    yuzey_kalinligi = 0.4
    yukseklik = 0
    ivme = 0
    isi = 25
    gps_1 = 38.674816
    gps_2 = 39.222515
    sure = 300
    def __init__(self) -> None:
        print("Hello Roket")

    def buzzer_ses(self):
        if self.yukseklik < 5 == True:
            return "Ses çalisti"
        return False
    
    def ilk_ayrilma_etkin(self):
        if self.ivme == 21.6413:
            time.sleep(0.5)
            return "İlk ayrılma etkin"
        else:
            return "Ayrılma Bekleniyor"
    
    def ikinci_ayrilma_etkin(self):
        if self.yukseklik < 500:
            fakeData.sure = 0
        if self.yukseklik < 21.6413:
            return "ikinci ayrılma etkin"
        else:
            return "Ayrılma Bekleniyor"
    def basinc_sensorunden_veri_al(self):
        return (fakeData.yuzey_alani / fakeData.agirlik) * self.ivme

    def isi_sensorunden_veri_al(self):
        return self.isi + fakeData.yuzeyin_isi_iletimi
    
    def yukseklik_sensorunden_veri_al(self):
        if fakeData.yukseklik < 21 and fakeData.yakit_miktari == 0:
            self.ivme = 0
            fakeData.sure = 0
        self.yukseklik += self.ivme
        if self.ivme == 21.6413:
            self.yukseklik -= (fakeData.agirlik * 9.80023)/100 + self.ivme
        return self.yukseklik

    def ivme_sensorunden_veri_al(self):
        if fakeData.yakit_miktari > 0:
            self.ivme += 45.3
            fakeData.yakit_miktari -= 45.3
            return self.ivme
        if fakeData.yakit_miktari <= 0:
            self.ivme -= 9.80023
            if self.ivme < 22:
                self.ivme = 21.6413
                return self.ivme   
            return self.ivme
    def gps_birinci(self):
        self.gps_1 += 0.000214
        return self.gps_1
    def gps_ikinci(self):
        self.gps_2 += 0.000036
        return self.gps_2
fakeData = fakeData()

for _ in range(fakeData.sure):
    buzzer = fakeData.buzzer_ses()
    ilk_ayrilma = fakeData.ilk_ayrilma_etkin()
    ikinci_ayrilma = fakeData.ikinci_ayrilma_etkin()
    basinc = fakeData.basinc_sensorunden_veri_al()
    isi = fakeData.isi_sensorunden_veri_al()
    yukseklik = fakeData.yukseklik_sensorunden_veri_al()
    ivme = fakeData.ivme_sensorunden_veri_al()
    gps_one = fakeData.gps_birinci()
    gps_two = fakeData.gps_ikinci()
    # print( "Buzzer "+str(buzzer) + " ilkAyrılma " + str(ilk_ayrilma) + " ikinci_ayrilma " + str(ikinci_ayrilma) + " basinc " + str(basinc) + " isi " + str(isi) + " yukseklik " + str(yukseklik) + " ivme " + str(ivme) + " gps_one " + str(gps_one) + " gps_two " + str(gps_two))
    writerr = str(buzzer)+ "@" + str(ilk_ayrilma) + "@" + str(ikinci_ayrilma) + "@" + str(basinc) + "@" + str(isi) + "@" + str(yukseklik) + "@" + str(ivme) + "@" + str(gps_one) + "@" + str(gps_two)
    f = open("demofile2.txt", "a")
    f.write(writerr + "\n")
    f.close()

    