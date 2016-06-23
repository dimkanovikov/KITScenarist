#ifndef MYTHES_GLOBAL_H
#define MYTHES_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MYTHES_LIBRARY)
#  define MYTHESSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MYTHESSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MYTHES_GLOBAL_H
