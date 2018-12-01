
include(/home/felix/projects/gl3w/gl3w.pri)

INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/xgl/shader \
    $$PWD/include/xgl/texture \
    $$PWD/include/xgl/vertexarray \
    $$PWD/include/xgl/program \
    $$PWD/include/xgl/buffer \
    $$PWD/include/xgl/all \
    $$PWD/include/xgl/detail/program_uniforms.h

xgl_use_glm: {
	DEFINES += XGL_ENABLE_GLM
}
