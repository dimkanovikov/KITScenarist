#ifndef DIGIPITCHSCRIPTSLIST_H
#define DIGIPITCHSCRIPTSLIST_H

#include <QLabel>
#include <QScrollArea>

class QPropertyAnimation;


namespace UserInterface
{
    class DigipitchScriptCard : public QLabel
    {
        Q_OBJECT

    public:
        explicit DigipitchScriptCard(const QString& _name, const QString& _genre, const QString& _author,
                                     const QString& _mediaJsonUrl, const QString& _scriptUrl, QWidget* _parent = nullptr);
        ~DigipitchScriptCard();

        /**
         * @brief Получить ссылку на сценарий
         */
        QString url() const { return m_url; }

    signals:
        /**
         * @brief На карточку кликнули
         */
        void clicked();

    protected:
        /**
         * @brief Переопределяем для испускания сигнала нажатия на карточке
         */
        void mouseReleaseEvent(QMouseEvent* _event) override;

        /**
         * @brief Переопределяем чтобы отображать/скрывать кнопки добавления/удаления
         */
        /** @{ */
        void enterEvent(QEvent* event) override;
        void leaveEvent(QEvent* event) override;
        /** @} */

    private:
        void mediaJsonDownloaded(const QByteArray& _jsonData);
        void mediaImageDownloaded(const QByteArray& _imageData);

    private:
        /**
         * @brief Виджет с описанием карточки
         */
        QFrame* m_descriptionFrame = nullptr;

        /**
         * @brief Ссылка на сценарий
         */
        const QString m_url;

        /**
         * @brief Анимация выделения карточки при наведении мышью
         */
        QPropertyAnimation* m_animation = nullptr;
    };

    /**
     * @brief Список последних сценариев проекта Digipitch
     */
    class DigipitchScriptsList : public QScrollArea
    {
        Q_OBJECT

    public:
        explicit DigipitchScriptsList(QWidget* _parent = nullptr);

        /**
         * @brief Загрузить сценарии
         */
        void loadScripts();

    private:
        /**
         * @brief Загружен список сценариев
         */
        void scriptsLoaded(const QByteArray& _data);

    private:
        /**
         * @brief Настроить представление
         */
        void initView();
    };
}

#endif // DIGIPITCHSCRIPTSLIST_H
