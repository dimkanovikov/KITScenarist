#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QMap>
#include <QSet>
#include <QString>

class Hunspell;
class QStringList;
class QTextCodec;


/**
 * @brief Класс проверяющего орфографию
 */
class SpellChecker
{
public:
    /**
     * @brief Возможные языки для проверки орфографии
     */
    enum Language {
        Undefined,
        Russian,
        RussianWithYo,
        Ukrainian,
        Belorussian,
        EnglishGB,
        EnglishUS,
        Spanish,
        French,
        Kazakh,
        ArmenianEastern,
        ArmenianWestern,
        Portuguese,
        PortugueseBrazilian,
        Swedish
    };

    /**
     * @brief Получить код заданного словаря
     */
    static QString languageCode(Language _language);

    /**
     * @brief Синглтон
     */
    static SpellChecker* createSpellChecker(const QString& _userDictionaryPath = QString::null);

public:
    ~SpellChecker();

    /**
     * @brief Установить язык для проверки орфографии
     */
    void setSpellingLanguage(SpellChecker::Language _spellingLanguage);

    /**
     * @brief Проверить орфографию слова
     * @param Слово для проверки
     * @return Корректность орфографии в слове
     */
    bool spellCheckWord(const QString& _word) const;

    /**
     * @brief Получить список близких слов (вариантов исправления ошибки)
     * @param Некоректное слово, для которого ищется список
     * @return Список близких слов
     */
    QStringList suggestionsForWord(const QString& _word) const;

//	/**
//	 * @brief Получить список синонимов для заданного слова
//	 */
//	QMap<QString, QSet<QString> > synonimsForWord(const QString& _word) const;

    /**
     * @brief Игнорировать слово
     * @param Слово, проверку которого необходимо игнорировать
     */
    void ignoreWord(const QString& _word) const;

    /**
     * @brief Добавить слово в словарь
     * @param Слово, которое необходимо добавить в словарь
     */
    void addWordToDictionary(const QString& _word) const;

    /**
     * @brief Получить текущий язык проверки орфографии
     */
    Language spellingLanguage() const;

private:
    SpellChecker(const QString& _userDictionaryPath);

    /**
     * @brief Тип словаря
     */
    enum FileType {
        Affinity,
        Indexes,
        Dictionary
    };

    /**
     * @brief Получить путь к файлу со словарём
     * @param Язык словаря
     * @param Тип словаря
     * @return Путь к файлу словаря
     */
    QString hunspellFilePath(
            SpellChecker::Language _language,
            SpellChecker::FileType _fileType) const;

    /**
     * @brief Получить путь к файлу со словарём
     * @param Язык словаря
     * @param Тип словаря
     * @return Путь к файлу словаря
     */
    QString mythesFilePath(
            SpellChecker::Language _language,
            SpellChecker::FileType _fileType) const;

    /**
     * @brief Добавить слово в словарный запас проверяющего
     * @param Слово для добавления
     *
     * @note После добавления слово считается корректным до удаления объекта проверяющего.
     *		 Для того, чтобы слово всегда считалось корректным его нужно добавить в
     *		 пользовательский словарь.
     */
    void addWordToChecker(const QString& _word) const;

private:
    /**
     * @brief Синглтон
     */
    static SpellChecker* s_spellChecker;

    /**
     * @brief Текущий язык проверки орфографии
     */
    Language m_spellingLanguage;

    /**
     * @brief Объект проверяющий орфографию
     */
    Hunspell* m_checker;

    /**
     * @brief Кодировка, которую использует проверяющий
     */
    QTextCodec* m_checkerTextCodec;

    /**
     * @brief Путь к файлу со словарём пользователя
     */
    QString m_userDictionaryPath;
};

#endif // SPELLCHECKER_H
