#ifndef COUNTER
#define COUNTER

class QString;
class QTextDocument;


namespace BusinessLogic
{
	/**
	 * @brief Класс счётчика
	 */
	class Counter
	{
	public:
		Counter() : m_words(0), m_charactersWithSpaces(0), m_charactersWithoutSpaces(0) {}

		/**
		 * @brief Количество слов
		 */
		int words() const { return m_words; }
		void setWords(int _count) {
			if (m_words != _count) {
				m_words = _count;
			}
		}
		void addWords(int _count) {
			m_words += _count;
		}

		/**
		 * @brief Количество символов с пробелами
		 */
		int charactersWithSpaces() const { return m_charactersWithSpaces; }
		void setCharactersWithSpaces(int _count) {
			if (m_charactersWithSpaces != _count) {
				m_charactersWithSpaces = _count;
			}
		}
		void addCharactersWithSpaces(int _count) {
			m_charactersWithSpaces += _count;
		}

		/**
		 * @brief Количество символов без пробелов
		 */
		int charactersWithoutSpaces() const { return m_charactersWithoutSpaces; }
		void setCharactersWithoutSpaces(int _count) {
			if (m_charactersWithoutSpaces != _count) {
				m_charactersWithoutSpaces = _count;
			}
		}
		void addCharactersWithoutSpaces(int _count) {
			m_charactersWithoutSpaces += _count;
		}

	private:
		/**
		 * @brief Количество слов
		 */
		int m_words;

		/**
		 * @brief Количество символов с пробелами
		 */
		int m_charactersWithSpaces;

		/**
		 * @brief Количество символов без пробелов
		 */
		int m_charactersWithoutSpaces;
	};

	/**
	 * @brief Проверить равенство двух счётчиков
	 */
	inline bool operator ==(const Counter& lhs, const Counter& rhs)
	{
		return lhs.words() == rhs.words()
				&& lhs.charactersWithSpaces() == rhs.charactersWithSpaces()
				&& lhs.charactersWithoutSpaces() == rhs.charactersWithoutSpaces();
	}

	/**
	 * @brief Проверить неравенство двух счётчиков
	 */
	inline bool operator !=(const Counter& lhs, const Counter& rhs)
	{
		return !(lhs == rhs);
	}
}

#endif // COUNTER

