TEMPLATE = lib
CONFIG       += debug_and_release

CONFIG       += unversioned_libname
CONFIG       += skip_target_version_ext

include( $$(TONATIUH_ROOT)/config.pri )


DEFINES += STATISTICSLIBRARY_EXPORTS

QT -=core	
QT -=gui	

CONFIG(debug, debug|release) {
	OBJECTS_DIR = $$(TONATIUH_ROOT)/debug
	MOC_DIR = $$(TONATIUH_ROOT)/debug
	OBJMOC = $$(TONATIUH_ROOT)/debug
	RCC_DIR = $$(TONATIUH_ROOT)/debug
}
else { 
	OBJECTS_DIR = $$(TONATIUH_ROOT)/release
	MOC_DIR = $$(TONATIUH_ROOT)/release
	OBJMOC = $$(TONATIUH_ROOT)/release
	RCC_DIR  = $$(TONATIUH_ROOT)/release
}
	

TARGET = statistics   

DEPENDPATH += . \
                $$(TONATIUH_ROOT)

 
# Input
HEADERS += *.h \

SOURCES += *.cpp \
          
 

CONFIG(debug, debug|release) {
	DESTDIR = $$(TONATIUH_ROOT)/bin/debug
}
else{
	DESTDIR= $$(TONATIUH_ROOT)/bin/release
}
