#ifndef COLORHELPER_H
#define COLORHELPER_h

#include <QColor>

/**
 * @brief Вспомогательные функции для работы с цветами
 */
class ColorHelper {
public:
    /**
     * @brief Получить цвет для курсора соавтора
     */
    static QColor cursorColor(int _index) {
        QColor color;
        switch (_index % 8) {
            case 0: color = Qt::red; break;
            case 1: color = Qt::darkGreen; break;
            case 2: color = Qt::blue; break;
            case 3: color = Qt::darkCyan; break;
            case 4: color = Qt::magenta; break;
            case 5: color = Qt::darkMagenta; break;
            case 6: color = Qt::darkRed; break;
            case 7: color = Qt::darkYellow; break;
        }
        return color;
    }
};

#endif //COLORHELPER_H
