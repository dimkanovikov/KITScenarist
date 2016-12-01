#ifndef WEBLOADERGLOBAL_H
#define WEBLOADERGLOBAL_H

#include <QtCore/QtGlobal>

# ifdef Q_OS_WIN
# define Q_DECL_EXPORT __declspec(dllexport)
# define Q_DECL_IMPORT __declspec(dllimport)
# elif defined(QT_VISIBILITY_AVAILABLE)
# define Q_DECL_EXPORT __attribute__((visibility("default")))
# define Q_DECL_IMPORT __attribute__((visibility("default")))
# define Q_DECL_HIDDEN __attribute__((visibility("hidden")))
# endif

#if defined(WEBLOADER_LIBRARY)
# define WEBLOADER_EXPORT Q_DECL_EXPORT
#else
# define WEBLOADER_EXPORT Q_DECL_IMPORT
#endif

#endif // WEBLOADERGLOBAL_H
