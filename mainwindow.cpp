#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>


auto fileName = [](int i) -> QString
{
	return "/media/Files/Data/frame_" + QString::number(i) + ".jpg";
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->frameWidthSpinBox->setMaximum(100);
	ui->gradWidthSpinBox->setMaximum(100);
	ui->powerDoubleSpinBox->setMaximum(3.);
	ui->powerDoubleSpinBox->setSingleStep(0.02);
	ui->powerDoubleSpinBox->setValue(1.);

	colDial = new QColorDialog(QColor(40, 140, 40));
	colDial->setVisible(true);

	QObject::connect(ui->frameWidthSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(drawPic()));
	QObject::connect(ui->gradWidthSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(drawPic()));
	QObject::connect(ui->powerDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(drawPic()));

	QObject::connect(colDial, &QColorDialog::currentColorChanged,
					 this, &MainWindow::drawPic);
	QObject::connect(ui->savePushButton, &QPushButton::clicked,
					 [this]()
	{
		int a = 0;
		do{ ++a; } while(QFile::exists(fileName(a)));
		this->pic.save(fileName(a), 0, 100);
	});
}



MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::drawPic()
{
	pic = QPixmap(1280, 1024);
	pic.fill("black");
	int frameWidth = ui->frameWidthSpinBox->value();
	int gradWidth = ui->gradWidthSpinBox->value();

	const QColor frameColor = this->colDial->currentColor();
	QPainter pnt;
	pnt.begin(&pic);


	auto func = [this](double in){ return pow(in, ui->powerDoubleSpinBox->value()); };

	for(int i = gradWidth + frameWidth; i >= frameWidth; --i)
	{
		double alpha = (i - frameWidth) / double(gradWidth);
//		QColor cur = QColor(frameColor.red(),
//							frameColor.green(),
//							frameColor.blue(),
//							func(1. - alpha) * 255.);
		QColor cur = QColor(frameColor.red() * func(1. - alpha),
							frameColor.green() * func(1. - alpha),
							frameColor.blue() * func(1. - alpha));

		pnt.setBrush(QBrush(cur));
		pnt.setPen(cur);

		pnt.drawRect(0, 0, pic.width(), i);
		pnt.drawRect(0, 0, i, pic.height());
		pnt.drawRect(pic.width(), pic.height(), -i, -pic.height());
		pnt.drawRect(pic.width(), pic.height(), -pic.width(), -i);
	}
	pnt.setBrush(QBrush(frameColor));
	pnt.setPen(frameColor);

	pnt.drawRect(0, 0, pic.width(), frameWidth);
	pnt.drawRect(0, 0, frameWidth, pic.height());
	pnt.drawRect(pic.width(), pic.height(), -frameWidth, -pic.height());
	pnt.drawRect(pic.width(), pic.height(), -pic.width(), -frameWidth);



	pnt.end();
	ui->label->setPixmap(pic.scaled(ui->label->size()));
}
