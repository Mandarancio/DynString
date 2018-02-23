project('DybString', 'c',
  version : '0.1',
  license : 'GPL3'
)


PROJECT_NAME = 'dynstring'
TEST_NAME = 'test_dynstring'

sources = [
    'dynstring.c'
]

test_srouces = [
  'test/test.c'
]

deps = [
  dependency('check')
]

shared_library(
    PROJECT_NAME,
    sources: [sources],
    dependencies : [],
    install: true
)

executable(
    TEST_NAME,
    sources: [test_srouces, sources,],
    c_args: ['-I../'],
    dependencies : deps,
    link_args: '-lcheck',
    install: false
)
