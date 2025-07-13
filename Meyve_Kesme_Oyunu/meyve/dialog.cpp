//Nidanur CILDIR
//21100011053
//Ozellik:
//Belirli araliklarla muz dusuruyoruz. muz ilk tiklamada soyuluyor.
//Sonraki tum tiklamalarda Kesilen Karpuz sayisi +2 oluyor.



#include "dialog.h"
#include "ui_dialog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPushButton>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>
#include <QRandomGenerator>

int PatlayanKarpuzSayisi = 0; // Patlayan karpuz sayısını tutacak global değişken
int KacirilanKarpuzSayisi = 0;
int EnYuksekSkor = 0; // En yüksek skor

Dialog::Dialog(QWidget *parent)//
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // Önceki skorları dosyadan oku
    okuSkorlar("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/skorlar.txt");

    // Arka planı beyaz yapacak QLabel'i oluştur
    background = new QLabel(this);
    background->setGeometry(0, 0, width(), 100);
    background->setStyleSheet("background-color: white");

    // "Süre:" yazısı için QLabel'i oluştur ve ayarla
    sure_labeli = new QLabel("Süre ", this);
    sure_labeli->setGeometry(5, 15, 50, 80);
    sure_labeli->setFont(QFont("Arial", 18)); // Font boyutunu 22 yap

    // Geri sayım için QLabel'i oluştur ve ayarla
    geri_sayim_labeli = new QLabel("30", this);
    geri_sayim_labeli->setGeometry(70, 15, 50, 80);
    geri_sayim_labeli->setFont(QFont("Arial", 22)); // Font boyutunu 22 yap
    geri_sayim_labeli->setStyleSheet("color: blue");

    KesilenKarpuz_Labeli = new QLabel("Kesilen Karpuz Sayısı=", this);
    KesilenKarpuz_Labeli->setGeometry(800, 0, 300, 50);
    KesilenKarpuz_Labeli->setFont(QFont("Arial", 18));
    KesilenKarpuz_Labeli->setWordWrap(true);

    KarpuzKesilen_sayisi = new QLabel("0", this);
    KarpuzKesilen_sayisi->setGeometry(1100, 0, 300, 50);
    KarpuzKesilen_sayisi->setFont(QFont("Arial", 18));
    KarpuzKesilen_sayisi->setStyleSheet("color: green");

    KacirilanKarpuz_sayisi = new QLabel("0", this);
    KacirilanKarpuz_sayisi->setGeometry(1100, 30, 300, 50);
    KacirilanKarpuz_sayisi->setFont(QFont("Arial", 18));
    KacirilanKarpuz_sayisi->setStyleSheet("color: red");

    KacirilanKarpuz_sayisi_labeli = new QLabel("Kaçırılan karpuz sayısı=", this);
    KacirilanKarpuz_sayisi_labeli->setGeometry(800, 30, 300, 50);
    KacirilanKarpuz_sayisi_labeli->setFont(QFont("Arial", 18));

    KoordinatOkuma("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/konumlar.txt");

    // Timer oluştur ve her saniye yeni bir buton oluşturacak şekilde bağla
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Dialog::KoordinatAlma);
    timer->start(1000); // Her 1000 ms (1 saniye) bir timeout sinyali göndersin

    // Geri sayım için bir QTimer oluştur ve her saniye geri sayım yapacak şekilde bağla
    QTimer *countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &Dialog::updateCountdown);
    countdownTimer->start(1000); // Her 1000 ms (1 saniye) bir timeout sinyali göndersin
    geriSayim = 33; // Başlangıçta 30 saniye olsun

    // Yeni buton ekleme zamanlayıcısı
    QTimer *floatingButtonTimer = new QTimer(this);
    connect(floatingButtonTimer, &QTimer::timeout, this, &Dialog::MuzKaydirma);
    floatingButtonTimer->start(5000); // Her 5 saniyede bir çağrılacak
}

