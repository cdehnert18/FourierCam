shader_file_names = [
  'vertex.glsl',
  'fragment.glsl',
  'funnyFragment.glsl',
  'dftFragment.glsl'
]

shader_files = files(shader_file_names)

shader_target_dir = join_paths(meson.project_build_root(), 'src/view/shader/shaderfiles')

shader_copy_targets = []

foreach i : range(4)
  shader_copy_targets += custom_target(
    'copy_shader_' + shader_file_names[i].replace('.', '_'),
    input: shader_files[i],
    output: shader_file_names[i],  # output includes extension
    command: ['cp', '@INPUT@', join_paths(shader_target_dir, shader_file_names[i])],
    build_by_default: true
  )
endforeach


shader_copy_targets
