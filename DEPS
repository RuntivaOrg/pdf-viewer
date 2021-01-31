use_relative_paths = True

gclient_gn_args_file = 'pdfium/build/config/gclient_args.gni'
gclient_gn_args = [
  'checkout_google_benchmark',
]

vars = {
  # By default, we should check out everything needed to run on the main
  # chromium waterfalls. This var can be also be set to 'small', in order
  # to skip things are not strictly needed to build chromium for development
  # purposes.
  'checkout_configuration': 'default',

  # By default, do not check out Google Benchmark. This only exists to satisfy
  # V8-enabled builds that require this variable. Running Google Benchmark is
  # not supported with PDFium.
  'checkout_google_benchmark': False,

  'checkout_instrumented_libraries': 'checkout_linux and checkout_configuration != "small"',

  'chromium_git': 'https://chromium.googlesource.com',
  'pdfium_git': 'https://pdfium.googlesource.com',

  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_ndk
  # and whatever else without interference from each other.
  'pdfium': '25f33d0bf5c7eefa1fbf16811870b02d705f2aa5',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_ndk
  # and whatever else without interference from each other.
  'android_ndk_revision': '27c0a8d090c666a50e40fceb4ee5b40b1a2d3f87',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling build
  # and whatever else without interference from each other.
  'build_revision': '781f0025b9b62fcdfcf066e3fc320841405e3e3a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling buildtools
  # and whatever else without interference from each other.
  'buildtools_revision': 'c38b5ab1c6f5a4913afe49709f7acde15229f400',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling catapult
  # and whatever else without interference from each other.
  'catapult_revision': 'e174329bab1589f8df20965fcb9dde4428673a64',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling clang
  # and whatever else without interference from each other.
  'clang_revision': '683dad6035f2348869736d1d44471825b7bc6766',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling clang_dsymutil
  # and whatever else without interference from each other.
  'clang_dsymutil_revision': 'M56jPzDv1620Rnm__jTMYS62Zi8rxHVq7yw0qeBFEgkC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling code_coverage
  # and whatever else without interference from each other.
  'code_coverage_revision': '6b21d11a706baa083fcedb293b1ac4eded16d65a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling depot_tools
  # and whatever else without interference from each other.
  'depot_tools_revision': '3253a1deca587cc8dd5d83d790cea8f9654e2c33',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling freetype
  # and whatever else without interference from each other.
  'freetype_revision': '03ceda9701cd8c08ea5b4ee0c2d558a98fc4ed7d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling gtest
  # and whatever else without interference from each other.
  'gtest_revision': '1b0cdaae57c046c87fb99cb4f69c312a7e794adb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling icu
  # and whatever else without interference from each other.
  'icu_revision': '899pdfiumr else without interference from each other.
  'jpeg_turbo_revision': '518d81558c797486e125e37cb529d65b560a6ea0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling markupsafe
  # and whatever else without interference from each other.
  'markupsafe_revision': '0944e71f4b2cb9a871bcbe353f95e889b64a611a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling nasm_source
  # and whatever else without interference from each other.
  'nasm_source_revision': '19f3fad68da99277b2882939d3b2fa4c4b8d51d9',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling pdfium_tests
  # and whatever else without interference from each other.
  'pdfium_tests_revision': 'db1faf0ee5ab4e8be140dc1c0fc0031ab70a817c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling skia
  # and whatever else without interference from each other.
  'skia_revision': '81bfabeb18e661dcbd7f9920c14f02a53826c4e2',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling tools_memory
  # and whatever else without interference from each other.
  'tools_memory_revision': '71214b910decfe2e7cfc8b0ffc072a1b97da2d36',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling trace_event
  # and whatever else without interference from each other.
  'trace_event_revision': '9b27757731a454d6e76f3d1153ae62d603a16897',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling v8
  # and whatever else without interference from each other.
  'v8_revision': '1d681a1a9f5b66ce7c18d756f80e1c3afab911d5',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling zlib
  # and whatever else without interference from each other.
  'zlib_revision': '2c183c9f93a328bfb3121284da13cf89a0f7e64a',
}

