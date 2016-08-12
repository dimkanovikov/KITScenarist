#include "SpellChecker.h"

#include <hunspell/hunspell.hxx>
//#include <mythes.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextStream>


QString SpellChecker::languageCode(SpellChecker::Language _language)
{
	QString code;
	switch (_language) {
		case Russian:
		default:
			code = "ru_RU";
			break;
		case RussianWithYo:
			code = "ru_RU_yo";
			break;
		case ArmenianEastern:
			code = "arm_ARM_east";
			break;
		case ArmenianWestern:
			code = "arm_ARM_west";
			break;
		case Belorussian:
			code = "be_BY";
			break;
		case EnglishGB:
			code = "en_GB";
			break;
		case EnglishUS:
			code = "en_US";
			break;
		case French:
			code = "fr_FR";
			break;
		case Kazakh:
			code = "kk_KZ";
			break;
		case Spanish:
			code = "es_ES";
			break;
		case Ukrainian:
			code = "uk_UA";
			break;
	}

	return code;
}

SpellChecker::SpellChecker(const QString& _userDictionaryPath) :
	m_spellingLanguage(SpellChecker::Undefined),
	m_checker(0),
	m_checkerTextCodec(0),
	m_userDictionaryPath(_userDictionaryPath)
{
	//
	// Настроим проверяющего
	//
	setSpellingLanguage(SpellChecker::Russian);
}

SpellChecker::~SpellChecker()
{
	delete m_checker;
	m_checker = 0;
}

void SpellChecker::setSpellingLanguage(SpellChecker::Language _spellingLanguage)
{
	if (m_spellingLanguage != _spellingLanguage) {
		m_spellingLanguage = _spellingLanguage;

		//
		// Удаляем предыдущего проверяющего
		//
		delete m_checker;
		m_checker = 0;

        //
        // Получаем пути к файлам словарей
        //
        QString affDictionary = hunspellFilePath(m_spellingLanguage, Affinity);
        QString dicDictionary = hunspellFilePath(m_spellingLanguage, Dictionary);

        const QFileInfo affFileInfo(affDictionary);
        const QFileInfo dicFileInfo(dicDictionary);
        if (affFileInfo.exists() && affFileInfo.size() > 0
            && dicFileInfo.exists() && dicFileInfo.size() > 0) {
            //
            // Создаём нового проверяющего
            //
            m_checker = new Hunspell(affDictionary.toLocal8Bit().constData(),
                                     dicDictionary.toLocal8Bit().constData());
            m_checkerTextCodec = QTextCodec::codecForName(m_checker->get_dic_encoding());

            //
            // Проверяющий обязательно должен быть создан
            //
            Q_ASSERT(m_checker);

            //
            // Загружаем слова из пользовательского словаря
            //
            if (!m_userDictionaryPath.isNull()) {
                QFile userDictonaryFile(m_userDictionaryPath);
                if (userDictonaryFile.open(QIODevice::ReadOnly)) {
                    QTextStream stream(&userDictonaryFile);
                    for(QString word = stream.readLine();
                        !word.isEmpty();
                        word = stream.readLine()) {
                        addWordToChecker(word);
                    }
                    userDictonaryFile.close();
                }
            }
        }
	}
}

bool SpellChecker::spellCheckWord(const QString& _word) const
{
    bool spelled = false;
    if (m_checker != 0) {
        //
        // Преобразуем слово в кодировку словаря и осуществим проверку
        //
        QByteArray encodedWordData = m_checkerTextCodec->fromUnicode(_word);
        const char* encodedWord = encodedWordData.constData();
        spelled = m_checker->spell(encodedWord);
    }
    return spelled;
}

QStringList SpellChecker::suggestionsForWord(const QString& _word) const
{
	QStringList suggestions;

    if (m_checker != 0) {
        QByteArray encodedWordData = m_checkerTextCodec->fromUnicode(_word);
        const char* encodedWord = encodedWordData.constData();
        //
        // Проверяем необходимость получения списка вариантов
        //
        if (!m_checker->spell(encodedWord)) {
            //
            // Получим массив вариантов
            //
            char ** suggestionsArray;
            int suggestionsCount = m_checker->suggest(&suggestionsArray, encodedWord);
            if (suggestionsCount > 0)
            {
                //
                // Преобразуем массив вариантов в список строк
                //
                for (int suggestionIndex = 0; suggestionIndex < suggestionsCount; suggestionIndex++) {
                    suggestions.append(m_checkerTextCodec->toUnicode(suggestionsArray[suggestionIndex]));
                }
                //
                // Освобождаем память
                //
                m_checker->free_list(&suggestionsArray, suggestionsCount);
            }
        }
    }

	return suggestions;
}

//QMap<QString, QSet<QString> > SpellChecker::synonimsForWord(const QString& _word) const
//{
//	QMap<QString, QSet<QString> > thesaurusEntries;

//	QByteArray encodedWordData = m_checkerTextCodec->fromUnicode(_word);
//	const char* encodedWord = encodedWordData.constData();
//	int info = 0;
//	char* root = "";
//	//
//	// Если в слове нет ошибок, получим его основную форму
//	//
//	if (m_checker->spell(encodedWord, &info, &root)) {
//		QString wordRoot = m_checkerTextCodec->toUnicode(root);
//		if (wordRoot.isEmpty()) {
//			wordRoot = _word;
//		}

//		//
//		// Загружаем тезаурус
//		//
//		const QString indexesPath = mythesFilePath(m_spellingLanguage, Indexes);
//		const QString dictionaryPath = mythesFilePath(m_spellingLanguage, Dictionary);
//		MyThes thesaurus(indexesPath.toUtf8().constData(), dictionaryPath.toUtf8().constData());
//		QTextCodec* thesTextCodec = QTextCodec::codecForName(thesaurus.get_th_encoding());

