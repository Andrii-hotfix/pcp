TEMPLATE        = app
LANGUAGE        = C++
SOURCES         = qmc_indom.cpp
CONFIG          += qt warn_on
release:DESTDIR	= build/debug
debug:DESTDIR	= build/release
INCLUDEPATH     += ../../../src/include
INCLUDEPATH     += ../../../src/libpcp_qmc/src
LIBS            += -L../../../src/libpcp/src
LIBS            += -L../../../src/libpcp_qmc/src
LIBS            += -L../../../src/libpcp_qmc/src/$$DESTDIR
LIBS            += -lpcp_qmc -lpcp
QT		-= gui
QMAKE_CFLAGS	+= $$(PCP_CFLAGS) $$(CFLAGS)
QMAKE_CXXFLAGS	+= $$(PCP_CFLAGS) $$(CXXFLAGS)
QMAKE_LFLAGS	+= $$(LDFLAGS)
