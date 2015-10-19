#ifndef SCENARIOTEXTEDITHELPERS_H
#define SCENARIOTEXTEDITHELPERS_H

#include <QString>


namespace Helpers
{
	/**
	 * @brief Получить текст закрывающего блока для группирующих элементов
	 */
	QString footerText(const QString& _headerText);
}

#endif // SCENARIOTEXTEDITHELPERS_H
