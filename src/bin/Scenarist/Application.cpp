#include "Application.h"

#include <ManagementLayer/ApplicationManager.h>

#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QStyle>
#include <QStyleFactory>
#include <QTranslator>


Application::Application(int& _argc, char** _argv) :
	QApplication(_argc, _argv),
	m_applicationManager(0)
{
	//
	// Настроим информацию о приложении
	//
	setOrganizationName("DimkaNovikov labs.");
	setOrganizationDomain("dimkanovikov.pro");
	setApplicationName("Scenarist");
	setApplicationVersion("0.4.5");

	//
	// Настроим стиль отображения внешнего вида приложения
	//
	setStyle(QStyleFactory::create("Fusion"));
	//
	// FIXME: почему-то ссылки не меняют цвет, если обновлять палитру на лету,
	// поэтому устанавливаю при загрузке приложения такой цвет, который подходит
	// к обеим темам
	//
	QPalette palette = QApplication::style()->standardPalette();
	palette.setColor(QPalette::Link, QColor("#0C6ACC"));
	setPalette(palette);

	//
	// Загрузим Courier New в базу шрифтов программы, если его там ещё нет
	//
	QFontDatabase fontDatabase;
	fontDatabase.addApplicationFont(":/Fonts/Courier New");

	//
	// Подключим файл переводов Qt
	//
	QTranslator* russianQtTranslator = new QTranslator;
	russianQtTranslator->load(":/Translations/Translations/qt_ru.qm");
	installTranslator(russianQtTranslator);

	//
	// Подключим дополнительный файл переводов Qt
	//
	QTranslator* russianQtBaseTranslator = new QTranslator;
	russianQtBaseTranslator->load(":/Translations/Translations/qtbase_ru.qm");
	installTranslator(russianQtBaseTranslator);

	//
	// Подключим файл переводов программы
	//
	QTranslator* russianTranslator = new QTranslator;
	russianTranslator->load(":/Translations/Translations/Scenarist_ru.qm");
	installTranslator(russianTranslator);
}

void Application::setupManager(ManagementLayer::ApplicationManager *_manager)
{
	m_applicationManager = _manager;
}

bool Application::event(QEvent *_event)
{
	bool result = true;
	if (_event->type() == QEvent::FileOpen
		&& m_applicationManager != 0) {
		QFileOpenEvent* fileOpenEvent = static_cast<QFileOpenEvent*>(_event);
		m_applicationManager->openFile(fileOpenEvent->file());
	} else {
		result = QApplication::event(_event);
	}

	return result;
}
