shader_file_names = [
'vertex.glsl',
'fragment.glsl',
'funnyFragment.glsl',
'dftFragment.glsl'
]

shader_files = files(shader_file_names)

shader_target_dir = join_paths(meson.project_build_root(), 'src/view/shader/shaderfiles')

shader_copy_targets = []

i = 0
foreach name : shader_file_names
	shader_copy_targets += custom_target(
		'copy_shader_' + name.replace('.', '_'),
		input: shader_files[i],
		output: name,
		command: ['cp', '@INPUT@', join_paths(shader_target_dir, name)],
		build_by_default: true
	)
	i += 1
endforeach

shader_copy_targets