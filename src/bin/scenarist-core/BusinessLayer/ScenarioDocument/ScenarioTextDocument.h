#ifndef SCENARIOTEXTDOCUMENT_H
#define SCENARIOTEXTDOCUMENT_H

#include "ScenarioTemplate.h"
#include <3rd_party/Helpers/DiffMatchPatchHelper.h>

#include <QTextDocument>
#include <QTextCursor>

namespace Domain {
    class ScenarioChange;
}

class QDomNodeList;

namespace BusinessLogic
{
    class ScenarioReviewModel;
    class ScenarioXml;


    /**
     * @brief Расширение класса текстового документа для предоставления интерфейса для обработки mime
     */
    class ScenarioTextDocument : public QTextDocument
    {
        Q_OBJECT

    public:
        /**
         * @brief Обновить ревизию блока
         * @note Это приходится делать вручную, т.к. изменения пользовательских свойств блока
         *		 не отслеживаются автоматически
         */
        static void updateBlockRevision(QTextCursor& _cursor);

    public:
        explicit ScenarioTextDocument(QObject *parent, ScenarioXml* _xmlHandler);

        /**
         * @brief Сформировать xml из сценария и рассчитать его хэш
         */
        void updateScenarioXml();

        /**
         * @brief Получить xml сценария
         */
        QString scenarioXml() const;

        /**
         * @brief Получить текущий хэш сценария
         */
        QByteArray scenarioXmlHash() const;

        /**
         * @brief Загрузить сценарий
         */
        void load(const QString& _scenarioXml);

        /**
         * @brief Получить майм представление данных в указанном диапазоне
         */
        QString mimeFromSelection(int _startPosition, int _endPosition) const;

        /**
         * @brief Вставить данные в указанную позицию документа
         */
        void insertFromMime(int _insertPosition, const QString& _mimeData);

        /**
         * @brief Применить патч
         */
        void applyPatch(const QString& _patch);

        /**
         * @brief Применить множество патчей
         * @note Метод для оптимизации, перестраивается весь документ
         */
        void applyPatches(const QList<QString>& _patches);

        /**
         * @brief Сохранить изменения текста
         */
        Domain::ScenarioChange* saveChanges();

        /**
         * @brief Собственные реализации отмены/повтора последнего действия
         */
        /** @{ */
        void undoReimpl();
        void redoReimpl();
        /** @} */

        /**
         * @brief Собственные реализации проверки доступности отмены/повтора последнего действия
         */
        /** @{ */
        bool isUndoAvailableReimpl() const;
        bool isRedoAvailableReimpl() const;
        /** @} */

        /**
         * @brief Установить курсор в заданную позицию
         */
        void setCursorPosition(QTextCursor& _cursor, int _position,
            QTextCursor::MoveMode _moveMode = QTextCursor::MoveAnchor);

        /**
         * @brief Получить модель редакторсках правок
         */
        ScenarioReviewModel* reviewModel() const;

        /**
         * @brief Документ в режиме отображения поэпизодника или сценария
         */
        bool outlineMode() const;

        /**
         * @brief Установить режим отображения поэпизодника или сценария
         */
        void setOutlineMode(bool _outlineMode);

        /**
         * @brief Получить список видимых блоков в зависимости от режима отображения поэпизодника или сценария
         */
        QList<BusinessLogic::ScenarioBlockStyle::Type> visibleBlocksTypes() const;

    signals:
        /**
         * @brief Сигналы уведомляющие об этапах применения патчей
         */
        /** @{ */
        void beforePatchApply();
        void afterPatchApply();
        /** @} */

        /**
         * @brief В документ были внесены редакторские примечания
         */
        void reviewChanged();

    private:
        /**
         * @brief Процедура удаления одинаковый первых и последних частей в xml-строках у _xmls
         * _reversed = false - удаляем первые, = true - удаляем последние
         */
        void removeIdenticalParts(QPair<DiffMatchPatchHelper::ChangeXml, DiffMatchPatchHelper::ChangeXml>& _xmls, bool _reversed);

        /**
         * @brief Обновляет в структуре ChangeXml поля plainLength и plainPos
         */
        void processLenghtPos(DiffMatchPatchHelper::ChangeXml& _xmls, int _k, bool _reversed);

        /**
         * @brief Возвращает позицию следующего тега после _prev, имеющего внутри себя тег <v>
         */
        int getNextChild(QDomNodeList& _list, int _prev);

        /**
         * @brief Возвращает позицию предыдущего тега перед _prev, имеющего внутри себя тек <v>
         */
        int getPrevChild(QDomNodeList& _list, int _prev);

    private:
        /**
         * @brief Обработчик xml
         */
        ScenarioXml* m_xmlHandler;

        /**
         * @brief Применяется ли патч в данный момент
         */
        bool m_isPatchApplyProcessed;

        /**
         * @brief  Xml текст сценария и его MD5-хэш
         */
        /** @{ */
        QString m_scenarioXml;
        QByteArray m_scenarioXmlHash;
        /** @} */

        /**
         * @brief Xml текст сценария и его MD5-хэш на момент последнего сохранения изменений
         */
        /** @{ */
        QString m_lastSavedScenarioXml;
        QByteArray m_lastSavedScenarioXmlHash;
        /** @} */

        /**
         * @brief Стеки для отмены/повтора последнего действия
         */
        /** @{ */
        QList<Domain::ScenarioChange*> m_undoStack;
        QList<Domain::ScenarioChange*> m_redoStack;
        /** @{ */

        /**
         * @brief Модель редакторских правок документа
         */
        ScenarioReviewModel* m_reviewModel;

        /**
         * @brief Включён ли режим отображения поэпизодного плана (true) или сценария (false)
         */
        bool m_outlineMode;
    };
}


#endif // SCENARIOTEXTDOCUMENT_H