//		//
//		// Получим синонимы
//		//
//		const QByteArray wordRootData = thesTextCodec->fromUnicode(wordRoot);
//		mentry* entries;
//		const int entriesCount = thesaurus.Lookup(wordRootData.constData(), wordRootData.length(), &entries);
//		if (entriesCount > 0) {
//			for (int entryIndex = 0; entryIndex < entriesCount; ++entryIndex) {
//				mentry entry = entries[entryIndex];
//				const QString entryDefinition = thesTextCodec->toUnicode(entry.defn);
//				QSet<QString> entryItems;
//				if (thesaurusEntries.contains(entryDefinition)) {
//					entryItems = thesaurusEntries.value(entryDefinition);
//				}

//				for (int entryItemIndex = 0; entryItemIndex < entry.count; ++entryItemIndex) {
//					const QString entryItem = thesTextCodec->toUnicode(entry.psyns[entryItemIndex]);
//					entryItems.insert(entryItem);
//				}

//				thesaurusEntries[entryDefinition] = entryItems;
//			}

//			//
//			// Очищаем всю выделенную тезаурусом память
//			//
//			thesaurus.CleanUpAfterLookup(&entries, entriesCount);
//		}
//	}

//	return thesaurusEntries;
//}

void SpellChecker::ignoreWord(const QString& _word) const
{
	//
	// Добавим слово в словарный запас проверяющего на текущую сессию
	//
	addWordToChecker(_word);
}

void SpellChecker::addWordToDictionary(const QString& _word) const
{
	//
	// Добавим слово в словарный запас проверяющего
	//
	addWordToChecker(_word);

	//
	// Запишем слово в пользовательский словарь
	//
	if (!m_userDictionaryPath.isNull()) {
		QFile userDictonaryFile(m_userDictionaryPath);
		if (userDictonaryFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
			QTextStream stream(&userDictonaryFile);
			stream << _word << "\n";
			userDictonaryFile.close();
		}
	}
}

SpellChecker::Language SpellChecker::spellingLanguage() const
{
	return m_spellingLanguage;
}

QString SpellChecker::hunspellFilePath(SpellChecker::Language _language,
		SpellChecker::FileType _fileType) const
{
	//
	// Получим файл со словарём в зависимости от выбранного языка,
	// по-умолчанию используется русский язык
	//
	QString fileName = languageCode(_language);

	//
	// Определим расширение файла, в зависимости от словаря
	//
	fileName += _fileType == Affinity ? ".aff" : ".dic";

	//
	// Сохраним словарь на диск во папку программы, если такового ещё нет
	//
	// ... определяемся с именем файла
	//
	QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString hunspellDictionariesFolderPath = appDataFolderPath + QDir::separator() + "Hunspell";
	QString dictionaryFilePath = hunspellDictionariesFolderPath + QDir::separator() + fileName;
	return dictionaryFilePath;
}

QString SpellChecker::mythesFilePath(SpellChecker::Language _language, FileType _fileType) const
{
	//
	// Словари хранятся в файлах ресурсов, но для ханспела нужны реальные файлы
	// поэтому, сохраняем файл из ресурсов на диск
	//
	const QString rcFilePath = ":/Thesaurus/MyThesDictionaries/";
	QString fileName;

	//
	// Получим файл со словарём в зависимости от выбранного языка,
	// по-умолчанию используется русский язык
	//
	fileName += languageCode(_language);
//	switch (_language) {
//		default:
//		case Russian:
//		case RussianWithYo:
//		case Belorussian:
//			fileName += "ru";
//			break;
//		case Ukrainian:
//			fileName += "uk";
//			break;
//		case EnglishGB:
//		case EnglishUS:
//			fileName += "en";
//			break;
//		case Spanish:
//			fileName += "es";
//			break;
//		case French:
//			fileName += "fr";
//			break;
//	}

	//
	// Определим расширение файла, в зависимости от словаря
	//
	fileName += _fileType == Indexes ? ".idx" : ".dat";

	//
	// Сохраним словарь на диск во папку программы, если такового ещё нет
	//
	// ... определяемся с именем файла
	//
	QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString hunspellDictionariesFolderPath = appDataFolderPath + QDir::separator() + "MyThes";
	QString dictionaryFilePath = hunspellDictionariesFolderPath + QDir::separator() + fileName;
	//
	// ... создаём папку для пользовательских файлов
	//
	QDir rootFolder = QDir::root();
	rootFolder.mkpath(hunspellDictionariesFolderPath);
	//
	//  создаём файл если такого ещё нет
	//
	if (!QFile::exists(dictionaryFilePath)) {
		QFile resourseFile(rcFilePath + fileName);
		resourseFile.open(QIODevice::ReadOnly);
		//
		QFile dictionaryFile(dictionaryFilePath);
		dictionaryFile.open(QIODevice::WriteOnly);
		dictionaryFile.write(resourseFile.readAll());
		//
		resourseFile.close();
		dictionaryFile.close();
	}

	//
	// TODO: логирование корректности записи файла
	//

	return dictionaryFilePath;
}

void SpellChecker::addWordToChecker(const QString& _word) const
{
    if (m_checker != 0) {
        //
        // Преобразуем слово в кодировку словаря и добавляем его в словарный запас
        //
        QByteArray encodedWord = m_checkerTextCodec->fromUnicode(_word);
        m_checker->add(encodedWord.constData());
    }
}