Dialog::~Dialog()//
{
    // Oyun bittiğinde skorları güncelle
    if (PatlayanKarpuzSayisi > EnYuksekSkor) {
        EnYuksekSkor = PatlayanKarpuzSayisi;
        QMessageBox::information(this, "Tebrikler!", QString("Süre doldu!\nYeni en yüksek skor: %1\nKesilen karpuz sayısı: %2\nKaçırılan karpuz sayısı: %3").arg(EnYuksekSkor).arg(PatlayanKarpuzSayisi).arg(KacirilanKarpuzSayisi));
    } else {
        QMessageBox::information(this, "Süre Bitti", QString("Süre doldu!\nKesilen karpuz sayısı: %1\nKaçırılan karpuz sayısı: %2\nEn yüksek skor: %3").arg(PatlayanKarpuzSayisi).arg(KacirilanKarpuzSayisi).arg(EnYuksekSkor));
    }
    yazSkorlar("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/skorlar.txt");

    delete ui;
}

void Dialog::KarpuzYapma(int x, int y)//
{
    QPushButton *PatlayacakKarpuz = new QPushButton(this);
    PatlayacakKarpuz->setGeometry(x, y, 100, 50);
    //Resmi icon nesnesine cekiyoruz:
    QIcon icon("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/resim/1.png");
    PatlayacakKarpuz->setIcon(icon); // Butona simgeyi ayarla
    PatlayacakKarpuz->setIconSize(QSize(50, 50)); // boyutunu ayarladik:
    PatlayacakKarpuz->setFlat(true);
    PatlayacakKarpuz->show();

    //Tiklanirsa ne olacagi :
    connect(PatlayacakKarpuz, &QPushButton::clicked, [PatlayacakKarpuz, this]() {
        PatlayanKarpuzSayisi++;
        KarpuzKesilen_sayisi->setText(QString::number(PatlayanKarpuzSayisi));
        PatlayacakKarpuz->setIcon(QIcon("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/resim/2.png"));
        //Tiklarsak egewr 2 nolu resim gelsin
        PatlayacakKarpuz->setIconSize(QSize(50, 50));
        PatlayacakKarpuz->setFlat(true);
    });

    // Bir timer yapiyoruz. Her 1 saniyede asagi inmesi icin :
    QTimer *BirSaniye = new QTimer(this);
    connect(BirSaniye, &QTimer::timeout, [PatlayacakKarpuz, BirSaniye, this]() {
        PatlayacakKarpuz->move(PatlayacakKarpuz->x(), PatlayacakKarpuz->y() + 15); // Butonu 20 birim aşağı kaydır

        // Eger buton ekranın disina ciktiysa, butonu siler ve kacirilan karpuz sayisini artir:
        if (PatlayacakKarpuz->y() > PatlayacakKarpuz->parentWidget()->height()) {
            PatlayacakKarpuz->deleteLater(); // PatlayacakKarpuzu sil
            BirSaniye->stop(); // Birsaniye timerini durdur
            BirSaniye->deleteLater(); // Birsaniye timerini sil
            KacirilanKarpuzSayisi++;
            KacirilanKarpuz_sayisi->setText(QString::number(KacirilanKarpuzSayisi)); //Labeli yeniliyoruyz:
        }
    });
    BirSaniye->start(100);

    // PatlayacakKarpuza tiklandiginda  hareketi durur ve 3 saniye sonra kaybolur:
    // durma
    //silme
    //3saniye
    connect(PatlayacakKarpuz, &QPushButton::clicked, [PatlayacakKarpuz, BirSaniye]() {
        BirSaniye->stop();
        QTimer::singleShot(3000, PatlayacakKarpuz, &QPushButton::deleteLater);
    });
}

