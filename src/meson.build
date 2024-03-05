project(
    'crazytrace',
    'cpp',
    license: ['GPL-3.0-or-later'],
    default_options: [
        # 'buildtype=release',
        'optimization=2',
        'warning_level=3',
        # 'werror=true',
        'cpp_std=c++20',
        'debug=true'
    ]
)

cpp_compiler = meson.get_compiler('cpp')

libtuntap_dep = dependency('libtuntap', required: false)
if not libtuntap_dep.found()
    libtuntap_dep = cpp_compiler.find_library('libtuntap', has_headers: ['tuntap.h'], required: true)
endif

boost_dep = dependency('boost', modules : ['log'])
libtins_dep = dependency('libtins')

executable(
    'crazytrace',
    ['main.cpp', 'tun_tap.cpp', 'crazytrace.cpp'],
    dependencies: [libtuntap_dep, boost_dep, libtins_dep]
)
