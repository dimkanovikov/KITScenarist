#include "SpellChecker.h"

#include <3rd_party/Hunspell/hunspell.hxx>

#include <QApplication>
#include <QTextCodec>
#include <QTextStream>
#include <QTemporaryFile>
#include <QStringList>


SpellChecker::SpellChecker(const QString& _userDictionaryPath) :
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
	//
	// Удаляем предыдущего проверяющего
	//
	delete m_checker;
	m_checker = 0;

	//
	// Получаем пути к файлам словарей
	//
	QString affDictionary = dictionaryFilePath(_spellingLanguage, Affinity);
	QString dicDictionary = dictionaryFilePath(_spellingLanguage, Dictionary);

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

QString SpellChecker::dictionaryFilePath(
		SpellChecker::Language _language,
		SpellChecker::DictionaryType _dictionaryType) const
{
	//
	// Словари хранятся в файлах ресурсов, но для ханспела нужны реальные файлы
	// поэтому, сохраняем файл из ресурсов на диск
	//
	QString filePath = ":/SpellChecking/HunspellDictionaries/";

	//
	// Получим файл со словарём в зависимости от выбранного языка,
	// по-умолчанию используется русский язык
	//
	switch (_language) {
		case Russian:
		default:
			filePath += "ru_RU";
			break;
		case EnglishAU:
			filePath += "en_AU";
			break;
		case EnglishGB:
			filePath += "en_GB";
			break;
		case EnglishUS:
			filePath += "en_US";
			break;
	}

	//
	// Определим расширение файла, в зависимости от словаря
	//
	filePath += _dictionaryType == Affinity ? ".aff" : ".dic";

	//
	// Сохраним словарь на диск во временный файл
	//
	QTemporaryFile* tempFile = new QTemporaryFile(qApp);
	if (tempFile->open()) {
		QFile resourseFile(filePath);
		resourseFile.open(QIODevice::ReadOnly);
		//
		tempFile->write(resourseFile.readAll());
		//
		resourseFile.close();
		tempFile->close();
	}

	return tempFile->fileName();
}

void SpellChecker::addWordToChecker(const QString& _word) const
{
	//
	// Преобразуем слово в кодировку словаря и добавляем его в словарный запас
	//
	QByteArray encodedWord = m_checkerTextCodec->fromUnicode(_word);
	m_checker->add(encodedWord.constData());
}
