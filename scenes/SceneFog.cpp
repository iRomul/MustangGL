#include "SceneFog.h"

SceneFog::SceneFog(glm::vec4 color, float density, GLenum fogMode) {
    glFogi(GL_FOG_MODE, fogMode);
    glFogfv(GL_FOG_COLOR, &color[0]);
    glFogf(GL_FOG_DENSITY, density);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogf(GL_FOG_START, 0.0f);
    glFogf(GL_FOG_END, 20.0f);
}

void SceneFog::enable() {
    glEnable(GL_FOG);
}
