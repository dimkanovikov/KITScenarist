#ifndef DIFFMATCHPATCHHELPER
#define DIFFMATCHPATCHHELPER

#include "DiffMatchPatch.h"
#include "TextEditHelper.h"

#include <QHash>
#include <QString>
#include <QRegularExpression>

namespace {
	/**
	 * @brief Является ли строка тэгом
	 */
	static bool isTag(const QString& _tag) {
		return _tag.startsWith("<") && _tag.endsWith(">");
	}

	/**
	 * @brief Является ли тэг открывающим
	 */
	static bool isOpenTag(const QString& _tag) {
		return isTag(_tag) && !_tag.isEmpty() && !_tag.contains("/");
	}

	/**
	 * @brief Является ли тэг закрывающим
	 */
	static bool isCloseTag(const QString& _tag) {
		return isTag(_tag) && !_tag.isEmpty() && _tag.contains("/");
	}

	/**
	 * @brief Удалить все xml-тэги сценария
	 */
	QString removeCommonXmlTagsForScenario(const QString& _xml) {
		QString result = _xml;
		result = result.remove("<?xml version=\"1.0\"?>\n");
		result = result.remove("<scenario>\n");
		result = result.remove("</scenario>\n");
		result = result.remove("<scene_group>\n");
		result = result.remove("</scene_group>\n");
		result = result.remove("<folder>\n");
		result = result.remove("</folder>\n");
		return result;
	}

	/**
	 * @brief Удалить все xml-тэги сценария
	 */
	QString removeXmlTagsForScenario(const QString& _xml) {
		QString result = _xml;
		result = removeCommonXmlTagsForScenario(result);
		result = TextEditHelper::removeXmlTags(result);
		return result;
	}
}


/**
 * @brief Класс со вспомогательными функциями для сравнения xml-текстов
 */
class DiffMatchPatchHelper
{
public:
	/**
	 * @brief Сформировать патч между двумя простыми текстами
	 */
	static QString makePatch(const QString& _text1, const QString& _text2) {
		diff_match_patch dmp;
		return dmp.patch_toText(dmp.patch_make(_text1, _text2));
	}

	/**
	 * @brief Сформировать патч между двумя xml-текстами
	 */
	static QString makePatchXml(const QString& _xml1, const QString& _xml2) {
		return
				plainToXml(
					makePatch(
						xmlToPlain(_xml1),
						xmlToPlain(_xml2)
						)
					);
	}

	/**
	 * @brief Применить патч для простого текста
	 */
	static QString applyPatch(const QString& _text, const QString& _patch) {
		diff_match_patch dmp;
		QList<Patch> patches = dmp.patch_fromText(_patch);
		return dmp.patch_apply(patches, _text).first;
	}

	/**
	 * @brief Применить патч для xml-текста
	 */
	static QString applyPatchXml(const QString& _xml, const QString& _patch) {
		return
				plainToXml(
					applyPatch(
						xmlToPlain(_xml),
						xmlToPlain(_patch)
						)
					);
	}

	/**
	 * @brief Изменение xml
	 */
	class ChangeXml {
	public:
		/**
		 * @brief Сам xml
		 */
		QString xml;

		/**
		 * @brief Позиция изменения
		 */
		int plainPos;

		/**
		 * @brief Длина текста
		 */
		int plainLength;

		ChangeXml() : plainPos(-1), plainLength(-1) {}
		ChangeXml(const QString& _xml, const int _pos, const int _length = -1) :
			xml(_xml), plainPos(_pos), plainLength(_length)
		{
			if (_length == -1) {
				plainLength = ::removeXmlTagsForScenario(xml).length();
			}
		}
	};

