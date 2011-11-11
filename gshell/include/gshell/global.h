#ifndef GSHELL_GLOBAL
#define GSHELL_GLOBAL

#include <QtCore/qglobal.h>

#if defined(GSHELL_LIBRARY)
#  define G_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define G_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
