// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#version 450 core

in vec4 position;

uniform mat4 model_view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * model_view * position;
}
