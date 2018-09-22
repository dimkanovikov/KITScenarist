#include "DigipitchScriptsList.h"

#include <3rd_party/Widgets/ImagesPane/FlowLayout.h>

#include <NetworkRequestLoader.h>

#include <QDesktopServices>
#include <QGraphicsDropShadowEffect>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

using UserInterface::DigipitchScriptsList;


UserInterface::DigipitchScriptCard::DigipitchScriptCard(const QString& _name, const QString& _genre,
                                                        const QString& _author, const QString& _mediaJsonUrl,
                                                        const QString& _scriptUrl, QWidget* _parent)
    : QLabel(_parent),
      m_descriptionFrame(new QFrame(this)),
      m_url(_scriptUrl),
      m_animation(new QPropertyAnimation(this))
{
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("QLabel { background-color: transparent; border-radius: 4px; } ");
    setAttribute(Qt::WA_Hover);
    setPixmap(QPixmap(":/Graphics/Images/screenplay-poster.png"));
    setScaledContents(true);
    setFixedSize(225, 336);
    //
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
    effect->setColor(Qt::transparent);
    effect->setBlurRadius(18);
    effect->setOffset(3, 5);
    setGraphicsEffect(effect);

    m_descriptionFrame->hide();

    m_animation->setTargetObject(effect);
    m_animation->setPropertyName("color");
    m_animation->setStartValue(QColor(Qt::transparent));
    m_animation->setEndValue(QColor(Qt::black));
    m_animation->setDirection(QPropertyAnimation::Backward);
    m_animation->setDuration(160);


    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->addStretch();
    {
        m_descriptionFrame->setObjectName("descriptionFrame");
        m_descriptionFrame->setStyleSheet("#descriptionFrame { background-color: rgba(0, 0, 0, 240); }"
                                          "QLabel { color: white; }");
        layout->addWidget(m_descriptionFrame);

        QVBoxLayout* descriptionLayout = new QVBoxLayout;
        descriptionLayout->addWidget(new QLabel(QString("<b>%1</b>").arg(_name), this));
        descriptionLayout->addWidget(new QLabel(_genre, this));
        descriptionLayout->addWidget(new QLabel(tr("Written by %1").arg(_author)));
        m_descriptionFrame->setLayout(descriptionLayout);
    }
    setLayout(layout);


    if (_mediaJsonUrl.isEmpty()) {
        return;
    }
    NetworkRequestLoader::loadAsync(QUrl(_mediaJsonUrl), this, &DigipitchScriptCard::mediaJsonDownloaded);
}

UserInterface::DigipitchScriptCard::~DigipitchScriptCard()
{
    setGraphicsEffect(nullptr);
}

void UserInterface::DigipitchScriptCard::mouseReleaseEvent(QMouseEvent* _event)
{
    QLabel::mouseReleaseEvent(_event);

    emit clicked();
}

void UserInterface::DigipitchScriptCard::enterEvent(QEvent* event)
{
    QLabel::enterEvent(event);

    if (m_animation->direction() != QPropertyAnimation::Forward) {
        m_descriptionFrame->show();
        m_animation->setDirection(QPropertyAnimation::Forward);
        m_animation->start();
    }
}

void UserInterface::DigipitchScriptCard::leaveEvent(QEvent* event)
{
    QLabel::leaveEvent(event);

    // Запланируем проверку необходимости убрать тень, это необходимо чтобы сперва выполнились
    // все события с идентификацией перемещением курсора и следующий виджет под курсором
    // обновил свой статус
    QTimer::singleShot(0, this, [this] {
        m_descriptionFrame->hide();
        m_animation->setDirection(QPropertyAnimation::Backward);
        m_animation->start();
    });
}

void UserInterface::DigipitchScriptCard::mediaJsonDownloaded(const QByteArray& _jsonData)
{
    const auto mediaDocument = QJsonDocument::fromJson(_jsonData);
    const auto mediaImage = mediaDocument.object()["media_details"].toObject()["sizes"].toObject()["full"].toObject();
    const auto coverUrl = mediaImage["source_url"].toString();
    if (coverUrl.isEmpty()) {
        return;
    }

    NetworkRequestLoader::loadAsync(QUrl(coverUrl), this, &DigipitchScriptCard::mediaImageDownloaded);
}

void UserInterface::DigipitchScriptCard::mediaImageDownloaded(const QByteArray& _imageData)
{
    QPixmap coverImage;
    coverImage.loadFromData(_imageData);
    setPixmap(coverImage);
}


// ****


DigipitchScriptsList::DigipitchScriptsList(QWidget* _parent)
    : QScrollArea(_parent)
{
    initView();

    QTimer::singleShot(0, this, &DigipitchScriptsList::loadScripts);
}

void UserInterface::DigipitchScriptsList::loadScripts()
{
    NetworkRequestLoader::loadAsync(QUrl("https://digipitch.com//wp-json/wp/v2/screenplays/"), this, &DigipitchScriptsList::scriptsLoaded);
}

void DigipitchScriptsList::scriptsLoaded(const QByteArray& _data)
{
    FlowLayout* layout = qobject_cast<FlowLayout*>(widget()->layout());

    //
    // Удаляем старые виджеты
    //
    while (layout->count() > 0) {
        layout->takeAt(0)->widget()->deleteLater();
    }

    //
    // Добавляем новые
    //
    const QJsonDocument document = QJsonDocument::fromJson(_data);
    const auto scripts = document.array();
    for (int scriptIndex = 0; scriptIndex < scripts.size(); ++scriptIndex) {
        const auto script = scripts.at(scriptIndex).toObject();
        const auto scriptName = script["title"].toObject()["rendered"].toString();
        const auto scriptGenre = [&script] () -> QString {
            QString genres;
            for (auto genre : script["script_genre"].toArray()) {
                if (!genres.isEmpty()) {
                    genres.append(", ");
                }
                genres.append(genre.toString());
            }
            return genres;
        } ();
        const auto scriptAuthor = script["script_screenwriter"].toArray().first().toObject()["post_title"].toString();
        const QString scriptCoverUrl = [&script] () -> QString {
            const auto mediaLinks = script["_links"].toObject()["wp:featuredmedia"].toArray();
            if (mediaLinks.size() > 0) {
                return mediaLinks.first().toObject()["href"].toString();
            }
            return QString{};
        } ();
        const auto scriptUrl = script["link"].toString();

        auto scriptCard = new DigipitchScriptCard(scriptName, scriptGenre, scriptAuthor, scriptCoverUrl, scriptUrl, this);
        connect(scriptCard, &DigipitchScriptCard::clicked, this, [scriptCard] { QDesktopServices::openUrl(scriptCard->url()); });
        layout->addWidget(scriptCard);
    }
}

void DigipitchScriptsList::initView()
{
    FlowLayout* layout = new FlowLayout(0, 24, 24);
    QWidget* content = new QWidget;
    content->setLayout(layout);

    setWidget(content);
    setWidgetResizable(true);
}
