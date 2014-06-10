#include "SpellChecker.h"

#include <hunspell/hunspell.hxx>

#include <QApplication>
#include <QTextCodec>
#include <QTextStream>
#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QFile>


SpellChecker::SpellChecker(const QString& _userDictionaryPath) :
	m_spellingLanguage(SpellChecker::Undefined),
	m_checker(0),
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
		QString affDictionary = dictionaryFilePath(m_spellingLanguage, Affinity);
		QString dicDictionary = dictionaryFilePath(m_spellingLanguage, Dictionary);

		//
		// Создаём нового проверяющего
		//
		m_checker = new Hunspell(affDictionary.toUtf8().constData(),
								 dicDictionary.toUtf8().constData());
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

bool SpellChecker::spellCheckWord(const QString& _word) const
{
	//
	// Преобразуем слово в кодировку словаря и осуществим проверку
	//
	QByteArray encodedWord = m_checkerTextCodec->fromUnicode(_word);
	return m_checker->spell(encodedWord.constData());
}

QStringList SpellChecker::suggestionsForWord(const QString& _word) const
{
	QStringList suggestions;

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

	return suggestions;
}

void SpellChecker::ignoreWord(const QString& _word) const
{
	//
	// Добавим слово в словарный запас проверяющего
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
		if (userDictonaryFile.open(QIODevice::Append)) {
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

QString SpellChecker::dictionaryFilePath(
		SpellChecker::Language _language,
		SpellChecker::DictionaryType _dictionaryType) const
{
	//
	// Словари хранятся в файлах ресурсов, но для ханспела нужны реальные файлы
	// поэтому, сохраняем файл из ресурсов на диск
	//
	const QString rcFilePath = ":/SpellChecking/HunspellDictionaries/";
	QString fileName;

	//
	// Получим файл со словарём в зависимости от выбранного языка,
	// по-умолчанию используется русский язык
	//
	switch (_language) {
		case Russian:
		default:
			fileName += "ru_RU";
			break;
		case RussianWithYo:
			fileName += "ru_RU_yo";
			break;
		case Ukrainian:
			fileName += "uk_UA";
			break;
		case Belorussian:
			fileName += "be_BY";
			break;
	}

	//
	// Определим расширение файла, в зависимости от словаря
	//
	fileName += _dictionaryType == Affinity ? ".aff" : ".dic";

	//
	// Сохраним словарь на диск во папку программы, если такового ещё нет
	//
	// ... определяемся с именем файла
	//
	QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString hunspellDictionariesFolderPath = appDataFolderPath + QDir::separator() + "Hunspell";
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
	//
	// Преобразуем слово в кодировку словаря и добавляем его в словарный запас
	//
	QByteArray encodedWord = m_checkerTextCodec->fromUnicode(_word);
	m_checker->add(encodedWord.constData());
}
