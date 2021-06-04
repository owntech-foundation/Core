
# Custom settings, as referred to as "extra_script" in platformio.ini
#
# See https://docs.platformio.org/en/latest/projectconf/section_env_advanced.html#extra-scriptss

Import("env")

env.Append( CXXFLAGS=[ "-Wno-register -fno-rtti" ] )
