#ifndef FOUNTAINIMPORTER_H
#define FOUNTAINIMPORTER_H

#include "AbstractImporter.h"
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

class QXmlStreamWriter;

namespace BusinessLogic
{
    /**
     * @brief Импортер Fountain-документов
     */
    class FountainImporter : public AbstractImporter
    {
        /*
                      . .
                     ` ' `
                 .'''. ' .'''.
                   .. ' ' ..
                  '  '.'.'  '
                  .'''.'.'''
                 ' .''.'.''. '
                   . . : . .
              {} _'___':'___'_ {}
              ||(_____________)||
              """"""(     )""""""
                    _)   (_             .^-^.  ~""~
                   (_______)~~"""~~     '._.'
               ~""~                     .' '.
                                        '.,.'
                                           `'`'
         */
    public:
        FountainImporter();

        /**
         * @brief Импорт сценария из документа
         */
        QString importScenario(const ImportParameters &_importParameters) const;

    private:
        /**
         * @brief Обработка конкретного блока перед его добавлением
         */
        void writeBlock(QXmlStreamWriter &writer, QString paragraphText,
                        ScenarioBlockStyle::Type blockStyle) const;

        /**
         * @brief Добавление блока
         */
        void reallyWriteBlock(QXmlStreamWriter &writer, const QString& paragraphText,
                              ScenarioBlockStyle::Type blockStyle) const;

        /**
         * @brief Добавление комментариев к блоку
         */
        void writeComments(QXmlStreamWriter &writer) const;

        //
        // Чтобы не передавать большое число параметров в функции, используются члены класса
        //

        /**
         * @brief Начало позиции в блоке для потенциальной будущей редакторской заметки
         */
        mutable unsigned prevBlockStart;

        /**
         * @brief Длина потенциальной будущей редакторской заметки
         */
        mutable unsigned prevBlockLen;

        /**
         * @brief Идет ли сейчас редакторская заметка
         */
        mutable bool noting = false;

        /**
         * @brief Идет ли сейчас комментарий
         */
        mutable bool commenting = false;

        /**
         * @brief Является ли текущий блок первым
         */
        mutable bool firstBlock = true;

        /**
         * @brief Текст блока (на случай multiline комментариев)
         */
        mutable QString text;

        /**
         * @brief Текст редакторской заметки (на случай multiline)
         */
        mutable QString comment;

        /**
         * @brief Редакторская заметка к текущему блоку
         * 		  tuple содержит комментарий, позиция и длина области редакторской заметки
         */
        mutable QVector<std::tuple<QString, unsigned, unsigned> > comments;
    };
}

#endif // FOUNTAINIMPORTER_H