	/**
	 * @brief Определить куски xml из документов, которые затрагивает данное изменение
	 * @return Пара: 1) текст, который был изменён; 2) текст замены
	 */
	static QPair<ChangeXml, ChangeXml> changedXml(const QString& _xml, const QString& _patch) {
		//
		// Применим патчи
		//
		const QString oldXml = xmlToPlain(_xml);
		const QString newXml = xmlToPlain(applyPatchXml(_xml, _patch));

		//
		// Формируем новый патч, он будет содержать корректные данные,
		// для текста сценария текущего пользователя
		//
		const QString newPatch = makePatch(oldXml, newXml);

		QPair<ChangeXml, ChangeXml> result;
		if (!newPatch.isEmpty()) {
			//
			// Разберём патчи на список
			//
			diff_match_patch dmp;
			QList<Patch> patches = dmp.patch_fromText(newPatch);

			//
			// Рассчитаем метрики для формирования xml для обновления
			//
			int oldStartPos = -1;
			int oldEndPos = -1;
			int oldDistance = 0;
			int newStartPos = -1;
			int newEndPos = -1;
			foreach (const Patch& patch, patches) {
				//
				// ... для старого
				//
				if (oldStartPos == -1
					|| patch.start1 < oldStartPos) {
					oldStartPos = patch.start1 - oldDistance;
				}
				if (oldEndPos == -1
					|| oldEndPos < (patch.start1 + patch.length1)) {
					oldEndPos = patch.start1 + patch.length1 - oldDistance;
				}
				oldDistance += patch.length2 - patch.length1;
				//
				// ... для нового
				//
				if (newStartPos == -1
					|| patch.start2 < newStartPos) {
					newStartPos = patch.start2;
				}
				if (newEndPos == -1
					|| newEndPos < (patch.start2 + patch.length2)) {
					newEndPos = patch.start2 + patch.length2;
				}
			}
			//
			// Отнимает один символ, т.к. в патче указан индекс символа начиная с 1
			//
			oldEndPos -= 1;
			newEndPos -= 1;


			//
			// Определим кусок xml из текущего документа для обновления
			//
			int oldStartPosForXml = oldStartPos;
			for (; oldStartPosForXml > 0; --oldStartPosForXml) {
				//
				// Идём до открывающего тега
				//
				if (isOpenTag(tagsMap().key(oldXml.at(oldStartPosForXml)))) {
					break;
				}
			}
			int oldEndPosForXml = oldEndPos;
			for (; oldEndPosForXml < oldXml.length(); ++oldEndPosForXml) {
				//
				// Идём до закрывающего тэга, он находится в конце строки
				//
				if (isCloseTag(tagsMap().key(oldXml.at(oldEndPosForXml)))) {
					++oldEndPosForXml;
					break;
				}
			}
			const QString oldXmlForUpdate = oldXml.mid(oldStartPosForXml, oldEndPosForXml - oldStartPosForXml);


			//
			// Определим кусок из нового документа для обновления
			//
			int newStartPosForXml = newStartPos;
			for (; newStartPosForXml > 0; --newStartPosForXml) {
				//
				// Идём до открывающего тега
				//
				if (isOpenTag(tagsMap().key(newXml.at(newStartPosForXml)))) {
					break;
				}
			}
			int newEndPosForXml = newEndPos;
			for (; newEndPosForXml < newXml.length(); ++newEndPosForXml) {
				//
				// Идём до закрывающего тэга, он находится в конце строки
				//
				if (isCloseTag(tagsMap().key(newXml.at(newEndPosForXml)))) {
					++newEndPosForXml;
					break;
				}
			}
			const QString newXmlForUpdate = newXml.mid(newStartPosForXml, newEndPosForXml - newStartPosForXml);


			//
			// Определим позиции xml в тексте без тэгов
			//
			const QString oldXmlPart = oldXml.left(oldStartPosForXml);
			const int oldXmlPartLength = oldXmlPart.length();
			const int oldPlainPartLength = ::removeXmlTagsForScenario(plainToXml(oldXmlPart)).length();
			int oldStartPosForPlain = oldStartPosForXml - (oldXmlPartLength - oldPlainPartLength);

			//
			const QString newXmlPart = newXml.left(newStartPosForXml);
			const int newXmlPartLength = newXmlPart.length();
			const int newPlainPartLength = ::removeXmlTagsForScenario(plainToXml(newXmlPart)).length();
			int newStartPosForPlain = newStartPosForXml - (newXmlPartLength - newPlainPartLength);

			result =
					QPair<ChangeXml, ChangeXml>(
						ChangeXml(plainToXml(oldXmlForUpdate), oldStartPosForPlain),
						ChangeXml(plainToXml(newXmlForUpdate), newStartPosForPlain)
						);
		}

		return result;
	}

private:
	/**
	 * @brief Преобразовать xml в плоский текст, заменяя тэги спецсимволами
	 */
	static QString xmlToPlain(const QString& _xml) {
		QString plain = _xml;
		foreach (const QString& key, tagsMap().keys()) {
			plain.replace(key, tagsMap().value(key));
		}
		return removeCommonXmlTagsForScenario(plain);
	}

	/**
	 * @brief Преобразовать плоский текст в xml, заменяя спецсимволы на тэги
	 */
	static QString plainToXml(const QString& _plain) {
		QString xml = _plain;
		foreach (const QString& value, tagsMap().values()) {
			xml.replace(value, tagsMap().key(value));
		}
		return xml;
	}

	/**
	 * @brief Добавить тэг и его закрывающий аналог в карту соответствий
	 */
	static void addTag(const QString& _tag, QHash<QString, QString>& _tagsMap, int& _index) {
		//
		// Для карты соответсвия используем символы юникода, которые врядли будут использоваться
		//
		_tagsMap.insert("<" + _tag + ">", QChar(_index++));
		_tagsMap.insert("</" + _tag + ">", QChar(_index++));
	}

	/**
	 * @brief Карта соответствий xml-тэгов и спецсимволов
	 */
	static const QHash<QString,QString>& tagsMap() {
		static QHash<QString,QString> s_tagsMap;
		static int s_charIndex = 44032;
		if (s_tagsMap.isEmpty()) {
			addTag("time_and_place", s_tagsMap, s_charIndex);
			addTag("scene_characters", s_tagsMap, s_charIndex);
			addTag("action", s_tagsMap, s_charIndex);
			addTag("character", s_tagsMap, s_charIndex);
			addTag("parenthetical", s_tagsMap, s_charIndex);
			addTag("dialog", s_tagsMap, s_charIndex);
			addTag("transition", s_tagsMap, s_charIndex);
			addTag("note", s_tagsMap, s_charIndex);
			addTag("title", s_tagsMap, s_charIndex);
			addTag("noprintable_text", s_tagsMap, s_charIndex);
			addTag("scene_group", s_tagsMap, s_charIndex);
			addTag("scene_group_header", s_tagsMap, s_charIndex);
			addTag("scene_group_footer", s_tagsMap, s_charIndex);
			addTag("folder", s_tagsMap, s_charIndex);
			addTag("folder_header", s_tagsMap, s_charIndex);
			addTag("folder_footer", s_tagsMap, s_charIndex);

			s_tagsMap.insert("<![CDATA[", QChar(s_charIndex++));
			s_tagsMap.insert("]]>", QChar(s_charIndex++));
		}
		return s_tagsMap;
	}
};

#endif // DIFFMATCHPATCHHELPER

