#include "ScenarioNavigatorItemDelegate.h"
#include "ScenarioNavigatorItemWidget.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <QApplication>
#include <QPainter>

using UserInterface::ScenarioNavigatorItemDelegate;
using UserInterface::ScenarioNavigatorItemWidget;


ScenarioNavigatorItemDelegate::ScenarioNavigatorItemDelegate(QObject* _parent) :
	QStyledItemDelegate(_parent),
	m_itemWidget(new ScenarioNavigatorItemWidget),
	m_showSceneNumber(false),
	m_showSceneDescription(true),
	m_sceneDescriptionIsSceneText(true),
	m_sceneDescriptionHeight(1)
{
}

ScenarioNavigatorItemDelegate::~ScenarioNavigatorItemDelegate()
{
	delete m_itemWidget;
}

void ScenarioNavigatorItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	//
	// Получим данные
	//
	QVariant icon = _index.data(Qt::DecorationRole);
	QVariant header = _index.data(Qt::DisplayRole);
	QVariant sceneText = _index.data(BusinessLogic::ScenarioModel::SceneTextIndex);
	QVariant synopsys = _index.data(BusinessLogic::ScenarioModel::SynopsisIndex);
	QVariant duration = _index.data(BusinessLogic::ScenarioModel::DurationIndex);
	QVariant sceneNumber = _index.data(BusinessLogic::ScenarioModel::SceneNumberIndex);

	//
	// Установим в виджет данные
	//
	// ... иконку
	//
	m_itemWidget->setIcon(icon.value<QPixmap>());
	//
	// ... заголовок
	//
	if (m_showSceneNumber
		&& !sceneNumber.isNull()) {
		m_itemWidget->setHeader(sceneNumber.toString() + ". " + header.toString());
	} else {
		m_itemWidget->setHeader(header.toString());
	}
	//
	// ... длительность
	//
	m_itemWidget->setDuration(duration.toInt());
	//
	// ... описание
	//
	if (m_showSceneDescription) {
		if (m_sceneDescriptionIsSceneText) {
			m_itemWidget->setDescription(sceneText.toString());
		} else {
			m_itemWidget->setDescription(synopsys.toString());
		}
	}

	//
	// Получим настройки стиля
	//
	QStyleOptionViewItemV4 opt = _option;
	initStyleOption(&opt, _index);

	//
	// Настраиваем размер области отрисовки
	//
	m_itemWidget->resize(opt.rect.size());

	//
	// Настраиваем палитру отображения
	//
	QPalette palette;

	//
	// Для выделенного объекта
	//
	if(opt.state.testFlag(QStyle::State_Selected))
	{
		palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::Highlight));
		palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::HighlightedText));
	}
	//
	// ... для остальных
	//
	else
	{
		//
		// Реализация альтернативных цветов в представлении
		//
		if(opt.features.testFlag(QStyleOptionViewItemV2::Alternate))
		{
			palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::AlternateBase));
			palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::WindowText));
		}
		else
		{
			palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::Base));
			palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::WindowText));
		}
	}

	//
	// Устанавливаем палитру виджету и всем его детям
	//
	m_itemWidget->setPalette(palette);
	foreach (QWidget* itemChild, m_itemWidget->findChildren<QWidget*>()) {
		itemChild->setPalette(palette);
	}

	//
	// Отрисовываем виджет
	//
	_painter->save();
	_painter->setRenderHint(QPainter::Antialiasing, true);
	_painter->translate(opt.rect.topLeft());

	m_itemWidget->render(_painter);

	_painter->restore();
}

QSize ScenarioNavigatorItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	Q_UNUSED(_option);
	Q_UNUSED(_index);

	return m_itemWidget->minimumSizeHint();
}

void ScenarioNavigatorItemDelegate::setShowSceneNumber(bool _show)
{
	if (m_showSceneNumber != _show) {
		m_showSceneNumber = _show;
	}
}

void ScenarioNavigatorItemDelegate::setShowSceneDescription(bool _show)
{
	if (m_showSceneDescription != _show) {
		m_showSceneDescription = _show;

		updateWidgetView();
	}
}

void ScenarioNavigatorItemDelegate::setSceneDescriptionIsSceneText(bool _isSceneText)
{
	if (m_sceneDescriptionIsSceneText != _isSceneText) {
		m_sceneDescriptionIsSceneText = _isSceneText;
	}
}

void ScenarioNavigatorItemDelegate::setSceneDescriptionHeight(int _height)
{
	if (m_sceneDescriptionHeight != _height) {
		m_sceneDescriptionHeight = _height;

		updateWidgetView();
	}
}

void ScenarioNavigatorItemDelegate::updateWidgetView()
{
	//
	// Настроим виджет
	//
	ScenarioNavigatorItemWidget::Type widgetType =
			m_showSceneDescription
			? ScenarioNavigatorItemWidget::HeaderAndDescription
			: ScenarioNavigatorItemWidget::OnlyHeader;
	m_itemWidget->setType(widgetType, m_sceneDescriptionHeight);
}
