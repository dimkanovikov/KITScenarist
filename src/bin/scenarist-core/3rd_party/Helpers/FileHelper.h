#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QString>


/**
 * @brief Вспомогательные функции для работы с файлами
 */
class FileHelper
{
public:
    /**
     * @brief Получить имя файла, которое можно сохранить в системе
     */
    static QString systemSavebleFileName(const QString& _fileName) {
        QString result = _fileName;
#ifdef Q_OS_WIN
        // Не надо заменять двоеточие после названия диска
        if (result.size() > 4) {
            result = result.left(4) + result.mid(4).replace("\"", "_").replace(":", "_");
        }
#endif
        return result;
    }
};

#endif // FILEHELPER_H
