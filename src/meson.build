project(
    'crazytrace',
    'cpp',
    license: ['GPL-3.0-or-later'],
    default_options: [
        'buildtype=release',
        'optimization=2',
        'warning_level=3',
        'werror=true',
        'cpp_std=c++20'
    ]
)

sources = files(
    'configuration.cpp',
    'crazytrace.cpp',
    'main.cpp',
    'nodecontainer.cpp',
    'nodeinfo.cpp',
    'nodereply.cpp',
    'noderequest.cpp',
    'randomgenerator.cpp',
    'tun_tap.cpp'
)

cpp_compiler = meson.get_compiler('cpp')

if not cpp_compiler.has_header('unistd.h')
    error('crazytrace requires unistd.h')
endif

libtuntap_dep = dependency('libtuntap', required: false)
if not libtuntap_dep.found()
    libtuntap_dep = cpp_compiler.find_library('libtuntap', has_headers: ['tuntap.h'], required: true)
endif

if not libtuntap_dep.found()
    error('crazytrace requires libtuntap')
endif

boost_dep = dependency('boost', modules : ['log'])
libtins_dep = dependency('libtins')
yamlcpp_dep = dependency('yaml-cpp')

cppcheck = find_program('cppcheck', required: false)
if cppcheck.found()
    run_target('cppcheck', command: [
        cppcheck,
        # Do not check external libraries
        '--suppress=*:*/tins/*',
        '--suppress=*:*/boost/*',

        # Problems with detecting system libraries
        '--suppress=missingIncludeSystem',

        # That's okay for me
        '--suppress=uninitMemberVar',

        '--enable=warning,style,information,missingInclude',
        '--error-exitcode=1',
        '--project=' + join_paths(meson.build_root(), 'compile_commands.json')
    ])
endif

flawfinder = find_program('flawfinder', required: false)
if flawfinder.found()
    run_target('flawfinder', command: [
        flawfinder,
        '--error-level=0',
        sources
    ])
endif

executable(
    'crazytrace',
    sources,
    dependencies: [libtuntap_dep, boost_dep, libtins_dep, yamlcpp_dep],
    install : true
)
