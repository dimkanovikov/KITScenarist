#ifndef SCENARIOTEXTEDITPRIVATE_H
#define SCENARIOTEXTEDITPRIVATE_H

#include "ScenarioTextEdit.h"

class QShortcut;
class QWidget;

namespace UserInterface
{
	/**
	 * @brief Класс для управления шорткатами блоков редактора сценария
	 */
	class ShortcutsManager : public QObject
	{
		Q_OBJECT

	public:
		ShortcutsManager(ScenarioTextEdit* _editor);

		/**
		 * @brief Считать значения горячих клавиш из настроек
		 */
		void update();

		/**
		 * @brief Получить шорткат для блока
		 */
		QString shortcut(int _forBlockType) const;

	private slots:
		/**
		 * @brief Сменить стиль текущего блока редактора на заданный
		 */
		void changeTextBlock(int _blockType) const;

	private:
		/**
		 * @brief Редактор сценария
		 */
		ScenarioTextEdit* m_editor;

		/**
		 * @brief Тип блока - горячие клавиши
		 */
		QHash<int, QShortcut*> m_shortcuts;
	};
}

#endif // SCENARIOTEXTEDITPRIVATE_H