void Dialog::MuzKaydirma()//
{
    QPushButton *SoyulacakMuz = new QPushButton(this);
    SoyulacakMuz->setGeometry(QRandomGenerator::global()->bounded(0, 1350), 100, 100, 50); // Rastgele x, sabit y koordinatı
    SoyulacakMuz->setIcon(QIcon("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/resim/3.png")); // Yeni resim
    SoyulacakMuz->setIconSize(QSize(50, 50));
    SoyulacakMuz->setFlat(true);
    SoyulacakMuz->show();
    //
    //
    //
    //
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [SoyulacakMuz, timer, this]() {
        SoyulacakMuz->move(SoyulacakMuz->x(), SoyulacakMuz->y() + 22); // Hızlı kaydırma
        if (SoyulacakMuz->y() > SoyulacakMuz->parentWidget()->height()) {
            SoyulacakMuz->deleteLater();
            timer->stop();
            //
            timer->deleteLater();
        }
    });
    timer->start(50);
    // tiklaninca yapilacaklar:
    connect(SoyulacakMuz, &QPushButton::clicked, [SoyulacakMuz, this]() {
        //
        //
        //buton->setEnabled(false);
        //
        SoyulacakMuz->setIcon(QIcon("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/resim/5.png"));
        SoyulacakMuz->setIconSize(QSize(50, 50));
        SoyulacakMuz->setFlat(true);

        QTimer::singleShot(3000, SoyulacakMuz, &QObject::deleteLater);
        PatlayanKarpuzSayisi += 2;
        //
        KarpuzKesilen_sayisi->setText(QString::number(PatlayanKarpuzSayisi));


        //
    });

}

void Dialog::KoordinatAlma()//
{
    if (!buttonLocations.isEmpty()) {
        QString location = buttonLocations.takeFirst();
        QStringList dosya_veri = location.split(" ");
        if (dosya_veri.size() == 2) {
            int kordinat_x = dosya_veri[0].toInt();
            int kordinat_y = dosya_veri[1].toInt();
            KarpuzYapma(kordinat_x, kordinat_y);
        }
    }
}
//

void Dialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
}

void Dialog::KoordinatOkuma(const QString &KoordinatDosyasiAdi)//
{
    QFile Koordinat(KoordinatDosyasiAdi);
    if (!Koordinat.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Dosya açılamadı: " << Koordinat.errorString();
        return;
    }

    QTextStream Oku(&Koordinat);
    while (!Oku.atEnd()) {
        QString row = Oku.readLine();
        buttonLocations.append(row); // Okunan konumu listeye ekle
    }

    Koordinat.close();
}
void Dialog::updateCountdown()//
{
    geriSayim--; // Geri sayımı azalt
    geri_sayim_labeli->setText(QString::number(geriSayim)); // Geri sayimi yenile:
    if (geriSayim == 0) {
        // Oyun bittiğinde QMessageBox gostersin:
        if (PatlayanKarpuzSayisi > EnYuksekSkor) {
            EnYuksekSkor = PatlayanKarpuzSayisi;
            QMessageBox::information(this, "Tebrikler!", QString("Süre doldu!\nYeni en yüksek skor: %1\nKesilen karpuz sayısı: %2\nKaçırılan karpuz sayısı: %3").arg(EnYuksekSkor).arg(PatlayanKarpuzSayisi).arg(KacirilanKarpuzSayisi));
        //
        } else {
            QMessageBox::information(this, "Süre Bitti", QString("Süre doldu!\nKesilen karpuz sayısı: %1\nKaçırılan karpuz sayısı: %2\nEn yüksek skor: %3").arg(PatlayanKarpuzSayisi).arg(KacirilanKarpuzSayisi).arg(EnYuksekSkor));
        //
        }
        yazSkorlar("C:/Users/monster/Desktop/Nida QT Odev1/Nida QT Odev1/Nida QT Odev/21100011053/skorlar.txt");
        close();
    }
}

void Dialog::okuSkorlar(const QString &SkorDosyasiAdi)//
{
    QFile SkorlarDosyasi(SkorDosyasiAdi);//
    //
    //
    //
    //
    //
    if (!SkorlarDosyasi.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Dosya açılamadı: " << SkorlarDosyasi.errorString();

        //
        return;
    }

    //
    QTextStream reader(&SkorlarDosyasi);
    while (!reader.atEnd()) {
        QString row = reader.readLine();
        EnYuksekSkor = row.toInt();
    }

    SkorlarDosyasi.close();
}

void Dialog::yazSkorlar(const QString &SkorlarDosyasiAdi)//
{
    QFile SkorlarDosyasi(SkorlarDosyasiAdi);
    if (!SkorlarDosyasi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Dosya açılamadı: " << SkorlarDosyasi.errorString();
        return;
    }
//
    //
    //
                                                                                                                                                                                                                                      //
    QTextStream writer(&SkorlarDosyasi);
    //
    writer << EnYuksekSkor;

    SkorlarDosyasi.close();
}


