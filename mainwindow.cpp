#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QDebug>
#include <fstream>
#include <set>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <wiringPi.h>
using namespace cv;
using namespace cv::dnn;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Load Wiring Pi
    wiringPiSetup();
    pinMode(1, OUTPUT);
    pinMode(4, OUTPUT);
    digitalWrite(1, LOW);
    digitalWrite(4, LOW);
    // Load model ONNX
    net = readNetFromONNX("/home/pi/models/squeezenet1.1-7.onnx");
    if (net.empty()) {
        qCritical() << "Failed to load ONNX model!";
        return;
    }
    // Load ImageNet labels
    ifstream ifs("/home/pi/models/imagenet_classes.txt");
    string line;
    while (getline(ifs, line)) labels.push_back(line);
    // Khởi tạo album ảnh (có thể dùng Qt Resource hoặc đường dẫn tương đối)
    album = {
        "./images/n02085620_7.jpg",
        "./images/n02085620_199.jpg",
        "./images/n02085620_588.jpg"
    };
    currentIndex = 0;
    selected = 0;
    showCurrentImage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Hiển thị ảnh hiện tại
void MainWindow::showCurrentImage() {
    if (album.isEmpty()) return;
    QPixmap pix(album[currentIndex]);
    if (pix.isNull()) {
        qDebug() << "Failed to load image:" << album[currentIndex];
    } else {
        ui->image->setPixmap(pix);
        ui->image->setScaledContents(true);
    }
}

// Chạy model phân loại Dog/Cat
void MainWindow::classifyCurrentImage() {
    QString path = album[currentIndex];
    Mat img = imread(path.toStdString());
    if (img.empty()) {
        qDebug() << "Cannot read image" << path;
        return;
    }

    Mat blob = blobFromImage(img, 1.0/255.0, Size(224,224), Scalar(), true, false);
    net.setInput(blob);
    if (net.empty()) {
        qDebug() << "Model not loaded, skip classification";
        return;
    }

    Mat prob = net.forward();

    Point classIdPoint;
    double confidence;
    minMaxLoc(prob.reshape(1,1), 0, &confidence, 0, &classIdPoint);
    int classId = classIdPoint.x;
    int rightAns = 0;
    string predicted = labels[classId];

    set<string> dogClasses = {"Chihuahua","Japanese spaniel","Maltese dog","Pekinese","Shih-Tzu","Blenheim spaniel","papillon","toy terrier","Rhodesian ridgeback","Afghan hound","basset","beagle","bloodhound","bluetick","black-and-tan coonhound","Walker hound","English foxhound","redbone","borzoi","Irish wolfhound","Italian greyhound","whippet","Ibizan hound","Norwegian elkhound","otterhound","Saluki","Scottish deerhound","Weimaraner","Staffordshire bullterrier","American Staffordshire terrier","Bedlington terrier","Border terrier","Kerry blue terrier","Irish terrier","Norfolk terrier","Norwich terrier","Yorkshire terrier","wire-haired fox terrier","Lakeland terrier","Sealyham terrier","Airedale","cairn","Australian terrier","Dandie Dinmont","Boston bull","miniature schnauzer","giant schnauzer","standard schnauzer","Scotch terrier","Tibetan terrier","silky terrier","soft-coated wheaten terrier","West Highland white terrier","Lhasa","flat-coated retriever","curly-coated retriever","golden retriever","Labrador retriever","Chesapeake Bay retriever","German short-haired pointer","vizsla","English setter","Irish setter","Gordon setter","Brittany spaniel","clumber","English springer","Welsh springer spaniel","cocker spaniel","Sussex spaniel","Irish water spaniel","kuvasz","schipperke","groenendael","malinois","briard","kelpie","komondor","Old English sheepdog","Shetland sheepdog","collie","Border collie","Bouvier des Flandres","Rottweiler","German shepherd","Doberman","miniature pinscher","Greater Swiss Mountain dog","Bernese mountain dog","Appenzeller","EntleBucher","boxer","bull mastiff","Tibetan mastiff","French bulldog","Great Dane","Saint Bernard","Eskimo dog","malamute","Siberian husky","dalmatian","affenpinscher","basenji","pug","Leonberg","Newfoundland","Great Pyrenees","Samoyed","Pomeranian","chow","keeshond","Brabancon griffon","Pembroke","Cardigan","toy poodle","miniature poodle","standard poodle","Mexican hairless","dingo","dhole","African hunting dog"};
    set<string> catClasses = {"tabby","tiger cat","Persian cat","Siamese cat","Egyptian cat","cougar","lynx","leopard","snow leopard","jaguar","lion","tiger","cheetah","Madagascar cat"};

    string result;
    if (dogClasses.count(predicted)){
        result = "Dog";
        rightAns = 1; // Dog
    }
    else if (catClasses.count(predicted)) {
        result = "Cat";
        rightAns = 2; // Cat
    }
    else {
        result = "Other";
        rightAns = 0;
    }

    if(rightAns == 0) {
        ui->ai->setText(QString::fromStdString(predicted).trimmed());
    }
    else if (rightAns == selected) {
        ui->ai->setText("Correct");
        digitalWrite(4, LOW);
        digitalWrite(1, HIGH);
    }
    else{
        ui->ai->setText("Incorrect");
        digitalWrite(1, LOW);
        digitalWrite(4, HIGH);
    }

    qDebug() << "Ảnh này là:" << QString::fromStdString(result)
            << "(model đoán:" << QString::fromStdString(predicted).trimmed() << ")"
            << "Confidence:" << confidence;
}

// Nút next slide
void MainWindow::on_next_slide_clicked() {
    if (album.isEmpty()) return;
    currentIndex = (currentIndex + 1) % album.size();
    ui->ai->setText("");
    digitalWrite(1, LOW);
    digitalWrite(4, LOW);
    showCurrentImage();
}

// Nút cat / dog (chỉ debug)
void MainWindow::on_dog_clicked() { qDebug() << "pressed DOG";  selected = 1;   classifyCurrentImage();}
void MainWindow::on_cat_clicked() { qDebug() << "pressed CAT";  selected = 2;   classifyCurrentImage();}
