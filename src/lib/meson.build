dep_libcrypto = dependency('libcrypto')

lib=library('lib',
	'FileHash.cpp',
	'FileInfo.cpp',
	'State.cpp',
	dependencies: [dep_libcrypto],
	install: true
)
dep_lib=declare_dependency(
	link_with : lib,
	include_directories : '.'
)