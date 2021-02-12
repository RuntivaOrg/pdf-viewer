use_relative_paths = True

vars = {
  'skia_git': 'https://skia.googlesource.com',
  'github_url': 'https://github.com',

  #
  # TODO: The values below need to be kept in sync
  # between //DEPS and //runtiva-pdf/DEPS, so if you're updating one,
  # update the other.
  #

  # When changing these, also update the svn revisions in deps_revisions.gni
  'sdl_revision':       '03b83bacd1f6f815d9f2b7d1493f5ac40e153edc', # release-2.0.14
  'sdl_ttf_revision':   'c0295db5fbf0e06148e47be0e7fc37ac8703162b', # release-2.0.15
  'sdl_image_revision': 'b4f28c1fc9ab2f91ae594fc3e86622c5c13ffbbb', # release-2.0.5
}

deps = {
  'pdf-viewer/third_party/externals/sdl':
    Var('skia_git') + '/third_party/sdl@' +
        Var('sdl_revision'),

  'pdf-viewer/third_party/externals/SDL_ttf':
    Var('github_url') + '/SDL-mirror/SDL_ttf.git@' +
        Var('sdl_ttf_revision'),

  'pdf-viewer/third_party/externals/SDL_image':
    Var('github_url') + '/SDL-mirror/SDL_image.git@' +
        Var('sdl_image_revision'),
}