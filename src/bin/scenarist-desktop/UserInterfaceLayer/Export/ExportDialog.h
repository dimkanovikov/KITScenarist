#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

class QAbstractItemModel;

namespace Ui {
    class ExportDialog;
}

namespace BusinessLogic {
    class ExportParameters;
}


namespace UserInterface
{
    /**
     * @brief Диалог экспорта
     */
    class ExportDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit ExportDialog(QWidget* _parent = 0);
        ~ExportDialog();

        /**
         * @brief Установить тип экспорта
         */
        void setExportType(int _type);

        /**
         * @brief Установить путь экспортируемого файла разработки
         */
        void setResearchExportFilePath(const QString& _filePath);

        /**
         * @brief Установить имя экспортируемого файла разработки
         */
        void setResearchExportFileName(const QString& _fileName);

        /**
         * @brief Установить модель документов для экспорта
         */
        void setResearchModel(QAbstractItemModel* _model);

        /**
         * @brief Установить путь экспортируемого файла скрипта
         */
        void setScriptExportFilePath(const QString& _filePath);

        /**
         * @brief Установить имя экспортируемого файла скрипта
         */
        void setScriptExportFileName(const QString& _fileName);

        /**
         * @brief Установить необходимость проверять переносы страниц
         */
        void setCheckPageBreaks(bool _check);

        /**
         * @brief Установить модель стилей
         */
        void setStylesModel(QAbstractItemModel* _model);

        /**
         * @brief Установить текущий стиль
         */
        void setCurrentStyle(const QString& _styleName);

        /**
         * @brief Установить нумеровать ли страницы
         */
        void setPageNumbering(bool _isChecked);

        /**
         * @brief Установить нумеровать ли сцены
         */
        void setScenesNumbering(bool _isChecked);

        /**
         * @brief Установить нумеровать ли реплики
         */
        void setDialoguesNumbering(bool _isChecked);

        /**
         * @brief Установить приставку сцен
         */
        void setScenesPrefix(const QString& _prefix);

        /**
         * @brief Установить необходимость сохранения редакторских пометок
         */
        void setSaveReviewMarks(bool _save);

        /**
         * @brief Установить печатать ли титульный лист
         */
        void setPrintTitle(bool _isChecked);

        /**
         * @brief Получить настройки экспорта
         */
        BusinessLogic::ExportParameters exportParameters() const;

        /**
         * @brief Текущий тип экспорта
         */
        int exportType() const;

        /**
         * @brief Получить путь к экспортируемому файлу разработки
         */
        QString researchFilePath() const;

        /**
         * @brief Получить путь к экспортируемому файлу текста
         */
        QString scriptFilePath() const;

        /**
         * @brief Формат экспорта
         */
        QString exportFormat() const;

        /**
         * @brief Переопределяем, чтобы настроить тип экспорта, перед отображением
         */
        void setVisible(bool _visible);

    signals:
        /**
         * @brief Сменился стиль
         */
        void currentStyleChanged(const QString& _styleName);

        /**
         * @brief Показать предварительный просмотр
         */
        void printPreview();

    private:
        /**
         * @brief Выбрать файл для экспорта разработки
         */
        void chooseResearchFile();

        /**
         * @brief Сменился формат
         */
        void updateScriptFileFormat();

        /**
         * @brief Выбрать файл для экспорта текста
         */
        void chooseScriptFile();

        /**
         * @brief При смене имени файла, обновить доступность кнопки экспорта,
         *		  а также проверить не будет ли произведено пересохранение
         */
        /** @{ */
        void checkResearchExportAvailability();
        void checkScriptExportAvailability();
        void checkExportAvailability(int _index);
        /** @} */

        /**
         * @brief Обновить видимость параметров экспорта в зависимости от экспортируемого типа
         */
        void updateParametersVisibility();

    private:
        /**
         * @brief Настроить представление
         */
        void initView() override;

        /**
         * @brief Настроить соединения
         */
        void initConnections() override;

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

        /**
         * @brief Переопределяем, чтобы возвратить таббар для заголовка
         */
        QWidget* titleWidget() const override;

    private:
        /**
         * @brief Интерфейс
         */
        Ui::ExportDialog* m_ui;

        /**
         * @brief Тип экспорта
         */
        int m_exportType = 0;

        /**
         * @brief Имя файла для экспорта
         */
        /** @{ */
        QString m_researchExportFileName;
        QString m_scriptExportFileName;
        /** @} */
    };
}

#endif // EXPORTDIALOG_H
