#include "UIConfigurator.h"
#include "ui_UIConfigurator.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QApplication>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QPalette>
#include <QStandardPaths>
#include <QStyleFactory>


UIConfigurator::UIConfigurator(QWidget *parent) :
	QWidget(),
	m_parentWidget(parent),
	ui(new Ui::UIConfigurator)
{
	ui->setupUi(this);

	initView();
	initConnections();

	ui->resetPalette->click();
	ui->resetStyleSheet->click();
	ui->applyPalette->click();
}

UIConfigurator::~UIConfigurator()
{
	delete ui;
}

void UIConfigurator::initView()
{
	ui->le_11->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_12->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_13->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_21->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_22->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_23->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_31->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_32->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_33->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_41->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_42->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_43->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_51->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_52->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_53->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_61->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_62->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_63->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_71->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_72->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_73->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_81->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_82->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_83->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_91->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_92->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_93->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_101->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_102->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_103->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_111->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_112->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_113->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_121->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_122->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_123->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_131->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_132->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_133->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_141->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_142->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_143->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_151->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_152->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->le_153->setContextMenuPolicy(Qt::CustomContextMenu);
}

void UIConfigurator::initConnections()
{
	QVector<QLineEdit*> colors;
	colors << ui->le_11
		   << ui->le_12
		   << ui->le_13
		   << ui->le_21
		   << ui->le_22
		   << ui->le_23
		   << ui->le_31
		   << ui->le_32
		   << ui->le_33
		   << ui->le_41
		   << ui->le_42
		   << ui->le_43
		   << ui->le_51
		   << ui->le_52
		   << ui->le_53
		   << ui->le_61
		   << ui->le_62
		   << ui->le_63
		   << ui->le_71
		   << ui->le_72
		   << ui->le_73
		   << ui->le_81
		   << ui->le_82
		   << ui->le_83
		   << ui->le_91
		   << ui->le_92
		   << ui->le_93
		   << ui->le_101
		   << ui->le_102
		   << ui->le_103
		   << ui->le_111
		   << ui->le_112
		   << ui->le_113
		   << ui->le_121
		   << ui->le_122
		   << ui->le_123
		   << ui->le_131
		   << ui->le_132
		   << ui->le_133
		   << ui->le_141
		   << ui->le_142
		   << ui->le_143
		   << ui->le_151
		   << ui->le_152
		   << ui->le_153;
	for (QLineEdit* le : colors) {
		connect(le, &QLineEdit::textChanged, this, &UIConfigurator::updateLineEditBGColor);
		connect(le, &QLineEdit::customContextMenuRequested, this, &UIConfigurator::configureLineEditBGColor);
	}

	connect(ui->resetPalette, &QPushButton::clicked, [=] {
		//
		// Настроим стандартную палитру
		//
		QPalette palette = QStyleFactory::create("Fusion")->standardPalette();
		bool useDarkTheme =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/use-dark-theme",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt();
		if (useDarkTheme) {
			palette.setColor(QPalette::Window, QColor("#26282a"));
			palette.setColor(QPalette::WindowText, QColor("#ebebeb"));
			palette.setColor(QPalette::Button, QColor("#414244"));
			palette.setColor(QPalette::ButtonText, QColor("#ebebeb"));
			palette.setColor(QPalette::Base, QColor("#404040"));
			palette.setColor(QPalette::AlternateBase, QColor("#353535"));
			palette.setColor(QPalette::Text, QColor("#ebebeb"));
			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
			palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
			palette.setColor(QPalette::Light, QColor("#404040"));
			palette.setColor(QPalette::Midlight, QColor("#696765"));
			palette.setColor(QPalette::Dark, QColor("#696765"));
			palette.setColor(QPalette::Shadow, QColor("#1c2023"));

			palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#a1a1a1"));
			palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#1b1e21"));
			palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#a1a1a1"));
			palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#333333"));
			palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#bcbdbf"));
			palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#666769"));
			palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
		} else {
			palette.setColor(QPalette::Window, QColor("#f6f6f6"));
			palette.setColor(QPalette::WindowText, QColor("#38393a"));
			palette.setColor(QPalette::Button, QColor("#e4e4e4"));
			palette.setColor(QPalette::ButtonText, QColor("#38393a"));
			palette.setColor(QPalette::Base, QColor("#ffffff"));
			palette.setColor(QPalette::AlternateBase, QColor("#eeeeee"));
			palette.setColor(QPalette::Text, QColor("#38393a"));
			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
			palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
			palette.setColor(QPalette::Light, QColor("#ffffff"));
			palette.setColor(QPalette::Midlight, QColor("#d6d6d6"));
			palette.setColor(QPalette::Dark, QColor("#bdbebf"));
			palette.setColor(QPalette::Mid, QColor("#a0a2a4"));
			palette.setColor(QPalette::Shadow, QColor("#585a5c"));

			palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#acadaf"));
			palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#f6f6f6"));
			palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#acadaf"));
			palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#595a5c"));
			palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#acadaf"));
			palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
		}
		//
		// Установим цвета в форму
		//
		ui->le_11->setText(palette.color(QPalette::Active, QPalette::WindowText).name());
		ui->le_12->setText(palette.color(QPalette::Disabled, QPalette::WindowText).name());
		ui->le_13->setText(palette.color(QPalette::Inactive, QPalette::WindowText).name());
		ui->le_21->setText(palette.color(QPalette::Active, QPalette::Button).name());
		ui->le_22->setText(palette.color(QPalette::Disabled, QPalette::Button).name());
		ui->le_23->setText(palette.color(QPalette::Inactive, QPalette::Button).name());
		ui->le_31->setText(palette.color(QPalette::Active, QPalette::Light).name());
		ui->le_32->setText(palette.color(QPalette::Disabled, QPalette::Light).name());
		ui->le_33->setText(palette.color(QPalette::Inactive, QPalette::Light).name());
		ui->le_41->setText(palette.color(QPalette::Active, QPalette::Midlight).name());
		ui->le_42->setText(palette.color(QPalette::Disabled, QPalette::Midlight).name());
		ui->le_43->setText(palette.color(QPalette::Inactive, QPalette::Midlight).name());
		ui->le_51->setText(palette.color(QPalette::Active, QPalette::Dark).name());
		ui->le_52->setText(palette.color(QPalette::Disabled, QPalette::Dark).name());
		ui->le_53->setText(palette.color(QPalette::Inactive, QPalette::Dark).name());
		ui->le_61->setText(palette.color(QPalette::Active, QPalette::Mid).name());
		ui->le_62->setText(palette.color(QPalette::Disabled, QPalette::Mid).name());
		ui->le_63->setText(palette.color(QPalette::Inactive, QPalette::Mid).name());
		ui->le_71->setText(palette.color(QPalette::Active, QPalette::Text).name());
		ui->le_72->setText(palette.color(QPalette::Disabled, QPalette::Text).name());
		ui->le_73->setText(palette.color(QPalette::Inactive, QPalette::Text).name());
		ui->le_81->setText(palette.color(QPalette::Active, QPalette::BrightText).name());
		ui->le_82->setText(palette.color(QPalette::Disabled, QPalette::BrightText).name());
		ui->le_83->setText(palette.color(QPalette::Inactive, QPalette::BrightText).name());
		ui->le_91->setText(palette.color(QPalette::Active, QPalette::ButtonText).name());
		ui->le_92->setText(palette.color(QPalette::Disabled, QPalette::ButtonText).name());
		ui->le_93->setText(palette.color(QPalette::Inactive, QPalette::ButtonText).name());
		ui->le_101->setText(palette.color(QPalette::Active, QPalette::Base).name());
		ui->le_102->setText(palette.color(QPalette::Disabled, QPalette::Base).name());
		ui->le_103->setText(palette.color(QPalette::Inactive, QPalette::Base).name());
		ui->le_111->setText(palette.color(QPalette::Active, QPalette::Window).name());
		ui->le_112->setText(palette.color(QPalette::Disabled, QPalette::Window).name());
		ui->le_113->setText(palette.color(QPalette::Inactive, QPalette::Window).name());
		ui->le_121->setText(palette.color(QPalette::Active, QPalette::Shadow).name());
		ui->le_122->setText(palette.color(QPalette::Disabled, QPalette::Shadow).name());
		ui->le_123->setText(palette.color(QPalette::Inactive, QPalette::Shadow).name());
		ui->le_131->setText(palette.color(QPalette::Active, QPalette::Highlight).name());
		ui->le_132->setText(palette.color(QPalette::Disabled, QPalette::Highlight).name());
		ui->le_133->setText(palette.color(QPalette::Inactive, QPalette::Highlight).name());
		ui->le_141->setText(palette.color(QPalette::Active, QPalette::HighlightedText).name());
		ui->le_142->setText(palette.color(QPalette::Disabled, QPalette::HighlightedText).name());
		ui->le_143->setText(palette.color(QPalette::Inactive, QPalette::HighlightedText).name());
		ui->le_151->setText(palette.color(QPalette::Active, QPalette::AlternateBase).name());
		ui->le_152->setText(palette.color(QPalette::Disabled, QPalette::AlternateBase).name());
		ui->le_153->setText(palette.color(QPalette::Inactive, QPalette::AlternateBase).name());
	});
	connect(ui->savePalette, &QPushButton::clicked, [=] {
		QDir::root().mkpath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/KITScenaristUI/");
		const QString fileName =
				QFileDialog::getSaveFileName(this, tr("Save palette to file"),
					QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/KITScenaristUI/kiscenarist-custom-palette.txt");
		if (!fileName.isEmpty()) {
			QFile file(fileName);
			file.open(QIODevice::WriteOnly | QIODevice::Truncate);

			QString palette;
			palette.append(ui->le_11->text() + "\n");
			palette.append(ui->le_12->text() + "\n");
			palette.append(ui->le_13->text() + "\n");
			palette.append(ui->le_21->text() + "\n");
			palette.append(ui->le_22->text() + "\n");
			palette.append(ui->le_23->text() + "\n");
			palette.append(ui->le_31->text() + "\n");
			palette.append(ui->le_32->text() + "\n");
			palette.append(ui->le_33->text() + "\n");
			palette.append(ui->le_41->text() + "\n");
			palette.append(ui->le_42->text() + "\n");
			palette.append(ui->le_43->text() + "\n");
			palette.append(ui->le_51->text() + "\n");
			palette.append(ui->le_52->text() + "\n");
			palette.append(ui->le_53->text() + "\n");
			palette.append(ui->le_61->text() + "\n");
			palette.append(ui->le_62->text() + "\n");
			palette.append(ui->le_63->text() + "\n");
			palette.append(ui->le_71->text() + "\n");
			palette.append(ui->le_72->text() + "\n");
			palette.append(ui->le_73->text() + "\n");
			palette.append(ui->le_81->text() + "\n");
			palette.append(ui->le_82->text() + "\n");
			palette.append(ui->le_83->text() + "\n");
			palette.append(ui->le_91->text() + "\n");
			palette.append(ui->le_92->text() + "\n");
			palette.append(ui->le_93->text() + "\n");
			palette.append(ui->le_101->text() + "\n");
			palette.append(ui->le_102->text() + "\n");
			palette.append(ui->le_103->text() + "\n");
			palette.append(ui->le_111->text() + "\n");
			palette.append(ui->le_112->text() + "\n");
			palette.append(ui->le_113->text() + "\n");
			palette.append(ui->le_121->text() + "\n");
			palette.append(ui->le_122->text() + "\n");
			palette.append(ui->le_123->text() + "\n");
			palette.append(ui->le_131->text() + "\n");
			palette.append(ui->le_132->text() + "\n");
			palette.append(ui->le_133->text() + "\n");
			palette.append(ui->le_141->text() + "\n");
			palette.append(ui->le_142->text() + "\n");
			palette.append(ui->le_143->text() + "\n");
			palette.append(ui->le_151->text() + "\n");
			palette.append(ui->le_152->text() + "\n");
			palette.append(ui->le_153->text() + "\n");

			file.write(palette.toUtf8());
			file.close();
		}
	});
	connect(ui->loadPalette, &QPushButton::clicked, [=] {

		const QString fileName =
				QFileDialog::getOpenFileName(this, tr("Open file with palette"),
					  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/KITScenaristUI/kiscenarist-custom-palette.txt");
		if (!fileName.isEmpty()) {
			QFile file(fileName);
			file.open(QIODevice::ReadOnly);

			ui->le_11->setText(file.readLine().trimmed());
			ui->le_12->setText(file.readLine().trimmed());
			ui->le_13->setText(file.readLine().trimmed());
			ui->le_21->setText(file.readLine().trimmed());
			ui->le_22->setText(file.readLine().trimmed());
			ui->le_23->setText(file.readLine().trimmed());
			ui->le_31->setText(file.readLine().trimmed());
			ui->le_32->setText(file.readLine().trimmed());
			ui->le_33->setText(file.readLine().trimmed());
			ui->le_41->setText(file.readLine().trimmed());
			ui->le_42->setText(file.readLine().trimmed());
			ui->le_43->setText(file.readLine().trimmed());
			ui->le_51->setText(file.readLine().trimmed());
			ui->le_52->setText(file.readLine().trimmed());
			ui->le_53->setText(file.readLine().trimmed());
			ui->le_61->setText(file.readLine().trimmed());
			ui->le_62->setText(file.readLine().trimmed());
			ui->le_63->setText(file.readLine().trimmed());
			ui->le_71->setText(file.readLine().trimmed());
			ui->le_72->setText(file.readLine().trimmed());
			ui->le_73->setText(file.readLine().trimmed());
			ui->le_81->setText(file.readLine().trimmed());
			ui->le_82->setText(file.readLine().trimmed());
			ui->le_83->setText(file.readLine().trimmed());
			ui->le_91->setText(file.readLine().trimmed());
			ui->le_92->setText(file.readLine().trimmed());
			ui->le_93->setText(file.readLine().trimmed());
			ui->le_101->setText(file.readLine().trimmed());
			ui->le_102->setText(file.readLine().trimmed());
			ui->le_103->setText(file.readLine().trimmed());
			ui->le_111->setText(file.readLine().trimmed());
			ui->le_112->setText(file.readLine().trimmed());
			ui->le_113->setText(file.readLine().trimmed());
			ui->le_121->setText(file.readLine().trimmed());
			ui->le_122->setText(file.readLine().trimmed());
			ui->le_123->setText(file.readLine().trimmed());
			ui->le_131->setText(file.readLine().trimmed());
			ui->le_132->setText(file.readLine().trimmed());
			ui->le_133->setText(file.readLine().trimmed());
			ui->le_141->setText(file.readLine().trimmed());
			ui->le_142->setText(file.readLine().trimmed());
			ui->le_143->setText(file.readLine().trimmed());
			ui->le_151->setText(file.readLine().trimmed());
			ui->le_152->setText(file.readLine().trimmed());
			ui->le_153->setText(file.readLine().trimmed());

			file.close();
		}
	});
	connect(ui->applyPalette, &QPushButton::clicked, this, &UIConfigurator::applyConfiguration);

	connect(ui->resetStyleSheet, &QPushButton::clicked, [=] {
		QFile file(":/Interface/UI/style-desktop.qss");
		file.open(QIODevice::ReadOnly);
		ui->styleSheet->clear();
		ui->styleSheet->setPlainText(file.readAll());
		file.close();
	});
	connect(ui->saveStyleSheet, &QPushButton::clicked, [=] {
		QDir::root().mkpath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/KITScenaristUI/");
		const QString fileName =
				QFileDialog::getSaveFileName(this, tr("Save style sheet to file"),
					  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/KITScenaristUI/kiscenarist-custom-stylesheet.txt");
		if (!fileName.isEmpty()) {
			QFile file(fileName);
			file.open(QIODevice::WriteOnly | QIODevice::Truncate);
			file.write(ui->styleSheet->toPlainText().toUtf8());
			file.close();
		}
	});
	connect(ui->loadStyleSheet, &QPushButton::clicked, [=] {
		const QString fileName =
				QFileDialog::getOpenFileName(this, tr("Open file with style sheet"),
					QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/KITScenaristUI/kiscenarist-custom-stylesheet.txt");
		if (!fileName.isEmpty()) {
			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			ui->styleSheet->clear();
			ui->styleSheet->setPlainText(file.readAll());
			file.close();
		}
	});
	connect(ui->applyStyleSheet, &QPushButton::clicked, this, &UIConfigurator::applyConfiguration);
}

void UIConfigurator::applyConfiguration()
{
	QPalette palette = QStyleFactory::create("Fusion")->standardPalette();
	palette.setColor(QPalette::Active, QPalette::WindowText, QColor(ui->le_11->text()));
	palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(ui->le_12->text()));
	palette.setColor(QPalette::Inactive, QPalette::WindowText, QColor(ui->le_13->text()));
	palette.setColor(QPalette::Active, QPalette::Button, QColor(ui->le_21->text()));
	palette.setColor(QPalette::Disabled, QPalette::Button, QColor(ui->le_22->text()));
	palette.setColor(QPalette::Inactive, QPalette::Button, QColor(ui->le_23->text()));
	palette.setColor(QPalette::Active, QPalette::Light, QColor(ui->le_31->text()));
	palette.setColor(QPalette::Disabled, QPalette::Light, QColor(ui->le_32->text()));
	palette.setColor(QPalette::Inactive, QPalette::Light, QColor(ui->le_33->text()));
	palette.setColor(QPalette::Active, QPalette::Midlight, QColor(ui->le_41->text()));
	palette.setColor(QPalette::Disabled, QPalette::Midlight, QColor(ui->le_42->text()));
	palette.setColor(QPalette::Inactive, QPalette::Midlight, QColor(ui->le_43->text()));
	palette.setColor(QPalette::Active, QPalette::Dark, QColor(ui->le_51->text()));
	palette.setColor(QPalette::Disabled, QPalette::Dark, QColor(ui->le_52->text()));
	palette.setColor(QPalette::Inactive, QPalette::Dark, QColor(ui->le_53->text()));
	palette.setColor(QPalette::Active, QPalette::Mid, QColor(ui->le_61->text()));
	palette.setColor(QPalette::Disabled, QPalette::Mid, QColor(ui->le_62->text()));
	palette.setColor(QPalette::Inactive, QPalette::Mid, QColor(ui->le_63->text()));
	palette.setColor(QPalette::Active, QPalette::Text, QColor(ui->le_71->text()));
	palette.setColor(QPalette::Disabled, QPalette::Text, QColor(ui->le_72->text()));
	palette.setColor(QPalette::Inactive, QPalette::Text, QColor(ui->le_73->text()));
	palette.setColor(QPalette::Active, QPalette::BrightText, QColor(ui->le_81->text()));
	palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(ui->le_82->text()));
	palette.setColor(QPalette::Inactive, QPalette::BrightText, QColor(ui->le_83->text()));
	palette.setColor(QPalette::Active, QPalette::ButtonText, QColor(ui->le_91->text()));
	palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(ui->le_92->text()));
	palette.setColor(QPalette::Inactive, QPalette::ButtonText, QColor(ui->le_93->text()));
	palette.setColor(QPalette::Active, QPalette::Base, QColor(ui->le_101->text()));
	palette.setColor(QPalette::Disabled, QPalette::Base, QColor(ui->le_102->text()));
	palette.setColor(QPalette::Inactive, QPalette::Base, QColor(ui->le_103->text()));
	palette.setColor(QPalette::Active, QPalette::Window, QColor(ui->le_111->text()));
	palette.setColor(QPalette::Disabled, QPalette::Window, QColor(ui->le_112->text()));
	palette.setColor(QPalette::Inactive, QPalette::Window, QColor(ui->le_113->text()));
	palette.setColor(QPalette::Active, QPalette::Shadow, QColor(ui->le_121->text()));
	palette.setColor(QPalette::Disabled, QPalette::Shadow, QColor(ui->le_122->text()));
	palette.setColor(QPalette::Inactive, QPalette::Shadow, QColor(ui->le_123->text()));
	palette.setColor(QPalette::Active, QPalette::Highlight, QColor(ui->le_131->text()));
	palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(ui->le_132->text()));
	palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(ui->le_133->text()));
	palette.setColor(QPalette::Active, QPalette::HighlightedText, QColor(ui->le_141->text()));
	palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(ui->le_142->text()));
	palette.setColor(QPalette::Inactive, QPalette::HighlightedText, QColor(ui->le_143->text()));
	palette.setColor(QPalette::Active, QPalette::AlternateBase, QColor(ui->le_151->text()));
	palette.setColor(QPalette::Disabled, QPalette::AlternateBase, QColor(ui->le_152->text()));
	palette.setColor(QPalette::Inactive, QPalette::AlternateBase, QColor(ui->le_153->text()));

	//
	// Применяем палитру
	//
	qApp->setPalette(palette);

	//
	// Чтобы все цветовые изменения подхватились, нужно заново переустановить стиль
	//
	m_parentWidget->setStyleSheet(ui->styleSheet->toPlainText());
	setStyleSheet(ui->styleSheet->toPlainText());
}

void UIConfigurator::updateLineEditBGColor()
{
	if (QLineEdit* le = qobject_cast<QLineEdit*>(sender())) {
		QString color = le->text();
		if (color.isEmpty()) {
			color = "ffffff";
		}
		le->setStyleSheet("background-color:" + color);
	}
}

void UIConfigurator::configureLineEditBGColor()
{
	if (QLineEdit* le = qobject_cast<QLineEdit*>(sender())) {
		const QColor color = QColorDialog::getColor(QColor(le->text()), this, tr("Choose color"));
		if (color.isValid()) {
			le->setText(color.name());
		}
	}
}