deps = {
  'pdfium':
    Var('pdfium_git') + '/pdfium.git@' +
      Var('pdfium'),

  'pdfium/base/trace_event/common':
    Var('chromium_git') + '/chromium/src/base/trace_event/common.git@' +
        Var('trace_event_revision'),

  'pdfium/build':
    Var('chromium_git') + '/chromium/src/build.git@' + Var('build_revision'),

  'pdfium/buildtools':
    Var('chromium_git') + '/chromium/src/buildtools.git@' +
        Var('buildtools_revision'),

  'pdfium/testing/corpus':
    Var('pdfium_git') + '/pdfium_tests@' + Var('pdfium_tests_revision'),

  'pdfium/third_party/android_ndk': {
    'url': Var('chromium_git') + '/android_ndk.git@' +
        Var('android_ndk_revision'),
    'condition': 'checkout_android',
  },

  'pdfium/third_party/catapult': {
    'url': Var('chromium_git') + '/catapult.git@' + Var('catapult_revision'),
    'condition': 'checkout_android',
  },

  'pdfium/third_party/depot_tools':
    Var('chromium_git') + '/chromium/tools/depot_tools.git@' +
        Var('depot_tools_revision'),

  'pdfium/third_party/freetype/src':
    Var('chromium_git') + '/chromium/src/third_party/freetype2.git@' +
        Var('freetype_revision'),

  'pdfium/third_party/googletest/src':
    Var('chromium_git') + '/external/github.com/google/googletest.git@' +
        Var('gtest_revision'),

  'pdfium/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu.git@' + Var('icu_revision'),

  'pdfium/third_party/instrumented_libraries':
    Var('chromium_git') +
        '/chromium/src/third_party/instrumented_libraries.git@' +
        Var('instrumented_lib_revision'),

  'pdfium/third_party/jinja2':
    Var('chromium_git') + '/chromium/src/third_party/jinja2.git@' +
        Var('jinja2_revision'),

  'pdfium/third_party/libjpeg_turbo':
    Var('chromium_git') + '/chromium/deps/libjpeg_turbo.git@' +
        Var('jpeg_turbo_revision'),

  'pdfium/third_party/markupsafe':
    Var('chromium_git') + '/chromium/src/third_party/markupsafe.git@' +
        Var('markupsafe_revision'),

  'pdfium/third_party/nasm':
    Var('chromium_git') + '/chromium/deps/nasm.git@' +
        Var('nasm_source_revision'),

  'pdfium/third_party/skia':
    Var('chromium_git') + '/skia.git@' +  Var('skia_revision'),

  'pdfium/third_party/zlib':
    Var('chromium_git') + '/chromium/src/third_party/zlib.git@' +
        Var('zlib_revision'),

  'pdfium/tools/clang':
    Var('chromium_git') + '/chromium/src/tools/clang@' +  Var('clang_revision'),

  'pdfium/tools/clang/dsymutil': {
    'packages': [
      {
        'package': 'chromium/llvm-build-tools/dsymutil',
        'version': Var('clang_dsymutil_revision'),
      }
    ],
    'condition': 'checkout_mac or checkout_ios',
    'dep_type': 'cipd',
  },

  'pdfium/tools/code_coverage':
    Var('chromium_git') + '/chromium/src/tools/code_coverage.git@' +
        Var('code_coverage_revision'),

  'pdfium/tools/memory':
    Var('chromium_git') + '/chromium/src/tools/memory@' +
        Var('tools_memory_revision'),

  'pdfium/v8':
    Var('chromium_git') + '/v8/v8.git@' + Var('v8_revision'),
}

recursedeps = [
  # buildtools provides clang_format, libc++, and libc++abi
  'pdfium/buildtools',
]

include_rules = [
  # Basic stuff that everyone can use.
  # Note: public is not here because core cannot depend on public.
  '+pdfium/build/build_config.h',
  '+pdfium/constants',
  '+pdfium/testing',
  '+pdfium/third_party/base',
]

specific_include_rules = {
  # Allow embedder tests to use public APIs.
  '(.*embeddertest\.cpp)': [
    '+public',
  ]
}

hooks = [
  {
    # Ensure that the DEPS'd "depot_tools" has its self-update capability
    # disabled.
    'name': 'disable_depot_tools_selfupdate',
    'pattern': '.',
    'action': [ 'python',
                'pdfium/third_party/depot_tools/update_depot_tools_toggle.py',
                '--disable',
    ],
  },
  {
    # Case-insensitivity for the Win SDK. Must run before win_toolchain below.
    'name': 'ciopfs_linux',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "linux"',
    'action': [ 'python',
                'pdfium/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/ciopfs',
                '-s', 'build/ciopfs.sha1',
    ]
  },
  {
    # Update the Windows toolchain if necessary.  Must run before 'clang' below.
    'name': 'win_toolchain',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': ['python', 'build/vs_toolchain.py', 'update', '--force'],
  },
  {
    # Update the Mac toolchain if necessary.
    'name': 'mac_toolchain',
    'pattern': '.',
    'condition': 'checkout_mac',
    'action': ['python', 'pdfium/build/mac_toolchain.py'],
  },
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'python',
                'pdfium/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'buildtools/win/clang-format.exe.sha1',
    ],
  },
  {
    'name': 'clang_format_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'python',
                'pdfium/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'pdfium/buildtools/mac/clang-format.sha1',
    ],
  },
  {
    'name': 'clang_format_linux',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'python',
                'pdfium/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'pdfium/buildtools/linux64/clang-format.sha1',
    ],
  },
  {
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'action': ['python',
               'pdfium/tools/clang/scripts/update.py'
    ],
  },
  {
    'name': 'sysroot_arm',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm',
    'action': ['python', 'pdfium/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm'],
  },
  {
    'name': 'sysroot_arm64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm64',
    'action': ['python', 'pdfium/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm64'],
  },
  {
    'name': 'sysroot_x86',
    'pattern': '.',
    'condition': 'checkout_linux and (checkout_x86 or checkout_x64)',
    'action': ['python', 'pdfium/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x86'],
  },
  {
    'name': 'sysroot_mips',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_mips',
    'action': ['python', 'pdfium/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=mips'],
  },
  {
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_x64',
    'action': ['python', 'pdfium/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },
  {
    'name': 'msan_chained_origins',
    'pattern': '.',
    'condition': 'checkout_instrumented_libraries',
    'action': [ 'python',
                'pdfium/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-instrumented-libraries',
                '-s', 'pdfium/third_party/instrumented_libraries/binaries/msan-chained-origins-trusty.tgz.sha1',
              ],
  },
  {
    'name': 'msan_no_origins',
    'pattern': '.',
    'condition': 'checkout_instrumented_libraries',
    'action': [ 'python',
                'pdfium/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-instrumented-libraries',
                '-s', 'pdfium/third_party/instrumented_libraries/binaries/msan-no-origins-trusty.tgz.sha1',
              ],
  },
  {
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python', 'pdfium/build/util/lastchange.py',
               '-o', 'pdfium/build/util/LASTCHANGE'],
  },
]
